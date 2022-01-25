/* 
 * Temperature Monitor
 * 
 * Records the ATTiny85 internal temperature every hour, logged in EEPROM.
 * When a button is pressed the last temperature is displayed in a large font.
 * A graph of the last five days readings is shown over the readout.
 * The graph's y axis range is from -10 to 50 degrees C.
 * 
 * When using Spence Konde's ATTinyCore https://github.com/SpenceKonde/ATTinyCore
 * and using a chip without Optiboot, with LTO enabled, this sketch takes 5572 bytes of flash.
 * 
 * The temperature is measured roughly every hour
 * and the measured temperatues are stored in EEPROM
 * 
 * When a button is pressed, the display shows a graph of the latest readings.
 * 
 * Power on:
 * check eeprom is set up to record log of temperatures
 * record current temperature
 * set up wake up interrupts on button press and watch dog timer
 * set up the screen and display the graph and latest temperature
 * Go to sleep
 * 
 * Every 8 seconds the watch dog timer wakes up the device
 * If the number of wake-ups means an hour has passed,
 * then the current temperature is logged.
 * If the screen has been on for 2 wake-ups then turn off the screen.
 * Go to sleep
 * 
 * If a button is pressed, the screen is turned on,
 * and the device goes back to sleep.
 */

#include <avr/sleep.h>
#include <EEPROM.h>
#include <TinyI2CMaster.h>
#include <Tiny4kOLED.h>
#include "eeprom_contents.h"

// Large font containing only digits.
// TinyOLED-Fonts library needed.
// https://github.com/datacute/TinyOLED-Fonts
#include "Sansita_Swashed_Regular_57_Digits.h"

// Routines to set and clear bits (used in the sleep code)
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

static bool eepromOk = false;
static uint8_t currentContrast = TEMPERATURE_DEFAULT_CONTRAST;

static uint8_t currentAddress = TEMPERATURE_READINGS_ADDRESS;

#define MAIN_BUTTON 4
#define ENTER_BUTTON 3

// Variables for the Sleep/power down modes:
volatile boolean f_wdt = 0;
static int wdt_counter = 0;
static int graph_display_counter = 0;

// The watch dog timer wakes up the device every 8 seconds
// The device graphs the last 120 readings
// 10800 * 8 seconds = 1 day. 120 days = 4 months, mark every 7th or 30th
// 450 * 8 seconds = 1 hour. 120 hours = 5 days, mark every 24th
// 75 * 8 seconds = 10 minutes. 1200 minutes = 20 hours, mark every 6th
// 15 * 8 seconds = 2 minutes. 240 minutes = 4 hours, mark every 30th
// 1 * 8 seconds = 8 seconds. 960 seconds = 16 minutes, mark every 38th
#define REPORT_PERIOD 450
#define GRAPH_TICK_COUNT 24

#define TEMP_CALIBRATION_OFFSET 288.0f
#define TEMP_CALIBRATION_GAIN 1.0f

// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  f_wdt=1;  // set global flag
}

ISR(PCINT0_vect) {}

ISR(ADC_vect) {
// ADC conversion complete
}

void setup() {
  setupInputs();
  setupOLED();
  setupFromEEPROM();
  setupWDT();
  oled.setContrast(currentContrast);
  displayGraph();
}

void setupInputs() {
  pinMode(MAIN_BUTTON, INPUT_PULLUP);
  pinMode(ENTER_BUTTON, INPUT_PULLUP);
  GIMSK = 1 << PCIE; // Enable pin-change interrupt
  PCMSK = (1 << MAIN_BUTTON) | (1 << ENTER_BUTTON);
}

void setupOLED(void) {
  oled.begin(128, 64, sizeof(tiny4koled_init_128x64br), tiny4koled_init_128x64br);
  oled.clear();
  oled.setFont(FONTSANSITASWASHEDREGULAR57DIGITS);
}

static void setupFromEEPROM() {
  eepromOk = EEPROM.read(TEMPERATURE_MAGIC_ADDRESS    ) == TEMPERATURE_MAGIC0 &&
             EEPROM.read(TEMPERATURE_MAGIC_ADDRESS + 1) == TEMPERATURE_MAGIC1 &&
             EEPROM.read(TEMPERATURE_MAGIC_ADDRESS + 2) == TEMPERATURE_MAGIC2 &&
             EEPROM.read(TEMPERATURE_MAGIC_ADDRESS + 3) == TEMPERATURE_MAGIC3;
  if (eepromOk) {
    currentContrast = EEPROM.read(TEMPERATURE_CONTRAST_ADDRESS);
    currentAddress = EEPROM.read(TEMPERATURE_CURRENT_ADDRESS);
    recordTemperature(); // Write a new value at power-on
  } else {
    settingsResetAction();
  }
}

static void setupWDT(void) {
  int ii = 9;
  byte bb;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  //bb|= (1<<WDCE);
  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}

void loop() {
  if (!processButtonInputs()) {
    system_sleep();
  }
  if (f_wdt == 1) {
    f_wdt=0;
    wdt_counter++;
    if (wdt_counter == REPORT_PERIOD) {
      wdt_counter = 0;
      recordTemperature();
    }
    if (graph_display_counter > 0) {
      graph_display_counter--;
      if (graph_display_counter == 0) {
        oled.off();
      }
    }
  }
}

bool processButtonInputs(void) {
  bool mainButtonIsDown = digitalRead(MAIN_BUTTON) == LOW;
  if (mainButtonIsDown) {
    displayGraph();
  }
  bool enterButtonIsDown = digitalRead(ENTER_BUTTON) == LOW;
  if (enterButtonIsDown) {
    displayGraph();
  }
  return mainButtonIsDown || enterButtonIsDown;
}

static uint8_t ticks[] = {
  0x04,
  0x10,
  0x40,
  0x00,
  0x01,
  0x04,
  0x10,
  0x40
  };

static uint8_t overlayGraph(uint8_t x, uint8_t y, uint8_t b) {
  int itemAddress = currentAddress + x - 2;
  if (itemAddress >= 128) itemAddress -= (128 - TEMPERATURE_READINGS_ADDRESS);
  uint8_t bottomBit = (7-y) << 3;
  int8_t reading = EEPROM.read(itemAddress);
  //if (reading < 0) reading = 0;
  //if (reading > 60) reading = 60;
  uint8_t v = (reading * 2); // mask out pixel below graph
  if ((v >= bottomBit) && (v < (bottomBit + 8))) {
    b &= ~(1 << (7 - (v - bottomBit)));
  }
  v++; // Shift graph up 1 pixel above ticks line
  if ((v >= bottomBit) && (v < (bottomBit + 8))) {
    b |= 1 << (7 - (v - bottomBit));
  }
  v++;// mask out pixel above graph
  if ((v >= bottomBit) && (v < (bottomBit + 8))) {
    b &= ~(1 << (7 - (v - bottomBit)));
  }
  return b;
}

static void displayGraph() {
  int8_t reading;
  for (uint8_t line = 0; line < 8; line++) {
    uint8_t bottomBit = (7-line) << 3;
    oled.setCursor(0,line);
    oled.startData();
    oled.sendData(ticks[line]);
    oled.sendData(0xFF);
    int itemAddress = currentAddress; // oldest reading
    uint8_t tick_column = 0;
    do {
      reading = EEPROM.read(itemAddress);
      //if (reading < 0) reading = 0;
      //if (reading > 60) reading = 60;
      uint8_t v = (reading * 2) + 1; // Shift graph up 1 pixel above ticks line
      uint8_t b = 0;
      if ((v >= bottomBit) && (v < (bottomBit + 8))) {
        b = 1 << (7 - (v - bottomBit));
      }
      if (line == 7) {
        b |= 0x40;
        tick_column++;
        if (tick_column == GRAPH_TICK_COUNT) {
          b |= 0x80;
          tick_column = 0;
        }
      }
      oled.sendData(b);
      itemAddress++;
      if (itemAddress >= 128) itemAddress = TEMPERATURE_READINGS_ADDRESS;
    } while (itemAddress != currentAddress);
    oled.sendData(0xFF);
    oled.sendData(ticks[line]);
    oled.endData();
  }
  // This can be made more efficient by not drawing the following large portion of the graph twice.
  oled.setCursor(27,1);
  oled.setCombineFunction(&overlayGraph);
  oled.print(reading);
  oled.setCombineFunction(NULL);
  oled.on();
  graph_display_counter = 2;
}
/*
static void setContrast(void) {
  oled.setContrast(currentContrast);
  if (eepromOk) {
    EEPROM.write(TEMPERATURE_CONTRAST_ADDRESS, currentContrast);
  }
}
*/
void settingsResetAction(void) {
  // EEPROM Header
  for (uint16_t offset = 0; offset < sizeof(header); offset++) {
    EEPROM.write(offset, pgm_read_byte(&header[offset]));
  }
  // Default all readings to current temperature value
  byte result = readTemperature();

  for (uint16_t address = sizeof(header); address < 128; address++) {
    EEPROM.write(address, result);
  }
  eepromOk = true;
  currentAddress = TEMPERATURE_READINGS_ADDRESS;
  currentContrast = TEMPERATURE_DEFAULT_CONTRAST;
}

static void recordTemperature(void) {
  int8_t result = readTemperature();

  EEPROM.write(currentAddress, (byte)result);
  currentAddress++;
  if (currentAddress >= 128) {
    currentAddress = TEMPERATURE_READINGS_ADDRESS;
  }
  EEPROM.write(TEMPERATURE_CURRENT_ADDRESS, currentAddress);
}

static int8_t readTemperature(void) {
	return getChipTemperatureCelsius();
}

/* below code based on https://github.com/jordan-public/Thermometer-Attiny85/blob/master/Thermometer-Attiny85.ino */

// From: http://21stdigitalhome.blogspot.com/2014/10/trinket-attiny85-internal-temperature.html
int8_t getChipTemperatureCelsius() {
  int8_t t_celsius;
  uint16_t rawTemp;
/*  
  // Measure temperature
  ADCSRA |= _BV(ADEN);           // Enable AD and start conversion
  ADMUX = 0xF | _BV( REFS1 );    // ADC4 (Temp Sensor) and Ref voltage = 1.1V;
  delay(100);                 // Settling time min 1 ms, wait 100 ms

  rawTemp = (float)getADC();     // use next sample as initial average
  ADCSRA &= ~(_BV(ADEN));        // disable ADC
*/
  GIMSK &= ~(_BV(PCIE)); // Disable pin-change interrupt
  WDTCR &= ~(_BV(WDIE));
  ADCSRA = 1<<ADEN | 1<<ADIE | 7<<ADPS0;   // Enable ADC, interrupt, 62.5kHz clock
  ADMUX = 0xF | _BV( REFS1 );  // Temperature and 1.1V reference
  //ADCSRB = 0<<ADTS0;                       // Free running
  delay(100);                 // Settling time min 1 ms, wait 100 ms
//
  set_sleep_mode(SLEEP_MODE_ADC);
  sleep_mode();
  rawTemp = ADC;
//
//  rawTemp = (float)getADC();
  ADCSRA &= ~(_BV(ADEN));        // disable ADC
  ADCSRA &= ~(_BV(ADIE));        // disable ADC interrupt
//
  GIMSK |= _BV(PCIE); // Enable pin-change interrupt again
  WDTCR |= _BV(WDIE);
  t_celsius = chipTemp(rawTemp);  
                                                                                   
  return t_celsius;
}

// Calibration of the temperature sensor has to be changed for your own ATtiny85
// per tech note: http://www.atmel.com/Images/doc8108.pdf
int8_t chipTemp(uint16_t raw) {
  float converted = (((float)raw - TEMP_CALIBRATION_OFFSET) * TEMP_CALIBRATION_GAIN);
  if (converted > 60) converted = 60;
  if (converted < 0) converted = 0;
  return (int8_t)converted;
}
 
// Common code for both sources of an ADC conversion
int getADC() {
  ADCSRA  |=_BV(ADSC);           // Start conversion
  while((ADCSRA & _BV(ADSC)));    // Wait until conversion is finished
  return ADC;
}

/* above code based on https://github.com/jordan-public/Thermometer-Attiny85/blob/master/Thermometer-Attiny85.ino */

void system_sleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  //sleep_enable();
  sleep_mode();                        // System actually sleeps here
  //sleep_disable();                     // System continues execution here when watchdog timed out 
}

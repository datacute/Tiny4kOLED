/* 
 * Power Monitor
 * 
 * Displays a visualisation of the battery level.
 * 
 * When using Spence Konde's ATTinyCore https://github.com/SpenceKonde/ATTinyCore
 * and using a chip without Optiboot, with LTO enabled, this sketch takes 5758 bytes of flash.
 * 
 * The battery level is measured roughly every hour
 * and the measured voltages are stored in EEPROM
 * 
 * When a button is pressed, the display shows a graph of the latest readings.
 * 
 * Power on:
 * set up buttons
 * Display current screen
 * turn on screen
 * Go to sleep
 * 
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
static uint8_t currentContrast = BATTERY_DEFAULT_CONTRAST;

static uint8_t currentAddress = BATTERY_READINGS_ADDRESS;

#define MAIN_BUTTON 4
#define ENTER_BUTTON 1

// Variables for the Sleep/power down modes:
volatile boolean f_wdt = 0;
static int wdt_counter = 0;
static int graph_display_counter = 0;

// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  f_wdt=1;  // set global flag
}

ISR(PCINT0_vect) {}

void setup() {
  setupADC();
  setupInputs();
  setupOLED();
  setupFromEEPROM();
  setupWDT();
  oled.setContrast(currentContrast);
}

static void setupADC() {
  ADMUX = 0b1100<<MUX0;        // Vcc Ref, Measure bandgap voltage (1.1)
  //ADCSRA = 1<<ADEN | 4<<ADPS0; // Enable, 62.5kHz ADC clock (16x prescalar)
  ADCSRA = 4<<ADPS0; // Disabled, 62.5kHz ADC clock (16x prescalar)
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
  eepromOk = EEPROM.read(BATTERY_MAGIC_ADDRESS    ) == BATTERY_MAGIC0 &&
             EEPROM.read(BATTERY_MAGIC_ADDRESS + 1) == BATTERY_MAGIC1 &&
             EEPROM.read(BATTERY_MAGIC_ADDRESS + 2) == BATTERY_MAGIC2 &&
             EEPROM.read(BATTERY_MAGIC_ADDRESS + 3) == BATTERY_MAGIC3;
  if (eepromOk) {
    currentContrast = EEPROM.read(BATTERY_CONTRAST_ADDRESS);
    currentAddress = EEPROM.read(BATTERY_CURRENT_ADDRESS);
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
    if (wdt_counter == 450) { // 450 for 1 hour
      wdt_counter = 0;
      recordVoltage();
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
  0x04, // 5.5V to 6.2V
  0x10, // 4.7V to 5.4V
  0x40, // 3.9V to 4.6V
  0x00, // 3.1V to 3.8V
  0x01, // 2.3V to 3.0V
  0x04, // 1.5V to 2.2V
  0x10, // 0.7V to 1.4V
  0x40  // bottom ticks line, 0.0 to 0.6 V
  };

static uint8_t overlayGraph(uint8_t x, uint8_t y, uint8_t b) {
  int itemAddress = currentAddress + x - 2;
  if (itemAddress >= 128) itemAddress -= (128 - BATTERY_READINGS_ADDRESS);
  uint8_t bottomBit = (7-y) << 3;
  uint8_t reading = EEPROM.read(itemAddress);
  uint8_t v = reading; // mask out pixel below graph
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
  uint8_t reading;
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
      uint8_t v = reading + 1; // Shift graph up 1 pixel above ticks line
      uint8_t b = 0;
      if ((v >= bottomBit) && (v < (bottomBit + 8))) {
        b = 1 << (7 - (v - bottomBit));
      }
      if (line == 7) {
        b |= 0x40;
        tick_column++;
        if (tick_column == 24) {
          b |= 0x80;
          tick_column = 0;
        }
      }
      oled.sendData(b);
      itemAddress++;
      if (itemAddress >= 128) itemAddress = BATTERY_READINGS_ADDRESS;
    } while (itemAddress != currentAddress);
    oled.sendData(0xFF);
    oled.sendData(ticks[line]);
    oled.endData();
  }
  // This can be made more efficient by not drawing the following large portion of the graph twice.
  oled.setCursor(27,1);
  oled.setCombineFunction(&overlayGraph);
  oled.print((float)reading/10.0, 1);
  oled.setCombineFunction(NULL);
  oled.on();
  graph_display_counter = 2;
}
/*
static void setContrast(void) {
  oled.setContrast(currentContrast);
  if (eepromOk) {
    EEPROM.write(BATTERY_CONTRAST_ADDRESS, currentContrast);
  }
}
*/
void settingsResetAction(void) {
  // EEPROM Header
  for (uint16_t offset = 0; offset < sizeof(header); offset++) {
    EEPROM.write(offset, pgm_read_byte(&header[offset]));
  }
  // Unused
  for (uint16_t address = sizeof(header); address < 128; address++) {
    EEPROM.write(address, 0);
  }
  eepromOk = true;
  currentAddress = BATTERY_CURRENT_ADDRESS;
  currentContrast = BATTERY_DEFAULT_CONTRAST;
}

static void recordVoltage(void) {
  uint16_t result = readADC();
  //result = 1126400L / result; // Calculate Vcc (in mV); 1.1*1024*1000
  result = 11264L / result; // 10ths of a volt
  EEPROM.write(currentAddress, (byte)result);
  currentAddress++;
  if (currentAddress >= 128) {
    currentAddress = BATTERY_READINGS_ADDRESS;
  }
  EEPROM.write(BATTERY_CURRENT_ADDRESS, currentAddress);
}

static uint16_t readADC() {
  uint8_t low,high;
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON  
  ADCSRA = ADCSRA | 1<<ADSC;           // Start
  do {} while (ADCSRA & 1<<ADSC);      // Wait while conversion in progress
  low = ADCL;
  high = ADCH;
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF
  return ((uint16_t)high<<8 | low);
}

void system_sleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  //sleep_enable();
  sleep_mode();                        // System actually sleeps here
  //sleep_disable();                     // System continues execution here when watchdog timed out 
}

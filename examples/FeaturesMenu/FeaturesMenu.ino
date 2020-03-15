/*
This example shows how to use a large number of the SSD1306 functions.
It is interactive, controlled by three buttons for up, down, and select.
*/

// Choose your I2C implementation before including Tiny4kOLED.h
// The default is selected is Wire.h

// To use the Wire library:
//#include <Wire.h>

// To use the Adafruit's TinyWireM library:
//#include <TinyWireM.h>

// To use the TinyI2C library from https://github.com/technoblogy/tiny-i2c
//#include <TinyI2CMaster.h>

// The blue OLED screen requires a long initialization on power on.
// The code to wait for it to be ready uses 20 bytes of program storage space
// If you are using a white OLED, this can be reclaimed by uncommenting
// the following line (before including Tiny4kOLED.h):
//#define TINY4KOLED_QUICK_BEGIN

#include <Tiny4kOLED.h>
// ============================================================================

#include <avr/sleep.h>
#define adc_disable() (ADCSRA &= -(1<<ADEN))
#define adc_enable()  (ADCSRA |=  (!<<ADEN))

const uint8_t rockerUpPin = 4;
const uint8_t rockerDownPin = 3;
const uint8_t rockerInPin = 1;

bool rockerUp = false;
bool rockerDown = false;
bool rockerIn = false;
bool rockerInChanged = false;

long debounceTime = 0L;
long lastWakeUp = 0L;
long debounceDelay = 100L;

bool switchingSettingLine = true;

bool performFrameSwitch = true;

const long repeatInputInterval = 200L;
long timeChangedInput = 0;

// The screen can show 4 setting lines at once.
uint8_t settingsPage = 0;
bool changedSettingsPage = false;
uint8_t redrawsSettingsPage = 2;

void NoOutput(uint8_t m, uint8_t value);
void OutputNum(uint8_t, uint8_t);
void OutputYN(uint8_t, uint8_t);

void NoAction(uint8_t);

#define CURRENT_SETTING_LINE 0
void OutputCurrentSettingLine(uint8_t, uint8_t);
void CurrentSettingLineChanged(uint8_t);

#define CONTRAST_VALUE (CURRENT_SETTING_LINE+1)
const char CONTRAST_NAME[] PROGMEM = "Contrast";
void ContrastChanged(uint8_t value) {
  oled.setContrast(value);
}

#define INVERSE_VALUE (CONTRAST_VALUE+1)
const char INVERSE_NAME[] PROGMEM = "Inverse";
void InverseChanged(uint8_t value) {
  oled.setInverse(value);
}

#define PIXELS_VALUE (INVERSE_VALUE+1)
const char PIXELS_NAME[] PROGMEM = "Pixels Off/RAM/On";
void PixelsChanged(uint8_t value);

#define VCOMH_DESELECT_VALUE (PIXELS_VALUE+1)
const char VCOMH_DESELECT_NAME[] PROGMEM = "VCOMH Deselect";
void VCOMHDeselectChanged(uint8_t value) {
  oled.setVcomhDeselectLevel(value);
}

#define FREQUENCY_VALUE (VCOMH_DESELECT_VALUE+1)
#define DIVIDE_RATIO_VALUE (FREQUENCY_VALUE+1)
const char FREQUENCY_NAME[] PROGMEM = "Frequency";
const char DIVIDE_RATIO_NAME[] PROGMEM = "Divide Ratio";
void DisplayClockChanged(uint8_t value);

#define PRE_CHARGE_1_VALUE (DIVIDE_RATIO_VALUE+1)
#define PRE_CHARGE_2_VALUE (PRE_CHARGE_1_VALUE+1)
const char PRE_CHARGE_1_NAME[] PROGMEM = "Precharge P1";
const char PRE_CHARGE_2_NAME[] PROGMEM = "Precharge P2";
void PrechargePeriodChanged(uint8_t value);

#define ZOOM_IN_VALUE (PRE_CHARGE_2_VALUE+1)
const char ZOOM_IN_NAME[] PROGMEM = "Zoom In";
void ZoomInChanged(uint8_t value) {
  if (value) {
    oled.enableZoomIn();
  } else {
    oled.disableZoomIn();
  }
}

#define FADE_VALUE (ZOOM_IN_VALUE+1)
const char FADE_NAME[] PROGMEM = "Fade";
void FadeChanged(uint8_t value);

#define BLINK_VALUE (FADE_VALUE+1)
const char BLINK_NAME[] PROGMEM = "Blink";
void BlinkChanged(uint8_t value);

#define FADE_INTERVAL_VALUE (BLINK_VALUE+1)

#define MUX_VALUE (FADE_INTERVAL_VALUE+1)
const char MUX_NAME[] PROGMEM = "MUX Ratio";
void MultiplexRatioChanged(uint8_t value) {
  oled.setMultiplexRatio(value);
}

#define DISPLAY_OFFSET_VALUE (MUX_VALUE+1)
const char DISPLAY_OFFSET_NAME[] PROGMEM = "Display Offset";
void DisplayOffsetChanged(uint8_t value) {
  oled.setDisplayOffset(value);
}

#define SEGMENT_REMAP_VALUE (DISPLAY_OFFSET_VALUE+1)
const char SEGMENT_REMAP_NAME[] PROGMEM = "Segment Remap";
void SegmentRemapChanged(uint8_t value) {
  oled.setSegmentRemap(value);
}

#define SCAN_DIRECTION_VALUE (SEGMENT_REMAP_VALUE+1)
const char SCAN_DIRECTION_NAME[] PROGMEM = "Scan Direction";
void ComOutputDirectionChanged(uint8_t value) {
  oled.setComOutputDirection(value);
}

#define DOUBLE_BUFFER_VALUE (SCAN_DIRECTION_VALUE+1)
const char DOUBLE_BUFFER_NAME[] PROGMEM = "Double Buffer";
void DoubleBufferChanged(uint8_t value) {
  if (value) {
    oled.switchRenderFrame();
  } else {
    if (oled.currentDisplayFrame() > 0) oled.switchDisplayFrame();
    if (oled.currentRenderFrame() > 0) oled.switchRenderFrame();
  }
}

#define MEMORY_MODE_VALUE (DOUBLE_BUFFER_VALUE+1)
const char MEMORY_MODE_NAME[] PROGMEM = "Memory Mode";

#define PAGED_COLUMN_START_VALUE (MEMORY_MODE_VALUE+1)
const char PAGED_COLUMN_START_NAME[] PROGMEM = "P Col Start";
void PagedColumnStartChanged(uint8_t value) {
  oled.setColumnStartAddress(value);
}

#define PAGED_PAGE_START_VALUE (PAGED_COLUMN_START_VALUE+1)
const char PAGED_PAGE_START_NAME[] PROGMEM = "P Page Start";
void PagedPageStartChanged(uint8_t value) {
  oled.setPageStartAddress(value);
}

#define COLUMN_START_VALUE (PAGED_PAGE_START_VALUE+1)
#define COLUMN_END_VALUE (COLUMN_START_VALUE+1)
const char COLUMN_START_NAME[] PROGMEM = "H/V Col Start";
const char COLUMN_END_NAME[] PROGMEM = "H/V Col End";
void ColumnAddressChanged(uint8_t value);

#define PAGE_START_VALUE (COLUMN_END_VALUE+1)
#define PAGE_END_VALUE (PAGE_START_VALUE+1)
const char PAGE_START_NAME[] PROGMEM = "H/V Page Start";
const char PAGE_END_NAME[] PROGMEM = "H/V Page End";
void PageAddressChanged(uint8_t value);

#define SCROLL_DIRECTION_VALUE (PAGE_END_VALUE+1)
#define SCROLL_INTERVAL_VALUE (SCROLL_DIRECTION_VALUE+1)
#define SCROLL_OFFSET_VALUE (SCROLL_INTERVAL_VALUE+1)
const char SCROLL_DIRECTION_NAME[] PROGMEM = "Direction";
const char INTERVAL_NAME[] PROGMEM = "Interval";
const char SCROLL_OFFSET_NAME[] PROGMEM = "Offset";
void OutputScrollDirection(uint8_t, uint8_t);
void OutputScrollInterval(uint8_t, uint8_t);
const uint8_t scrollIntervals[8] =      {7, 4, 5, 6, 0,  1,  2,   3};
const uint8_t scrollIntervalFrames[8] = {2, 3, 4, 5, 6, 32, 64, 128};

#define DEMO_SIZE_VALUE (SCROLL_OFFSET_VALUE+1)
const char DEMO_SIZE_NAME[] PROGMEM = "Demo Size";

#define START_PAGE_VALUE (DEMO_SIZE_VALUE+1)
const char START_PAGE_NAME[] PROGMEM = "H Start Page";

#define END_PAGE_VALUE (START_PAGE_VALUE+1)
const char END_PAGE_NAME[] PROGMEM = "H End Page";

#define START_LINE_VALUE (END_PAGE_VALUE+1)
const char START_LINE_NAME[] PROGMEM = "V Top Line";

#define SCROLL_LINES_VALUE (START_LINE_VALUE+1)
const char SCROLL_LINES_NAME[] PROGMEM = "V Lines";

#define DEMO_ACTIVE_VALUE (SCROLL_LINES_VALUE+1)
const char DEMO_ACTIVE_NAME[] PROGMEM = "Demo On";
void DemoActiveChanged(uint8_t value);

#define CURRENT_VOLTAGE_VALUE (DEMO_ACTIVE_VALUE+1)
const char CURRENT_VOLTAGE_NAME[] PROGMEM = "Vcc";
void OutputCurrentVoltage(uint8_t, uint8_t);

const uint8_t settingLines = CURRENT_VOLTAGE_VALUE;

uint8_t setting_changed = settingLines + 1;

typedef void (*ActionFunc) (uint8_t);
typedef void (*OutputFunc) (uint8_t, uint8_t);

struct {
  PGM_P name;
  const uint8_t min;
  const uint8_t max;
  const ActionFunc actionFn;
  const OutputFunc outputFn;
} const s[settingLines+1] PROGMEM = {
// name                     min max    action                     output
  {NULL,                    1,  settingLines, 
                                       CurrentSettingLineChanged, OutputCurrentSettingLine},
  {CONTRAST_NAME,           0,  255,   ContrastChanged,           OutputNum},
  {INVERSE_NAME,            0,  1,     InverseChanged,            OutputYN},
  {PIXELS_NAME,             0,  2,     PixelsChanged,             NoOutput},
  {VCOMH_DESELECT_NAME,     0,  7,     VCOMHDeselectChanged,      OutputNum},

  {FREQUENCY_NAME,          0,  15,    DisplayClockChanged,       OutputNum},
  {DIVIDE_RATIO_NAME,       1,  16,    DisplayClockChanged,       OutputNum},
  {PRE_CHARGE_1_NAME,       1,  15,    PrechargePeriodChanged,    OutputNum},
  {PRE_CHARGE_2_NAME,       1,  15,    PrechargePeriodChanged,    OutputNum},
 
  {ZOOM_IN_NAME,            0,  1,     ZoomInChanged,             OutputYN},
  {FADE_NAME,               0,  1,     FadeChanged,               OutputYN},
  {BLINK_NAME,              0,  1,     BlinkChanged,              OutputYN},
  {INTERVAL_NAME,           0,  15,    NoAction,                  OutputNum},

  {MUX_NAME,                16, 64,    MultiplexRatioChanged,     OutputNum},
  {DISPLAY_OFFSET_NAME,     0,  63,    DisplayOffsetChanged,      OutputNum},
  {SEGMENT_REMAP_NAME,      0,  1,     SegmentRemapChanged,       OutputYN},
  {SCAN_DIRECTION_NAME,     0,  1,     ComOutputDirectionChanged, OutputYN},

  {DOUBLE_BUFFER_NAME,      0,  1,     DoubleBufferChanged,       OutputYN},
  {MEMORY_MODE_NAME,        0,  2,     NoAction,                  OutputNum},
  {PAGED_COLUMN_START_NAME, 0,  127,   PagedColumnStartChanged,   OutputNum},
  {PAGED_PAGE_START_NAME,   0,  7,     PagedPageStartChanged,     OutputNum},

  {COLUMN_START_NAME,       0,  127,   ColumnAddressChanged,      OutputNum},
  {COLUMN_END_NAME,         0,  127,   ColumnAddressChanged,      OutputNum},
  {PAGE_START_NAME,         0,  7,     PageAddressChanged,        OutputNum},
  {PAGE_END_NAME,           0,  7,     PageAddressChanged,        OutputNum},

  {SCROLL_DIRECTION_NAME,   0,  2,     NoAction,                  OutputScrollDirection},
  {INTERVAL_NAME,           0,  7,     NoAction,                  OutputScrollInterval},
  {SCROLL_OFFSET_NAME,      0,  63,    NoAction,                  OutputNum},
  {DEMO_SIZE_NAME,          0,  255,   NoAction,                  OutputNum},
 
  {START_PAGE_NAME,         0,  7,     NoAction,                  OutputNum},
  {END_PAGE_NAME,           0,  7,     NoAction,                  OutputNum},
  {START_LINE_NAME,         0,  63,    NoAction,                  OutputNum},
  {SCROLL_LINES_NAME,       0,  64,    NoAction,                  OutputNum},

  {DEMO_ACTIVE_NAME,        0,  1,     DemoActiveChanged,         OutputYN},
  {CURRENT_VOLTAGE_NAME,    0,  255,   NoAction,                  OutputCurrentVoltage}
};

uint8_t v[settingLines+1] = {
  1, // CURRENT_SETTING_LINE

  0x7F, // CONTRAST_VALUE
  0,    // INVERSE_VALUE
  1,    // PIXELS_VALUE
  2,    // VCOMH_DESELECT_VALUE

  8,    // FREQUENCY_VALUE
  1,    // DIVIDE_RATIO_VALUE
  2,    // PRE_CHARGE_1_VALUE
  2,    // PRE_CHARGE_2_VALUE
 
  0,    // ZOOM_IN_VALUE
  0,    // FADE_VALUE
  0,    // BLINK_VALUE
  0,    // FADE_INTERVAL_VALUE

  32,   // MUX_VALUE
  0,    // DISPLAY_OFFSET_VALUE
  1,    // SEGMENT_REMAP_VALUE
  1,    // SCAN_DIRECTION_VALUE

  1,    // DOUBLE_BUFFER_VALUE
  2,    // MEMORY_MODE_VALUE
  32,   // PAGED_COLUMN_START_VALUE
  1,    // PAGED_PAGE_START_VALUE

  32,   // COLUMN_START_VALUE
  127,  // COLUMN_END_VALUE
  1,    // PAGE_START_VALUE
  3,    // PAGE_END_VALUE

  0,    // SCROLL_DIRECTION_VALUE
  0,    // SCROLL_INTERVAL_VALUE
  1,    // SCROLL_OFFSET_VALUE
  255,  // DEMO_SIZE_VALUE
 
  1,    // START_PAGE_VALUE
  3,    // END_PAGE_VALUE
  8,    // START_LINE_VALUE
  16,   // SCROLL_LINES_VALUE

  0,    // DEMO_ACTIVE_VALUE
  0     // CURRENT_VOLTAGE_VALUE (Unused)
};

uint8_t r[settingLines+1] = { 0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0 };

void DemoActiveChanged(uint8_t value) {
  if (value) {
    oled.clear();
    if (oled.currentDisplayFrame() > 0) oled.switchDisplayFrame();
    if (oled.currentRenderFrame() > 0) oled.switchRenderFrame();
    uint8_t memMode = v[MEMORY_MODE_VALUE];
    if (memMode == 2) {
      oled.setCursor(v[PAGED_COLUMN_START_VALUE],v[PAGED_PAGE_START_VALUE]);
    } else {
      oled.setCursor(v[COLUMN_START_VALUE],v[PAGE_START_VALUE]);
    }
    oled.setMemoryAddressingMode(memMode);
    for (uint8_t k = 0; k < v[DEMO_SIZE_VALUE]; k++) {
      oled.fillLength(k, 1);
    }
    oled.setMemoryAddressingMode(2);

    uint8_t scrollInterval = scrollIntervals[v[SCROLL_INTERVAL_VALUE]];
    uint8_t scrollDirection = v[SCROLL_DIRECTION_VALUE];
    uint8_t scrollOffset = v[SCROLL_OFFSET_VALUE];
    uint8_t startPage = v[START_PAGE_VALUE];
    uint8_t endPage = v[END_PAGE_VALUE];
    uint8_t topLine = v[START_LINE_VALUE];
    uint8_t scrollLines = v[SCROLL_LINES_VALUE];
    if (scrollDirection == 0) {
      if (scrollOffset == 0) oled.scrollLeft(startPage,scrollInterval,endPage);
      else {
        oled.scrollLeftOffset(startPage,scrollInterval,endPage,scrollOffset);
        oled.setVerticalScrollArea(topLine, scrollLines);
      }
      oled.activateScroll();
    } else if (scrollDirection == 2) {
      if (scrollOffset == 0) oled.scrollRight(startPage,scrollInterval,endPage);
      else {
        oled.scrollRightOffset(startPage,scrollInterval,endPage,scrollOffset);
        oled.setVerticalScrollArea(topLine, scrollLines);
      }
      oled.activateScroll();
    }
  } else {
    oled.deactivateScroll();
    if (oled.currentDisplayFrame() > 0) oled.switchDisplayFrame();
    if (oled.currentRenderFrame() == 0) oled.switchRenderFrame();
    oled.setVerticalScrollArea(0,64);
  }
}

void DisplayClockChanged(__attribute__((unused)) uint8_t value) {
  oled.setDisplayClock(v[DIVIDE_RATIO_VALUE], v[FREQUENCY_VALUE]);
}

void PrechargePeriodChanged(__attribute__((unused)) uint8_t value) {
  oled.setPrechargePeriod(v[PRE_CHARGE_1_VALUE], v[PRE_CHARGE_2_VALUE]);
}

void ColumnAddressChanged(__attribute__((unused)) uint8_t value) {
  oled.setColumnAddress(v[COLUMN_START_VALUE], v[COLUMN_END_VALUE]);
}

void PageAddressChanged(__attribute__((unused)) uint8_t value) {
  oled.setPageAddress(v[PAGE_START_VALUE], v[PAGE_END_VALUE]);
}

void FadeChanged(uint8_t value) {
  if (value) {
    oled.fadeOut(v[FADE_INTERVAL_VALUE]);
  } else {
    oled.disableFadeOutAndBlinking();
  }
}

void BlinkChanged(uint8_t value) {
  if (value) {
    oled.blink(v[FADE_INTERVAL_VALUE]);
  } else {
    oled.disableFadeOutAndBlinking();
  }
}

void sleepWaitingForInput();

void PixelsChanged(uint8_t value) {
  if (value == 0) {
    oled.off();
    sleepWaitingForInput(1<<rockerUpPin);
    oled.on();
  } else if (value == 1) {
    oled.setEntireDisplayOn(false);
  } else {
    oled.setEntireDisplayOn(true);
  }
}

ISR(PCINT0_vect) {}

void setup() {
  setupPins();
  setupOLED();
  setupADC();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void setupPins() {
  pinMode(rockerUpPin, INPUT_PULLUP);
  pinMode(rockerDownPin, INPUT_PULLUP);
  pinMode(rockerInPin, INPUT_PULLUP);
}

void setupOLED() {
  oled.begin();

  // Two rotations are supported,
  // The begin() method sets the rotation to 1.
  //oled.setRotation(0);

  // Some newer devices do not contain an external current reference.
  // Older devices may also support using the internal curret reference,
  // which provides more consistent brightness across devices.
  // The internal current reference can be configured as either low current, or high current.
  // Using true as the parameter value choses the high current internal current reference,
  // resulting in a brighter display, and a more effective contrast setting.
  //oled.setInternalIref(true);

  // Two fonts are supplied with this library, FONT8X16 and FONT6X8
  // Other fonts are available from the TinyOLED-Fonts library
  oled.setFont(FONT6X8);
  oled.clear();
  oled.on();
  oled.switchRenderFrame();
  oled.clear();
}

void loop() {
  if (!performFrameSwitch && !rockerIn && !rockerUp && !rockerDown)
    sleepWaitingForInput((1<<rockerUpPin) | (1<<rockerDownPin) | (1<<rockerInPin));
  readInputs();
  processInputs();
  processChanges();
  updateDisplay();
}

void sleepWaitingForInput(uint8_t pins) {
  // Don't sleep till after the debounce period - as otherwise a bounce will wake it up
  long currentTime = millis();
  if (currentTime < debounceTime && currentTime >= lastWakeUp) // time might wrap
  {
    return;
  }
  uint8_t temp = TIMSK;
  TIMSK = 0;                         // Disable timer interrupt(s)
  GIMSK = 1<<PCIE;                   // Enable pin-change interrupt
  PCMSK = pins;
  ADCSRA &= ~(1<<ADEN);              // Disable ADC to save power
  sleep_enable();
  sleep_cpu();

  GIMSK = 0;                         // Turn off interrupt
  TIMSK = temp;                      // Re-enable timer interrupt(s)
  ADCSRA |= 1<<ADEN;                 // Re-enable ADC
  lastWakeUp = millis();
  debounceTime = lastWakeUp;
}

void readInputs() {
  bool oldRockerUp = rockerUp;
  bool oldRockerDown = rockerDown;
  bool oldRockerIn = rockerIn;
  rockerUp = digitalRead(rockerUpPin)==LOW;
  rockerDown = digitalRead(rockerDownPin)==LOW;
  rockerIn = digitalRead(rockerInPin)==LOW;
  rockerInChanged = false;
  if ((oldRockerUp != rockerUp) || (oldRockerDown != rockerDown) || (oldRockerIn != rockerIn))
  {
    long currentTime = millis();
    if (currentTime < debounceTime && currentTime >= lastWakeUp) // time might wrap
    {
      return;
    }
    debounceTime = currentTime + debounceDelay;
  }
  if (oldRockerIn != rockerIn)
  {
    rockerInChanged = true;
  }
}

void processInputs() {
  if (switchingSettingLine) {
    processInputsSwitchingSettingLine();
  } else {
    processInputsForSetting(v[CURRENT_SETTING_LINE]);
  }
  if (rockerInChanged && rockerIn) {
    switchingSettingLine = !switchingSettingLine;
    setting_changed = CURRENT_SETTING_LINE;
  }
}

void processInputsSwitchingSettingLine() {
  processInputsForSetting(CURRENT_SETTING_LINE);
  if (setting_changed == CURRENT_SETTING_LINE) {
    uint8_t oldSettingsPage = settingsPage;
    settingsPage = (v[CURRENT_SETTING_LINE] - 1) >> 2;
    changedSettingsPage = oldSettingsPage != settingsPage;
  }
}

void processInputsForSetting(uint8_t m) {
  long ms = millis();
  if ((ms > timeChangedInput) && ((ms - timeChangedInput) < repeatInputInterval)) return;
  uint8_t value = v[m];
  uint8_t oldValue = value;
  bool inc = rockerUp;
  bool dec = rockerDown;
  if (m == CURRENT_SETTING_LINE) {
    inc = rockerDown;
    dec = rockerUp;
  }
  if (inc && (value < pgm_read_byte(&(s[m].max)))) value++;
  if (dec && (value > pgm_read_byte(&(s[m].min)))) value--;
  if (oldValue != value) {
    setting_changed = m;
    v[m] = value;
  }
}

bool CheckValueChanged(uint8_t m) {
  if (setting_changed == m) {
    setting_changed = settingLines + 1;
    r[m]=v[DOUBLE_BUFFER_VALUE] + 1;
    timeChangedInput=millis();
    return true;
  }
  return false;
}

void processChanges() {
  for (uint8_t m = 0; m <= settingLines; m++)
    if (CheckValueChanged(m)) ((ActionFunc)pgm_read_ptr(&(s[m].actionFn)))(v[m]);
}

void NoAction(__attribute__((unused)) uint8_t value) {}

void CurrentSettingLineChanged(__attribute__((unused)) uint8_t value) {
  if (changedSettingsPage) {
    redrawsSettingsPage=v[DOUBLE_BUFFER_VALUE] + 1;
  }
}

bool CheckRedraw(uint8_t m) {
  if (r[m] > 0) {
    r[m]--;
    performFrameSwitch = true;
    return true;
  }
  return false;
}

void updateDisplay() {
  performFrameSwitch = false;
  
  if (redrawsSettingsPage > 0) {
    OutputSettingsPage();
    redrawsSettingsPage--;
    performFrameSwitch = true;
  }

  for (uint8_t m = 0; m <= settingLines; m++)
    if (CheckRedraw(m)) ((OutputFunc)pgm_read_ptr(&(s[m].outputFn)))(m, v[m]);

  if (performFrameSwitch && v[DOUBLE_BUFFER_VALUE]) oled.switchFrame();
}

void OutputSettingsPage() {
  oled.clear();
  r[CURRENT_SETTING_LINE]=redrawsSettingsPage;
  uint8_t m = settingsPage * 4 + 1;
  for (uint8_t l = 0; (l < 4) && (m <= settingLines); l++, m++) {
    oled.setCursor(0, l);
    for (uint8_t k = 0; k < strlen_P((PGM_P)pgm_read_word(&(s[m].name))); k++)
    {
      char myChar =  pgm_read_byte((PGM_P)pgm_read_word(&(s[m].name)) + k);
      oled.print(myChar);
    }
    oled.print(':');
    r[m]=redrawsSettingsPage;
  }
}

void PositionCursor(uint8_t m) {
  oled.setCursor((strlen_P((PGM_P)pgm_read_word(&(s[m].name))) + 2) * 6, (m - 1) & 0x03);
}

void OutputNumOnly(uint8_t value) {
  oled.print(value);
  if (value<100) oled.print(' ');
  if (value<10) oled.print(' ');
}

void NoOutput(__attribute__((unused)) uint8_t m, __attribute__((unused)) uint8_t value) {}

void OutputCurrentSettingLine(__attribute__((unused)) uint8_t m, uint8_t value) {
  uint8_t currentLine = (value - 1) & 0x03;
  for (uint8_t l = 0; l < 4; l++) {
    oled.setCursor(120, l);
    if (l == currentLine) {
      if (switchingSettingLine)
        oled.print('-');
      else
        oled.print('<');
    } else {
      oled.print(' ');
    }
  }
}

void OutputNum(uint8_t m, uint8_t value) {
  PositionCursor(m);
  OutputNumOnly(value);
}

void OutputYN(uint8_t m, uint8_t value) {
  PositionCursor(m);
  if (value)
    oled.print('Y');
  else
    oled.print('N');
}

void OutputScrollInterval(uint8_t m, uint8_t value) {
  PositionCursor(m);
  OutputNumOnly(scrollIntervalFrames[value]);
}

void OutputScrollDirection(uint8_t m, uint8_t value) {
  PositionCursor(m);
  if (value == 0) oled.print('L');
  else if (value == 1) oled.print('-');
  else oled.print('R');
}

void OutputCurrentVoltage(uint8_t m, __attribute__((unused)) uint8_t value) {
  PositionCursor(m);
  unsigned int result = readADC();
  result = 1126400L / result; // Calculate Vcc (in mV); 1.1*1024*1000
  oled.print(result);
  oled.print(F(" mV"));
}

void setupADC() {
  ADMUX = 0b1100<<MUX0; // Vcc Ref, Measure bandgap voltage (1.1)
  ADCSRA = 1<<ADEN | 4<<ADPS0;            // Enable, 62.5kHz ADC clock (16x prescalar)
}

unsigned int readADC() {
  unsigned char low,high;
  ADCSRA = ADCSRA | 1<<ADSC;     // Start
  do {} while (ADCSRA & 1<<ADSC); // Wait while conversion in progress
  low = ADCL;
  high = ADCH;
  return (high<<8 | low);
}

/*
EEPROM CONTENTS

Location (HEX)
000:   4 bytes - magic EEPROM content identifier #1D544D50 (ID TMP)
004:   1 byte - next location to write data to.
005:   1 byte - current contrast - hold button down to cycle (todo)
006:   1 byte - used to adjust freq/turn off, 0 means 1 reading per hour (todo)
007:   1 byte - graph scale and type, 0 means graph -10 to 50 deg C (todo)
008: 120 bytes of readings in a circular buffer.

Since the readings are being displayed in a screen
with maximum resolution of 128x64
Each reading is converted to a value from -10 to 50
where 0 means -10 degrees C.
*/

/*
 * The Magic Number
 * 
 * If this does not match the first four bytes of the EEPROM,
 * then the EEPROM does not contain battery monitor information.
 * The EEPROM will be cleared so that the graph of historical data
 * is not showing garbage.
 */
#define TEMPERATURE_MAGIC 0x1D544D50
#define TEMPERATURE_MAGIC0 0x1D
#define TEMPERATURE_MAGIC1 0x54
#define TEMPERATURE_MAGIC2 0x4D
#define TEMPERATURE_MAGIC3 0x50

#define TEMPERATURE_MAGIC_ADDRESS 0
#define TEMPERATURE_CURRENT_ADDRESS 4
#define TEMPERATURE_CONTRAST_ADDRESS 5
#define TEMPERATURE_FREQUENCY_ADDRESS 6
#define TEMPERATURE_GRAPH_TYPE_ADDRESS 7
#define TEMPERATURE_READINGS_ADDRESS 8

#define TEMPERATURE_DEFAULT_CONTRAST 0x10
#define TEMPERATURE_DEFAULT_FREQUENCY 0x00
#define TEMPERATURE_DEFAULT_GRAPH 0x00

const byte header [] PROGMEM = {
  TEMPERATURE_MAGIC0, TEMPERATURE_MAGIC1, TEMPERATURE_MAGIC2, TEMPERATURE_MAGIC3, 
  TEMPERATURE_READINGS_ADDRESS, // current reading
  TEMPERATURE_DEFAULT_CONTRAST,  // contrast
  TEMPERATURE_DEFAULT_FREQUENCY,  // reading frequency
  TEMPERATURE_DEFAULT_GRAPH   // graph scale and type
};

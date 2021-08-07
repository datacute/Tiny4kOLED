/*
EEPROM CONTENTS

Location (HEX)
000:   4 bytes - magic EEPROM content identifier #1D424154 (ID BAT)
004:   1 byte - next location to write data to.
005:   1 byte - current contrast - hold button down to cycle (todo)
006:   1 byte - used to adjust freq/turn off, 0 means 1 reading per hour (todo)
007:   1 byte - graph scale and type, 0 means graph 0V to 6V (todo)
008: 120 bytes of readings in a circular buffer.

Since the readings are being displayed in a screen
with maximum resolution of 128x64
and maximum voltage is 6V
Each reading is converted to a value from 0 to 60
where 60 means 6 volts.
*/

/*
 * The Magic Number
 * 
 * If this does not match the first four bytes of the EEPROM,
 * then the EEPROM does not contain battery monitor information.
 * The EEPROM will be cleared so that the graph of historical data
 * is not showing garbage.
 */
#define BATTERY_MAGIC 0x1D424154
#define BATTERY_MAGIC0 0x1D
#define BATTERY_MAGIC1 0x42
#define BATTERY_MAGIC2 0x41
#define BATTERY_MAGIC3 0x54

#define BATTERY_MAGIC_ADDRESS 0
#define BATTERY_CURRENT_ADDRESS 4
#define BATTERY_CONTRAST_ADDRESS 5
#define BATTERY_FREQUENCY_ADDRESS 6
#define BATTERY_GRAPH_TYPE_ADDRESS 7
#define BATTERY_READINGS_ADDRESS 8

#define BATTERY_DEFAULT_CONTRAST 0x10
#define BATTERY_DEFAULT_FREQUENCY 0x00
#define BATTERY_DEFAULT_GRAPH 0x00

const byte header [] PROGMEM = {
  BATTERY_MAGIC0, BATTERY_MAGIC1, BATTERY_MAGIC2, BATTERY_MAGIC3, 
  BATTERY_READINGS_ADDRESS, // current reading
  BATTERY_DEFAULT_CONTRAST,  // contrast
  BATTERY_DEFAULT_FREQUENCY,  // reading frequency
  BATTERY_DEFAULT_GRAPH   // graph scale and type
};

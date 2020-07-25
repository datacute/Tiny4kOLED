/*

28 columns x 31 rows
at 1 bit per square - requires 28 x 4 bytes = 112 bytes
Could store a 112 byte background (wall/space) in flash (though the map is mirrored so only need to store one side of it)

Pills could be stored as bits of the clear path to fill - takes 39 bytes but makes everything a lot more complicated
Need to store current state as well as initial state.
Maybe store the lines of pills? 7 horizontal x 26, 10 vertical x 22 (less efficient to put into bytes: 7 x 4 + 10 x 3 = 58)
                             or 7 horizontal x 16, 10 vertical x 29 (more efficient          in bytes: 7 x 2 + 10 x 4 = 54)

Initial state is probably more efficent as an algorithm:
    If wall then not a pill
    Else if first 9 or last 11 rows then a pill
    Else if 7th or 22nd column then a pill
    Else not a pill

The original uses x positions 0x3B to 0x20
and y positions $21 to $3F

############################  0
#............##............#  1
#.####.#####.##.#####.####.#  2
#O####.#####.##.#####.####O#  3
#.####.#####.##.#####.####.#  4
#..........................#  5
#.####.##.########.##.####.#  6
#.####.##.########.##.####.#  7
#......##....##....##......#  8
######.##### ## #####.######  9
     #.##### ## #####.#      10
     #.##    []    ##.#      11
     #.## ###  ### ##.#      12
######.## #      # ##.###### 13
      .   #[][][]#   .       14
######.## #      # ##.###### 15
     #.## ######## ##.#      16
     #.##  READY!  ##.#      17
     #.## ######## ##.#      18
######.## ######## ##.###### 19
#............##............# 20
#.####.#####.##.#####.####.# 21
#.####.#####.##.#####.####.# 22
#O..##.......().......##..O# 23
###.##.##.########.##.##.### 24
###.##.##.########.##.##.### 25
#......##....##....##......# 26
#.##########.##.##########.# 27
#.##########.##.##########.# 28
#..........................# 29
############################ 30
  ()()

The map is mirrored, so we only need to store one side of it:
*/

const uint8_t mirroredMap [] PROGMEM = {
    0xFF,0x01,0xDD,0xDD,0xDD,0xDD,0x01,0xDD,0xDD,0x1D,0xDD,0xDD,0xC1,0xDF,
    0xA3,0xA2,0xA2,0xA2,0xA2,0xBE,0x00,0xBF,0xBF,0x06,0xF6,0x16,0x10,0x07,
    0xF8,0x08,0x68,0x68,0xE8,0xEF,0x00,0x6F,0x6F,0x60,0x6D,0x6D,0x0D,0x7D,
    0x7F,0x43,0x5B,0x58,0x5B,0x5B,0x58,0x5F,0x5F,0x58,0x5B,0x5B,0x43,0x5F
};

const uint8_t horizontalPelletRows [] = {
  1,5,8,20,23,26,29
};
const uint8_t horizontalPelletsInit [] PROGMEM = {
  0xB4,0x0D,0xDB,0x02,
  0xB4,0x6D,0xDB,0x02,
  0x34,0x0C,0xC3,0x02,
  0xB4,0x0D,0xDB,0x02,
  0x84,0x0D,0x1B,0x02,
  0x34,0x0C,0xC3,0x02,
  0xB4,0x6D,0xDB,0x02,
};

const uint8_t verticalPelletRows [] = {
  1,3,6,9,12,15,18,21,24,26
};
const uint8_t verticalPelletsInit [] PROGMEM = {
  0xFE,0x01,0xF0,0x3C,
  0x22,0x01,0x90,0x27,
  0xFE,0xFF,0xFF,0x27,
  0xE2,0x01,0x90,0x27,
  0x3E,0x01,0xF0,0x3C,
  0x3E,0x01,0xF0,0x3C,
  0xE2,0x01,0x90,0x27,
  0xFE,0xFF,0xFF,0x27,
  0x22,0x01,0x90,0x27,
  0xFE,0x01,0xF0,0x3C,
};

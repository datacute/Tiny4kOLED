/*
 * TinyNunchuk & Tiny4kOLED example of a scrolling PacMan maze
 */

/*
 * My next steps if there's ram left, are to add 
 * - ghost movement
 * - super pellets switching ghost mode
 * - collisions
 * - display of score (currently holding C button pauses the game and shows the score)
 */

// Choose your I2C implementation before including Tiny4kOLED.h or TinyNunchuk.h
// The default is selected is Wire.h

// To use the Wire library:
//#include <Wire.h>

// To use the Adafruit's TinyWireM library:
//#include <TinyWireM.h>

// To use the TinyI2C library from https://github.com/technoblogy/tiny-i2c
//#include <TinyI2CMaster.h>

#include <TinyNunchuk.h>

// The blue OLED screen requires a long initialization on power on.
// The code to wait for it to be ready uses 20 bytes of program storage space
// If you are using a white OLED, this can be reclaimed by uncommenting
// the following line (before including Tiny4kOLED.h):
//#define TINY4KOLED_QUICK_BEGIN

#include <Tiny4kOLED.h>

#include "PacManMap.h"
#include "Graphics.h"

bool gameStarted;
uint32_t score;
uint8_t pacmanDirection;
uint8_t pacmanLocationX;
uint8_t pacmanLocationY;
uint8_t pacmanStep;
bool wallAbove;
bool wallBelow;
bool wallLeft;
bool wallRight;
int8_t blinkyLocationX; // Red Ghost
int8_t blinkyLocationY;
int8_t pinkyLocationX; // Pink Ghost
int8_t pinkyLocationY;
int8_t inkyLocationX; // Blue Ghost
int8_t inkyLocationY;
int8_t clydeLocationX; // Orange Ghost
int8_t clydeLocationY;

const uint8_t movementX [] = { 0xFF, 0x01, 0x00, 0x00 };
const uint8_t movementY [] = { 0x00, 0x00, 0xFF, 0x01 };

// In order to keep pacman in the middle of the screen,
// the 0 values for topMazeLine and leftMazeColumn
// need to be to outside the map
#define screenOffsetXTiles 8
#define screenOffsetYTiles 4

uint8_t topMazeLine;
uint8_t leftMazeColumn;

uint8_t tileBits [136] = {};

uint8_t horizontalPellets[7*4] = {};
uint8_t verticalPellets[10*4] = {};

void setup() {
  nunchuk.begin();
  oled.begin(128, 64, sizeof(tiny4koled_init_128x64br), tiny4koled_init_128x64br);
  //oled.setComOutputDirection(0);
  restartGame();
}

void loop() {
  nunchuk.update();

  if (!gameStarted) {
    if (topMazeLine != (19 + screenOffsetYTiles) * 8) {
      scrollUp();
    } else {
      if (nunchuk.z) {
        gameStarted = true;
      }
    }
    return;
  }

  while (nunchuk.c) {
    // pause to show scores and lives, and allow restarting
    showScore();
    if (nunchuk.z) {
      restartGame();
      // wait for Z to be released
      while (nunchuk.z) {
        nunchuk.update();
      }
      return;
    } else {
      // wait for C to be released
      while (nunchuk.c) {
        nunchuk.update();
      }
      oled.setDisplayStartLine(topMazeLine % 64);
      redrawMaze(128, 8, 0, 0);
    }
  }

  // Pacman can only change from moving left/right to up/down when step is zero
  // Only need to check for wall collisions when step is zero
  // Still need to change this as attempted diagonal movement should result in change of direction when possible
  bool movingLeft = nunchuk.x < 64;
  bool movingRight = nunchuk.x > 192;
  bool movingUp = nunchuk.y > 192;
  bool movingDown = nunchuk.y < 64;
  if (pacmanStep == 0) { // Diagonal movement changes axis
    if (((pacmanDirection == 0) || (pacmanDirection == 1))) {
      // Check up/down before left/right
      if (movingUp && !wallAbove) movePacmanDecreasingStep(2);
      else if (movingDown && !wallBelow) movePacmanIncreasingStep(3);
      else if ((movingLeft || ((pacmanDirection == 0) && !movingRight)) && !wallLeft) movePacmanDecreasingStep(0);
      else if ((movingRight || ((pacmanDirection == 1) && !movingLeft)) && !wallRight) movePacmanIncreasingStep(1);
    } else {
      // Check left/right before up/down
      if (movingLeft && !wallLeft) movePacmanDecreasingStep(0);
      else if (movingRight && !wallRight) movePacmanIncreasingStep(1);
      else if ((movingUp || ((pacmanDirection == 2) && !movingDown)) && !wallAbove) movePacmanDecreasingStep(2);
      else if ((movingDown || ((pacmanDirection == 3) && !movingUp))&& !wallBelow) movePacmanIncreasingStep(3);
    }
  } else { // Can't change axis
    switch (pacmanDirection) {
      case 0:
        if (movingRight) movePacmanIncreasingStep(1);
        else movePacmanDecreasingStep(0);
        break;
      case 1:
        if (movingLeft) movePacmanDecreasingStep(0);
        else movePacmanIncreasingStep(1);
        break;
      case 2:
        if (movingDown) movePacmanIncreasingStep(3);
        else movePacmanDecreasingStep(2);
        break;
      default: // 3
        if (movingUp) movePacmanDecreasingStep(2);
        else movePacmanIncreasingStep(3);
        break;
    }
  }
}

void restartGame() {
  for (uint8_t p = 0; p < 7*4; p++) {
    horizontalPellets[p] = pgm_read_byte(&horizontalPelletsInit[p]);
  }
  for (uint8_t p = 0; p < 10*4; p++) {
    verticalPellets[p] = pgm_read_byte(&verticalPelletsInit[p]);
  }
  gameStarted = false;
  score = 0L;
  pacmanDirection = 0;
  pacmanLocationX = 13;
  pacmanLocationY = 23;
  pacmanStep = 4;
  blinkyLocationX = 13 * 8 + 4; // Red Ghost
  blinkyLocationY = 11 * 8;
  pinkyLocationX = 13 * 8 + 4; // Pink Ghost
  pinkyLocationY = 14 * 8;
  inkyLocationX = 11 * 8 + 4; // Blue Ghost
  inkyLocationY = 14 * 8;
  clydeLocationX = 15 * 8 + 4; // Orange Ghost
  clydeLocationY = 14 * 8;
  topMazeLine = screenOffsetYTiles * 8;
  leftMazeColumn = screenOffsetXTiles * 8 + 6 * 8;
  checkSurroundings();
  oled.off();
  redrawMaze(128, 8, 0, 0);
  oled.setDisplayStartLine(topMazeLine % 64);
  // We're sacrificing a line to allow smooth scrolling
  oled.setDisplayOffset(0);
  oled.setMultiplexRatio(63);
  oled.on();
  delay(1000);
}

void showScore() {
  oled.clear();
  oled.setDisplayStartLine(0);
  oled.setCursor(0, 0);
  oled.startData();
  int32_t remainingScore = score;
  int32_t scoreSize = 1000000L;
  while (scoreSize > 0) {
    uint8_t digit = 0;
    while (remainingScore >= scoreSize) {
      digit++;
      remainingScore -= scoreSize;
    }
    sendDigit(digit);
    scoreSize /= 10;
  }
  oled.endData();
}

void sendDigit(uint8_t digit) {
  uint8_t digitGlyphsOffset = digit * 3;
  for (uint8_t j = 0; j < 3; j++) {
    oled.sendData(pgm_read_byte(&digitGlyphs[digitGlyphsOffset++]));
  }
  oled.sendData(0x00);
  oled.sendData(0x00);
}

void scrollUp() {
  // 1. Hide the bottom line
  // With reversed com output direction setting,
  // one less row from the bottom of the screen is shown
  //oled.setMultiplexRatio(63);
  // so also shift the offset
  //oled.setDisplayOffset(1);
  // 2. Change which line is the top line
  topMazeLine++;
  oled.setDisplayStartLine(topMazeLine % 64);
  // 3. Redraw the page that contains the new bottom line
  // If that's not the top page, then just redraw the bottom page
  if ((topMazeLine & 0x07) == 0) {
    redrawMaze(128,1,0,7);
  } else {
    redrawMaze(128,1,0,0);
  }
  // 4. Change the display to start from the top line
  //oled.setDisplayOffset(0);
  //oled.setMultiplexRatio(64);
}

void scrollDown() {
  if (topMazeLine == 0) return;

  // 1. Hide the top line
  // With reversed com output direction setting,
  // one less row from the bottom of the screen is shown
  oled.setMultiplexRatio(62);
  // 2. Change which line is the top line
  topMazeLine--;
  // 3. Redraw the page that contains the top line
  redrawMaze(128,1,0,0);
  // 4. Change the display to start from the top line
  oled.setDisplayStartLine(topMazeLine % 64);
  oled.setMultiplexRatio(63);
}

void scrollLeft() {
  leftMazeColumn++;
  oled.scrollContentLeft(0,7,0,127);
  delay(20);
  redrawMaze(1, 8, 127, 0);
}

void scrollRight() {
  if (leftMazeColumn == 0) return;
  leftMazeColumn--;
  oled.scrollContentRight(0,7,0,127);
  delay(20);
  redrawMaze(1, 8, 0, 0);
}

void drawPacman() {
  // Pacman starts in the middle, so redraw the middle
  switch (pacmanDirection) {
    case 0:
      redrawMaze(9,1,60,4);
      break;
    case 1:
      redrawMaze(9,1,59,4);
      break;
    case 2:
      redrawMaze(8,3,60,3);
      break;
    default: // 3
      redrawMaze(8,3,60,3);
      break;
  }
}

void movePacmanIncreasingStep(uint8_t newDirection) {
  if (pacmanStep == 7) {
    changePacmanLocation(newDirection);
    pacmanStep = 0;
    checkSurroundings();
  } else {
    pacmanStep++;
  }
  pacmanDirection = newDirection;
  if (newDirection == 1) {
    if (pacmanLocationX == 27) {
      pacmanLocationX = 0;
      leftMazeColumn = (screenOffsetXTiles - 8 + pacmanLocationX) * 8 + 4;
      redrawMaze(128, 8, 0, 0);
      return;
    }
    scrollLeft();
  } else {
    scrollUp();
  }
  drawPacman();
}

void movePacmanDecreasingStep(uint8_t newDirection) {
  if (pacmanStep == 0) {
    changePacmanLocation(newDirection);
    pacmanStep = 7;
  } else {
    pacmanStep--;
    if (pacmanStep == 0) checkSurroundings();
  }
  pacmanDirection = newDirection;
  if (newDirection == 0) {
    if ((pacmanLocationX == 0) && (pacmanStep == 0)) {
      pacmanLocationX = 26;
      leftMazeColumn = (screenOffsetXTiles - 8 + pacmanLocationX) * 8 + 4;
      redrawMaze(128, 8, 0, 0);
      return;
    }
    scrollRight();
  } else {
    scrollDown();
  }
  drawPacman();
}

void changePacmanLocation(uint8_t newDirection) {
  pacmanLocationX += movementX[newDirection];
  pacmanLocationY += movementY[newDirection];
}

void redrawMaze(uint8_t w, uint8_t h, uint8_t sx, uint8_t page) {
  uint8_t topMazeTile = topMazeLine >> 3;
  uint8_t topBlankBits = topMazeLine & 0x07;
  uint8_t bottomMazeTile = ((topMazeLine + 7) >> 3) + 7; // (topMazeLine + 63) >> 3
  uint8_t bottomBlankBits = 7 - ((topMazeLine + 7) & 0x07);
  uint8_t xOffset = (leftMazeColumn + sx) & 0x07;
  uint8_t limit = w + xOffset;
  for (int8_t tileY = topMazeTile + page; tileY < topMazeTile + page + h; tileY++) {
    for (uint8_t j = 0; j < 136; j++) {
      tileBits[j] = 0;
    }
    uint8_t p = tileY & 0x07;
    uint8_t mask = 0xFF;
    if ((topBlankBits != 0) && (tileY == topMazeTile)) {
      mask = 0xFF << topBlankBits;
    }
    // We might be blanking the last row prior to shifting displayOffset up
    if ((bottomBlankBits != 0) && (tileY == bottomMazeTile)) {
      mask = 0xFF >> bottomBlankBits;
    }
    drawPage(mask, tileY, w, sx);
    if ((topBlankBits != 0) && (bottomBlankBits != 0) && (tileY == topMazeTile)) {
      uint8_t bottomMask = 0xFF >> bottomBlankBits;
      drawPage(bottomMask, bottomMazeTile, w, sx);
    }
    oled.setCursor(sx, p);
    oled.startData();
    for (uint8_t j = xOffset; j < limit; j++) {
      oled.sendData(tileBits[j]);
    }
    oled.endData();
  }
}

void drawPage(uint8_t mask, uint8_t tileY, uint8_t w, uint8_t sx) {
  uint8_t leftMazeTile = (leftMazeColumn + sx) >> 3;
  uint8_t rightMazeTile = (leftMazeColumn + sx + w - 1) >> 3;
  uint8_t c = 0;
  for (int8_t tileX = leftMazeTile; tileX <= rightMazeTile; tileX++) {
    if (isWall(tileX, tileY)) {
      bool wallRight = isWall(tileX + 1, tileY);
      bool wallAbove = isWall(tileX, tileY - 1);
      bool wallLeft = isWall(tileX - 1, tileY);
      bool wallBelow = isWall(tileX, tileY + 1);
      for (uint8_t j = 0; j < 8; j++) {
        uint8_t bits = 0;
        if (wallRight) bits |= pgm_read_byte(&sprites[spriteWallRight * 8 + j]);
        if (wallAbove) bits |= pgm_read_byte(&sprites[spriteWallAbove * 8 + j]);
        if (wallLeft) bits |= pgm_read_byte(&sprites[spriteWallLeft * 8 + j]);
        if (wallBelow) bits |= pgm_read_byte(&sprites[spriteWallBelow * 8 + j]);
        tileBits[c++] |= bits & mask;
      }
    } else {
      uint8_t spriteDetail = getSprite(tileX, tileY);
      uint8_t sprite = spriteDetail & spriteMask;
      uint8_t spriteOffset = sprite * 8;
      uint8_t spriteLeavingShift = 0;
      uint8_t spriteEnteringShift = 0;
      if (spriteDetail & spriteShifted) {
        if (sprite <= spritePacmanRight2) {
          c += pacmanStep;
        } else {
          spriteLeavingShift = pacmanStep;
        }
      }
      if (spriteDetail & spriteEntering) {
        spriteEnteringShift = 8 - pacmanStep;
      }
      if (spriteDetail & spriteReversed) {
        spriteOffset += 7;
        for (uint8_t j = 0; j < 8; j++) {
          tileBits[c++] |= ((pgm_read_byte(&sprites[spriteOffset--]) << spriteLeavingShift) >> spriteEnteringShift) & mask;
        }
      } else {
        for (uint8_t j = 0; j < 8; j++) {
          tileBits[c++] |= ((pgm_read_byte(&sprites[spriteOffset++]) << spriteLeavingShift) >> spriteEnteringShift) & mask;
        }
      }
      if (spriteDetail & spriteShifted) {
        if (sprite <= spritePacmanRight2) {
          c += 8 - pacmanStep;
          tileX++;
        }
      }
    }
  }
}

bool isWall(int8_t x, int8_t y) {
  if (x < 0 + screenOffsetXTiles) return false;
  if (x > 27 + screenOffsetXTiles) return false;
  if (y < 0 + screenOffsetYTiles) return false;
  if (y > 30 + screenOffsetYTiles) return false;
  x-= screenOffsetXTiles;
  y-= screenOffsetYTiles;

  uint8_t mapOffset = (y >> 3) * 14;
  if (x < 14) {
    mapOffset += x;
  } else {
    mapOffset += (27 - x);
  }
  uint8_t bits = pgm_read_byte(&mirroredMap[mapOffset]);
  return (bits >> (y & 0x7)) & 0x01;
}

void checkSurroundings() {
  wallAbove = isWall(pacmanLocationX + screenOffsetXTiles, pacmanLocationY - 1 + screenOffsetYTiles);
  wallBelow = isWall(pacmanLocationX + screenOffsetXTiles, pacmanLocationY + 1 + screenOffsetYTiles);
  wallLeft = isWall(pacmanLocationX - 1 + screenOffsetXTiles, pacmanLocationY + screenOffsetYTiles);
  wallRight = isWall(pacmanLocationX + 1 + screenOffsetXTiles, pacmanLocationY + screenOffsetYTiles);
  eatPellet();
}

void eatPellet() {
  for (uint8_t vp = 0; vp < 10; vp++) {
    if (verticalPelletRows[vp] == pacmanLocationX) {
      uint8_t pelletOffset = vp*4 + (pacmanLocationY >> 3);
      uint8_t current = verticalPellets[pelletOffset];
      uint8_t palletBit = pacmanLocationY & 0x07;
      if (bitRead(current, palletBit)) {
        bitClear(current, palletBit);
        verticalPellets[pelletOffset] = current;
        score += 10;
        if (((pacmanLocationX == 1) || (pacmanLocationX == 26)) && ((pacmanLocationY == 3) || (pacmanLocationY == 23))) score += 40;
      }
      return;
    }
  }
  for (uint8_t hp = 0; hp < 7; hp++) {
    if (horizontalPelletRows[hp] == pacmanLocationY) {
      uint8_t pelletOffset = hp*4 + (pacmanLocationX >> 3);
      uint8_t current = horizontalPellets[pelletOffset];
      uint8_t palletBit = pacmanLocationX & 0x07;
      if (bitRead(current, palletBit)) {
        bitClear(current, palletBit);
        horizontalPellets[pelletOffset] = current;
        score += 10;
        if (((pacmanLocationX == 1) || (pacmanLocationX == 26)) && ((pacmanLocationY == 3) || (pacmanLocationY == 23))) score += 40;
      }
      return;
    }
  }
}

uint8_t getSprite(int8_t x, int8_t y) {
  if ((x < 0 + screenOffsetXTiles) || (x > 27 + screenOffsetXTiles) || (y < 0 + screenOffsetYTiles) || (y > 30 + screenOffsetYTiles)) return spriteBlank;
  x-= screenOffsetXTiles;
  y-= screenOffsetYTiles;
  if (x == pacmanLocationX) {
    if (y == pacmanLocationY) {
      // Need to consider animation
      uint8_t pacmanSprite = spritePacmanRight1;
      if (pacmanDirection == 0) {
        pacmanSprite |= spriteReversed;
      } else if (pacmanDirection == 2) {
        pacmanSprite = spritePacmanUp1;
      } else if (pacmanDirection == 3) {
        pacmanSprite = spritePacmanDown1;
      }
      if (pacmanStep & 1) pacmanSprite++;
      if (pacmanStep) pacmanSprite |= spriteShifted;
      return pacmanSprite;
    } else {
      if ((pacmanStep != 0) && (y == pacmanLocationY + 1)) {
        uint8_t pacmanSprite = spritePacmanUp1;
        if (pacmanDirection == 3) {
          pacmanSprite = spritePacmanDown1;
        }
        if (pacmanStep & 1) pacmanSprite++;
        pacmanSprite |= spriteEntering;
        return pacmanSprite;
      }
    }
  }
  if (x == (blinkyLocationX >> 3) && y == (blinkyLocationY >> 3)) {
    // Need to consider animation
    return spriteGhostRightHunting | spriteReversed;
  }
  if (x == (pinkyLocationX >> 3) && y == (pinkyLocationY >> 3)) {
    // Need to consider direction & animation
    return spriteGhostRightHunting | spriteReversed;
  }
  if (x == (inkyLocationX >> 3) && y == (inkyLocationY >> 3)) {
    // Need to consider animation
    return spriteGhostRightHunting | spriteReversed;
  }
  if (x == (clydeLocationX >> 3) && y == (clydeLocationY >> 3)) {
    // Need to consider animation
    return spriteGhostRightHunting | spriteReversed;
  }

  // READY!
  if (!gameStarted && y == 17 && x >= 11 && x <= 16) return (spriteR + x - 11);

  for (uint8_t vp = 0; vp < 10; vp++) {
    if (verticalPelletRows[vp]==x) {
      if ((verticalPellets[vp*4 + (y >> 3)] >> (y & 0x07)) & 0x01) {
        if (((x == 1) || (x == 26)) && ((y == 3) || (y == 23))) return spriteSuperPellet;
        return spritePellet;
      }
      return spriteBlank;
    }
  }
  for (uint8_t hp = 0; hp < 7; hp++) {
    if (horizontalPelletRows[hp]==y) {
      if ((horizontalPellets[hp*4 + (x >> 3)] >> (x & 0x07)) & 0x01) {
        return spritePellet;
      }
      return spriteBlank;
    }
  }
  return spriteBlank;
}

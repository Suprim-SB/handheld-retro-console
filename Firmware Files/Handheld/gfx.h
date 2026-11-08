#pragma once
#include <string.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "config.h"


Adafruit_SH1106G display(SCREEN_W, SCREEN_H, &Wire, -1, I2C_HZ, I2C_HZ);

static inline bool pxGet(int x, int y) {
  if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) return false;
  return display.getBuffer()[x + (y >> 3) * SCREEN_W] & (1 << (y & 7));
}

static inline void pxSet(int x, int y, bool on) {
  if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) return;
  uint8_t* b = display.getBuffer();
  if (on) b[x + (y >> 3) * SCREEN_W] |=  (1 << (y & 7));
  else    b[x + (y >> 3) * SCREEN_W] &= ~(1 << (y & 7));
}

static inline void pxXor(int x, int y) {
  if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) return;
  display.getBuffer()[x + (y >> 3) * SCREEN_W] ^= (1 << (y & 7));
}


static void invertRect(int x, int y, int w, int h) {
  for (int yy = y; yy < y + h; yy++)
    for (int xx = x; xx < x + w; xx++)
      pxXor(xx, yy);
}


static void shiftRows(int y0, int h, int dx) {
  if (dx == 0) return;
  bool tmp[SCREEN_W];
  for (int y = y0; y < y0 + h; y++) {
    if (y < 0 || y >= SCREEN_H) continue;
    for (int x = 0; x < SCREEN_W; x++) tmp[x] = pxGet(x, y);
    for (int x = 0; x < SCREEN_W; x++) {
      int sx = (x - dx) % SCREEN_W;
      if (sx < 0) sx += SCREEN_W;
      pxSet(x, y, tmp[sx]);
    }
  }
}

static inline uint8_t hash8(uint32_t x, uint32_t y) {
  uint32_t h = x * 374761393u + y * 668265263u;
  h = (h ^ (h >> 13)) * 1274126177u;
  return (uint8_t)((h ^ (h >> 16)) & 0xFF);
}

static inline int textWidth(const char* s, int size) { return (int)strlen(s) * 6 * size; }

static void printCentered(const char* s, int y, int size) {
  display.setTextSize(size);
  display.setCursor((SCREEN_W - textWidth(s, size)) / 2, y);
  display.print(s);
}

static void drawSparkle(int cx, int cy, int r) {
  display.drawPixel(cx, cy, SH110X_WHITE);
  for (int i = 1; i <= r; i++) {
    display.drawPixel(cx - i, cy, SH110X_WHITE);
    display.drawPixel(cx + i, cy, SH110X_WHITE);
    display.drawPixel(cx, cy - i, SH110X_WHITE);
    display.drawPixel(cx, cy + i, SH110X_WHITE);
  }
  if (r >= 3) {
    display.drawPixel(cx - 1, cy - 1, SH110X_WHITE);
    display.drawPixel(cx + 1, cy - 1, SH110X_WHITE);
    display.drawPixel(cx - 1, cy + 1, SH110X_WHITE);
    display.drawPixel(cx + 1, cy + 1, SH110X_WHITE);
  }
}

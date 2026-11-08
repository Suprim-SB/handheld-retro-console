#pragma once
#include "gfx.h"
#include "input.h"
#include "sound.h"

// ============================================================
//  hwtest.h  -  hardware test screen (the Part 1 test)
//
//  HOW TO OPEN IT: hold the B button while switching the console on.
//  It shows every button live, beeps a different note per button and
//  shows the frame rate. Use it to check the wiring of a new PCB.
// ============================================================

static const uint16_t HW_BTN_TONE[BTN_COUNT] = { 880, 660, 523, 784, 1047, 440 };
static const char*    hwLast   = "-";
static uint16_t       hwCount  = 0;
static uint16_t       hwFps    = 0;
static uint16_t       hwFrames = 0;
static uint32_t       hwStamp  = 0;

static void hwSquare(int x, int y, bool on) {
  if (on) display.fillRect(x, y, 12, 12, SH110X_WHITE);
  else    display.drawRect(x, y, 12, 12, SH110X_WHITE);
}

static void hwRound(int cx, int cy, char label, bool on) {
  if (on) display.fillCircle(cx, cy, 8, SH110X_WHITE);
  else    display.drawCircle(cx, cy, 8, SH110X_WHITE);
  display.setTextColor(on ? SH110X_BLACK : SH110X_WHITE);
  display.setCursor(cx - 3, cy - 4);       // a 6x8 character, centred
  display.print(label);
  display.setTextColor(SH110X_WHITE);
}

static void hwtestFrame() {
  // react to presses
  for (uint8_t b = 0; b < BTN_COUNT; b++) {
    if (input.pressed((Button)b)) {
      hwCount++;
      hwLast = BTN_NAMES[b];
      sound.beep(HW_BTN_TONE[b], 60);
      LOGF("%s pressed (total %u)\n", BTN_NAMES[b], hwCount);
    }
  }

  // frames per second
  hwFrames++;
  if (millis() - hwStamp >= 1000) {
    hwFps = hwFrames;
    hwFrames = 0;
    hwStamp = millis();
  }

  // draw
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.print("HW TEST");
  display.setCursor(84, 0);
  display.print(hwFps);
  display.print(" fps");
  display.drawFastHLine(0, 10, SCREEN_W, SH110X_WHITE);

  hwSquare(18, 16, input.held(BTN_UP));
  hwSquare(6,  28, input.held(BTN_LEFT));
  hwSquare(30, 28, input.held(BTN_RIGHT));
  hwSquare(18, 40, input.held(BTN_DOWN));

  hwRound(108, 30, 'A', input.held(BTN_A));
  hwRound(88,  40, 'B', input.held(BTN_B));

  display.setCursor(0, 56);
  display.print("last:");
  display.print(hwLast);
  display.print(" n=");
  display.print(hwCount);

  display.display();
}

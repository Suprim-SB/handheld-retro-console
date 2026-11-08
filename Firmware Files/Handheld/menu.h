#pragma once
#include <math.h>
#include <stdio.h>
#include "gfx.h"
#include "input.h"
#include "sound.h"
#include "icons.h"

// ============================================================
//  menu.h  -  the pizza-slice game wheel
//
//  Controls
//    wheel:      RIGHT = spin to the next game (clockwise)
//                LEFT  = spin back to the previous game
//                A     = open that game's full-screen view
//    full view:  A = start the game      B = back to the wheel
//
//  The wheel has one slice per game. The slice on the right side
//  (under the pointer) is the selected one; it is drawn inverted
//  (white) so it stands out.
// ============================================================

constexpr int   WHEEL_CX  = 34;       // wheel centre
constexpr int   WHEEL_CY  = 32;
constexpr int   WHEEL_R   = 30;       // wheel radius
constexpr int   ICON_R    = 19;       // how far from the centre the icons sit
constexpr int   ICON_SIZE = 15;
constexpr float SLICE_DEG = 360.0f / GAME_COUNT;
constexpr float DEG2RAD   = 0.0174532925f;

enum MenuState : uint8_t { M_WHEEL, M_ZOOM_IN, M_VIEW, M_ZOOM_OUT };

class Menu {
public:
  // intro = true: the wheel spins in. false: it just appears (returning from a game).
  void begin(bool intro) {
    state_ = M_WHEEL;
    irisT_ = 0.0f;
    spinIn_ = intro;
    rot_ = intro ? target() + 288.0f : target();
    last_ = millis();
  }

  int selected() const {
    return ((steps_ % GAME_COUNT) + GAME_COUNT) % GAME_COUNT;
  }

  // Runs one frame: reads buttons, animates, draws.
  // Returns the game number (0..4) when the player starts a game, else -1.
  int frame() {
    const uint32_t now = millis();
    float dt = (now - last_) / 1000.0f;
    last_ = now;
    if (dt > 0.05f) dt = 0.05f;

    int launch = -1;

    switch (state_) {
      case M_WHEEL:
        if (input.repeat(BTN_RIGHT)) { steps_++; sound.play(SFX_TICK, 1); }
        if (input.repeat(BTN_LEFT))  { steps_--; sound.play(SFX_TICK, 1); }
        if (input.pressed(BTN_A))    { state_ = M_ZOOM_IN; irisT_ = 0.0f; sound.play(SFX_SELECT, ARRAY_LEN(SFX_SELECT)); }
        break;

      case M_ZOOM_IN:
        irisT_ += dt / 0.28f;
        if (irisT_ >= 1.0f) { irisT_ = 1.0f; state_ = M_VIEW; }
        break;

      case M_VIEW:
        if (input.pressed(BTN_A)) {
          launch = selected();
          sound.play(SFX_LAUNCH, ARRAY_LEN(SFX_LAUNCH));
        } else if (input.pressed(BTN_B)) {
          state_ = M_ZOOM_OUT;
          irisT_ = 1.0f;
          sound.play(SFX_TICK, 1);
        }
        break;

      case M_ZOOM_OUT:
        irisT_ -= dt / 0.25f;
        if (irisT_ <= 0.0f) { irisT_ = 0.0f; state_ = M_WHEEL; }
        break;
    }

    // Smoothly turn the wheel toward where it should be.
    const float diff = target() - rot_;
    rot_ += diff * (1.0f - expf(-dt * (spinIn_ ? 6.0f : 16.0f)));
    if (fabsf(target() - rot_) < 0.3f) { rot_ = target(); spinIn_ = false; }

    draw();
    return launch;
  }

private:
  MenuState state_  = M_WHEEL;
  int       steps_  = 0;          // counts every click; selected game = steps_ mod 5
  float     rot_    = 0.0f;       // current wheel angle in degrees
  float     irisT_  = 0.0f;       // 0..1 progress of the zoom transition
  bool      spinIn_ = false;
  uint32_t  last_   = 0;

  float target() const { return -steps_ * SLICE_DEG; }

  // ---------- drawing ----------

  void drawBold(const char* s, int x, int y) {      // text drawn twice, 1 px apart
    display.setCursor(x, y);     display.print(s);
    display.setCursor(x + 1, y); display.print(s);
  }

  // Flip the fixed wedge on the right of the wheel (the "selected" window).
  void invertSelectedWedge() {
    for (int dx = 1; dx <= WHEEL_R; dx++) {
      const int maxdy = (dx * 93) >> 7;               // tan(36 deg) = 0.7265 ~ 93/128
      for (int dy = -maxdy; dy <= maxdy; dy++) {
        if (dx * dx + dy * dy <= WHEEL_R * WHEEL_R) pxXor(WHEEL_CX + dx, WHEEL_CY + dy);
      }
    }
  }

  void drawScene() {
    display.setTextColor(SH110X_WHITE);

    // Wheel outline and centre hub.
    display.drawCircle(WHEEL_CX, WHEEL_CY, WHEEL_R, SH110X_WHITE);
    display.fillCircle(WHEEL_CX, WHEEL_CY, 3, SH110X_WHITE);

    // Slice dividers: one line between each pair of neighbouring slices.
    for (int i = 0; i < GAME_COUNT; i++) {
      const float b = (rot_ + i * SLICE_DEG + SLICE_DEG / 2.0f) * DEG2RAD;
      display.drawLine(WHEEL_CX + (int)lroundf(5 * cosf(b)),       WHEEL_CY - (int)lroundf(5 * sinf(b)),
                       WHEEL_CX + (int)lroundf(WHEEL_R * cosf(b)), WHEEL_CY - (int)lroundf(WHEEL_R * sinf(b)),
                       SH110X_WHITE);
    }

    // One icon in the middle of each slice.
    for (int i = 0; i < GAME_COUNT; i++) {
      const float a = (rot_ + i * SLICE_DEG) * DEG2RAD;
      drawIcon(i, WHEEL_CX + (int)lroundf(ICON_R * cosf(a)),
                  WHEEL_CY - (int)lroundf(ICON_R * sinf(a)),
                  ICON_SIZE, SH110X_WHITE, SH110X_BLACK);
    }

    invertSelectedWedge();

    // Pointer on the right of the wheel.
    display.fillTriangle(WHEEL_CX + WHEEL_R + 3, WHEEL_CY,
                         WHEEL_CX + WHEEL_R + 9, WHEEL_CY - 4,
                         WHEEL_CX + WHEEL_R + 9, WHEEL_CY + 4, SH110X_WHITE);

    // Right-hand panel: number, name, hint.
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    char idx[16];
    snprintf(idx, sizeof(idx), "%d/%d", selected() + 1, GAME_COUNT);
    display.setCursor(80, 4);
    display.print(idx);

    const char* name = GAME_NAMES[selected()];
    const char* sp = strchr(name, ' ');
    if (sp) {                                        // two-word names go on two lines
      char first[12];
      int n = (int)(sp - name);
      if (n > 11) n = 11;
      memcpy(first, name, n);
      first[n] = 0;
      drawBold(first, 78, 26);
      drawBold(sp + 1, 78, 36);
    } else {
      drawBold(name, 78, 31);
    }
    display.setCursor(78, 54);
    display.print("A:VIEW");
  }

  // The full-screen view: white drawing on a black background.
  void drawView() {
    display.drawRoundRect(0, 0, SCREEN_W, SCREEN_H, 5, SH110X_WHITE);   // thin frame
    const int g = selected();
    drawIcon(g, SCREEN_W / 2, 19, 30, SH110X_WHITE, SH110X_BLACK);

    const char* name = GAME_NAMES[g];
    const int size = (textWidth(name, 2) <= 124) ? 2 : 1;
    display.setTextColor(SH110X_WHITE);
    display.setTextSize(size);
    display.setCursor((SCREEN_W - textWidth(name, size)) / 2, (size == 2) ? 38 : 42);
    display.print(name);

    display.setTextSize(1);
    display.setCursor((SCREEN_W - textWidth("A:START  B:BACK", 1)) / 2, 56);
    display.print("A:START  B:BACK");
    display.setTextColor(SH110X_WHITE);
  }

  void draw() {
    display.clearDisplay();
    if (state_ == M_VIEW) {
      drawView();
    } else {
      drawScene();
      if (state_ == M_ZOOM_IN || state_ == M_ZOOM_OUT) {
        // A black circle with a white edge grows out of the selected slice and
        // wipes the wheel away, ending on the black full-screen view.
        const float e = irisT_ * irisT_ * (3.0f - 2.0f * irisT_);   // smoothstep
        const int r = (int)(e * 86.0f);
        if (r > 0) {
          display.fillCircle(WHEEL_CX + ICON_R, WHEEL_CY, r, SH110X_BLACK);
          display.drawCircle(WHEEL_CX + ICON_R, WHEEL_CY, r, SH110X_WHITE);
        }
      }
    }
    display.display();
  }
};

Menu menu;

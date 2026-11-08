#pragma once
#include "config.h"

// ============================================================
//  input.h  -  reads the 6 buttons with debouncing
//
//  Buttons connect GPIO -> GND, so we use the internal pull-ups.
//  A pressed button reads LOW; this class hides that from you.
//
//  USAGE (call input.update() ONCE at the top of every frame):
//      input.update();
//      if (input.pressed(BTN_A)) { ... }   // true for ONE frame when pushed
//      if (input.held(BTN_LEFT)) { ... }   // true the whole time it is down
//      if (input.released(BTN_B)) { ... }  // true for ONE frame when let go
//      if (input.repeat(BTN_DOWN)) { ... } // fires on press, then repeats
// ============================================================

enum Button : uint8_t { BTN_UP, BTN_DOWN, BTN_LEFT, BTN_RIGHT, BTN_A, BTN_B, BTN_COUNT };

const char* const BTN_NAMES[BTN_COUNT] = { "UP", "DOWN", "LEFT", "RIGHT", "A", "B" };

const uint8_t BTN_GPIO[BTN_COUNT] = {
  Pin::Up, Pin::Down, Pin::Left, Pin::Right, Pin::BtnA, Pin::BtnB
};

class InputManager {
public:
  void begin() {
    for (uint8_t i = 0; i < BTN_COUNT; i++) {
      pinMode(BTN_GPIO[i], INPUT_PULLUP);
      raw_[i] = false;
      stable_[i] = false;
      changedAt_[i] = 0;
      pressedAt_[i] = 0;
      lastRepeat_[i] = 0;
    }
    pressedMask_ = 0;
    releasedMask_ = 0;
  }

  // Call once per frame, before you check any button.
  void update() {
    const uint32_t now = millis();
    pressedMask_ = 0;
    releasedMask_ = 0;

    for (uint8_t i = 0; i < BTN_COUNT; i++) {
      const bool r = (digitalRead(BTN_GPIO[i]) == LOW);   // LOW = pressed

      // Restart the debounce timer whenever the raw reading changes.
      if (r != raw_[i]) {
        raw_[i] = r;
        changedAt_[i] = now;
      }

      // Accept the new state only once it has been stable long enough.
      if (raw_[i] != stable_[i] && (now - changedAt_[i]) >= DEBOUNCE_MS) {
        stable_[i] = raw_[i];
        if (stable_[i]) {
          pressedMask_ |= (1 << i);
          pressedAt_[i] = now;
          lastRepeat_[i] = now;
        } else {
          releasedMask_ |= (1 << i);
        }
      }
    }
  }

  bool held(Button b) const     { return stable_[b]; }
  bool pressed(Button b) const  { return pressedMask_ & (1 << b); }
  bool released(Button b) const { return releasedMask_ & (1 << b); }
  bool anyPressed() const       { return pressedMask_ != 0; }

  // How long (ms) the button has been held down; 0 if not held.
  uint32_t heldFor(Button b) const {
    return stable_[b] ? (millis() - pressedAt_[b]) : 0;
  }

  // Fires once on the press, then (after firstMs) every everyMs while held.
  // Great for menus and for moving a Tetris piece sideways.
  bool repeat(Button b, uint16_t firstMs = 300, uint16_t everyMs = 90) {
    if (pressed(b)) return true;
    if (!stable_[b]) return false;
    const uint32_t now = millis();
    if (now - pressedAt_[b] < firstMs) return false;
    if (now - lastRepeat_[b] >= everyMs) {
      lastRepeat_[b] = now;
      return true;
    }
    return false;
  }

private:
  bool     raw_[BTN_COUNT];
  bool     stable_[BTN_COUNT];
  uint32_t changedAt_[BTN_COUNT];
  uint32_t pressedAt_[BTN_COUNT];
  uint32_t lastRepeat_[BTN_COUNT];
  uint8_t  pressedMask_;
  uint8_t  releasedMask_;
};

// The one and only input object (the whole firmware is a single compilation
// unit, so a global defined in a header is fine here).
InputManager input;

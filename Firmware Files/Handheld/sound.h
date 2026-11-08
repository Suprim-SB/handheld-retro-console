#pragma once
#include "config.h"

// ============================================================
//  sound.h  -  plays beeps and little tunes on the passive buzzer
//
//  The notes are timed by a small background task (it runs next to
//  your main loop), so a jingle keeps perfect timing even while the
//  screen is busy drawing, and playing a sound never freezes a game.
//
//  USAGE (the global object is called 'sound'):
//      sound.begin();                                   // once, in setup()
//      sound.beep(880, 60);                             // 880 Hz for 60 ms
//      sound.play(SFX_SELECT, ARRAY_LEN(SFX_SELECT));   // a ready-made tune
//      sound.stop();
//
//  A frequency of 0 inside a sequence means "silence" (a rest).
//  Sequences must be 'static const' arrays like the ones below.
// ============================================================

// Note frequencies in Hz (A4 = 440). Use as Hz::C5, Hz::G5, ...
namespace Hz {
  constexpr uint16_t C4 = 262, D4 = 294, E4 = 330, F4 = 349, G4 = 392, A4 = 440, B4 = 494;
  constexpr uint16_t C5 = 523, D5 = 587, E5 = 659, F5 = 698, G5 = 784, A5 = 880, B5 = 988;
  constexpr uint16_t C6 = 1047, D6 = 1175, E6 = 1319, G6 = 1568;
  constexpr uint16_t C7 = 2093, E7 = 2637, G7 = 3136;
}

struct NoteStep {
  uint16_t freq;   // Hz, 0 = rest
  uint16_t ms;     // length in milliseconds
};

// ---------------- Ready-made sounds ----------------

// Boot: the "ba-ding" when GAME BOY lands
static const NoteStep SFX_BOOT_CHIME[] = { {Hz::C6, 90}, {Hz::G6, 260} };

// Boot: a burst of static while the logo glitches (about half a second)
static const NoteStep SFX_GLITCH[] = {
  {1800,12},{300,10},{2600,8},{150,16},{3100,10},{700,12},{0,14},{2200,10},
  {400,14},{3400,8},{120,18},{1500,10},{2800,12},{0,12},{900,10},{3300,14},
  {250,12},{2000,10},{3000,16},{180,14},{2400,10},{500,12},{3200,8},{0,16},
  {1100,12},{2900,10},{200,16},{3500,10},{800,14},{2300,12},{150,20},{3000,10},
  {600,14},{0,10},{2700,14},{350,16}
};

// Boot: rising "ding" with twinkles while it dissolves (about 1.1 s)
static const NoteStep SFX_SPARKLE[] = {
  {Hz::C6,70},{Hz::E6,70},{Hz::G6,70},{Hz::C7,90},{0,60},
  {Hz::E7,60},{0,30},{Hz::G7,60},{0,30},{Hz::E7,50},{0,30},
  {Hz::G7,90},{0,80},{Hz::C7,70},{0,40},{Hz::G7,200}
};

// Menu sounds
static const NoteStep SFX_TICK[]   = { {1800, 12} };                                          // wheel click
static const NoteStep SFX_SELECT[] = { {Hz::E5, 50}, {Hz::A5, 70} };                          // A on a slice
static const NoteStep SFX_LAUNCH[] = { {Hz::C5, 50}, {Hz::E5, 50}, {Hz::G5, 50}, {Hz::C6, 140} };  // start a game
static const NoteStep SFX_BACK[]   = { {Hz::E5, 40}, {Hz::C5, 70} };                          // going back

// Misc
static const NoteStep SFX_STARTUP[] = { {Hz::A5, 80}, {Hz::D6, 120} };
static const NoteStep SFX_ERROR[]   = { {200, 120}, {0, 60}, {200, 120}, {0, 60}, {200, 120} };

// ---------------- The engine ----------------

static portMUX_TYPE soundMux = portMUX_INITIALIZER_UNLOCKED;

class SoundEngine {
public:
  bool muted = false;

  void begin() {
    silence();
    xTaskCreate(taskEntry, "sound", 3072, this, 2, nullptr);
  }

  // Start a sequence of notes (replaces whatever is playing).
  void play(const NoteStep* seq, uint8_t len) {
    if (muted || !seq || len == 0) return;
    portENTER_CRITICAL(&soundMux);
    reqSeq_ = seq;
    reqLen_ = len;
    reqNew_ = true;
    reqStop_ = false;
    portEXIT_CRITICAL(&soundMux);
  }

  // Single beep.
  void beep(uint16_t freq, uint16_t ms) {
    single_[0] = { freq, ms };
    play(single_, 1);
  }

  void stop() {
    portENTER_CRITICAL(&soundMux);
    reqStop_ = true;
    reqNew_ = false;
    portEXIT_CRITICAL(&soundMux);
  }

  bool busy() const { return playing_; }

  // Not needed any more (the background task does the timing).
  // Kept so older code that calls it still compiles.
  void update() {}

private:
  // --- written by the main loop, read by the sound task ---
  const NoteStep* volatile reqSeq_ = nullptr;
  volatile uint8_t reqLen_  = 0;
  volatile bool    reqNew_  = false;
  volatile bool    reqStop_ = false;
  volatile bool    playing_ = false;
  NoteStep single_[1];

  // --- only used inside the sound task ---
  const NoteStep* seq_ = nullptr;
  uint8_t  len_ = 0;
  uint8_t  idx_ = 0;
  uint32_t noteStart_ = 0;

  static void taskEntry(void* self) { static_cast<SoundEngine*>(self)->run(); }

  void run() {
    for (;;) {
      bool doStop = false, doNew = false;
      const NoteStep* s = nullptr;
      uint8_t l = 0;

      portENTER_CRITICAL(&soundMux);
      if (reqStop_) { doStop = true; reqStop_ = false; }
      if (reqNew_)  { doNew = true; s = reqSeq_; l = reqLen_; reqNew_ = false; }
      portEXIT_CRITICAL(&soundMux);

      if (doStop) { seq_ = nullptr; playing_ = false; silence(); }
      if (doNew)  { seq_ = s; len_ = l; idx_ = 0; playing_ = true; startNote(); }

      if (seq_ && (millis() - noteStart_) >= seq_[idx_].ms) {
        if (++idx_ >= len_) {
          seq_ = nullptr;
          playing_ = false;
          silence();
        } else {
          startNote();
        }
      }
      vTaskDelay(1);   // check again in ~1 ms
    }
  }

  void startNote() {
    noteStart_ = millis();
    const uint16_t f = seq_[idx_].freq;
    if (f) tone(Pin::Buzzer, f);
    else   silence();
  }

  // Stop the tone and park the pin LOW so no DC current flows
  // through the buzzer coil.
  void silence() {
    noTone(Pin::Buzzer);
    pinMode(Pin::Buzzer, OUTPUT);
    digitalWrite(Pin::Buzzer, LOW);
  }
};

SoundEngine sound;

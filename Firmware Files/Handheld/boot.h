#pragma once
#include <math.h>
#include "gfx.h"
#include "input.h"
#include "sound.h"

//  boot.h  -  the start-up animation
//
//    0.0 s   "GAME BOY" slides down from the top
//    0.65 s  "A D V A N C E D" types in, chime plays, a shine sweeps across
//    2.0 s   it GLITCHESand changes into
//            "UNADVANCED" with the UN highlighted
//    3.6 s   it dissolves into sparkles with a rising ding
//    4.6 s   once its finished the menu appears
//
//  Press any button to skip it.
//  All the timings are in the BootT block below - change them if you want

namespace BootT {                       // times is in millisconds
  constexpr uint32_t SlideEnd     = 600;
  constexpr uint32_t SubStart     = 650;
  constexpr uint32_t LetterMs     = 45;    // time between typed letters
  constexpr uint32_t ShineStart   = 1100;
  constexpr uint32_t ShineLen     = 400;
  constexpr uint32_t GlitchStart  = 2000;
  constexpr uint32_t GlitchEnd    = 2900;
  constexpr uint32_t FlashEnd     = 3300;  // UN blinks until here
  constexpr uint32_t SparkleStart = 3600;
  constexpr uint32_t End          = 4600;
}

constexpr int BOOT_TITLE_Y = 18;   
constexpr int BOOT_SUB_Y   = 40;    
constexpr int BOOT_GAP     = 3;     

static const char BOOT_GLITCH_CHARS[] = "#%&@?!/<>=+*0123456789";


static void bootTitle(int y) {
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(16, y);                
  display.print("GAME BOY");
}

static void bootSubtitle(const char* s, int shown, bool hlUN) {
  const int n  = (int)strlen(s);
  const int x0 = (SCREEN_W - (n * (6 + BOOT_GAP) - BOOT_GAP)) / 2;
  display.setTextSize(1);
  if (hlUN) {
    display.fillRect(x0 - 2, BOOT_SUB_Y - 2, 2 * (6 + BOOT_GAP) - BOOT_GAP + 4, 11, SH110X_WHITE);
  }
  for (int i = 0; i < n && i < shown; i++) {
    display.setTextColor((hlUN && i < 2) ? SH110X_BLACK : SH110X_WHITE);
    display.setCursor(x0 + i * (6 + BOOT_GAP), BOOT_SUB_Y);
    display.print(s[i]);
  }
  display.setTextColor(SH110X_WHITE);
}


// Phase 1: slide in, type, shine.
static void bootIntro(uint32_t t) {
  int y = BOOT_TITLE_Y;
  if (t < BootT::SlideEnd) {
    float p = (float)t / BootT::SlideEnd;
    float e = 1.0f - (1.0f - p) * (1.0f - p) * (1.0f - p);        // ease-out
    y = (int)(-16 + (BOOT_TITLE_Y + 16) * e);
  }
  bootTitle(y);

  if (t >= BootT::SubStart) {
    int shown = (int)((t - BootT::SubStart) / BootT::LetterMs) + 1;
    if (shown > 8) shown = 8;
    bootSubtitle("ADVANCED", shown, false);
  }

  if (t >= BootT::ShineStart && t < BootT::ShineStart + BootT::ShineLen) {
    float p = (float)(t - BootT::ShineStart) / BootT::ShineLen;
    int sx = (int)(-12 + p * 150);
    for (int yy = BOOT_TITLE_Y - 2; yy < BOOT_SUB_Y + 10; yy++) {
      int x0 = sx - (yy - BOOT_TITLE_Y) / 3;
      for (int xx = x0; xx < x0 + 6; xx++) {
        if (pxGet(xx, yy)) pxSet(xx, yy, false);
      }
    }
  }
}

static void bootGlitch(uint32_t t) {
  const float g = (float)(t - BootT::GlitchStart) / (BootT::GlitchEnd - BootT::GlitchStart); 
  const float intensity = sinf(3.14159f * g);       
  const bool  second = g > 0.5f;                    
  const char* base = second ? "UNADVANCED" : "ADVANCED";
  char tmp[16];
  const int n = (int)strlen(base);
  const int corruptPct = (int)(60.0f * intensity);
  for (int i = 0; i < n; i++) {
    tmp[i] = (random(0, 100) < corruptPct)
               ? BOOT_GLITCH_CHARS[random(0, (int)sizeof(BOOT_GLITCH_CHARS) - 1)]
               : base[i];
  }
  tmp[n] = 0;

  int ty = BOOT_TITLE_Y;
  if (random(0, 100) < (int)(40.0f * intensity)) ty += random(-2, 3);  
  bootTitle(ty);
  bootSubtitle(tmp, n, second && g > 0.7f);

  const int bands = 1 + (int)(intensity * 5.0f);
  for (int b = 0; b < bands; b++) {
    shiftRows(random(6, 58), random(2, 9), random(-18, 19));
  }

  const int blocks = (int)(intensity * 3.0f);
  for (int b = 0; b < blocks; b++) {
    invertRect(random(0, 100), random(8, 56), random(8, 40), random(1, 5));
  }

  const int dots = (int)(intensity * 40.0f);
  for (int d = 0; d < dots; d++) {
    display.drawPixel(random(0, SCREEN_W), random(0, SCREEN_H), SH110X_WHITE);
  }
 
  if (random(0, 100) < (int)(8.0f * intensity)) invertRect(0, 0, SCREEN_W, SCREEN_H);
}

static void bootHold(uint32_t t) {
  bool hl = true;
  if (t < BootT::FlashEnd) hl = (((t - BootT::GlitchEnd) / 110) % 2) == 0;
  bootTitle(BOOT_TITLE_Y);
  bootSubtitle("UNADVANCED", 10, hl);
}

static void bootDissolve(uint32_t t) {
  const float p = (float)(t - BootT::SparkleStart) / (BootT::End - BootT::SparkleStart);
  bootTitle(BOOT_TITLE_Y);
  bootSubtitle("UNADVANCED", 10, true);

  const int thr = (int)(p * 300.0f);
  for (int y = 10; y < 58; y++) {
    for (int x = 0; x < SCREEN_W; x++) {
      if (pxGet(x, y) && (thr >= 255 || hash8(x, y) < thr)) pxSet(x, y, false);
    }
  }

  for (int j = 0; j < 18; j++) {
    const float start = (hash8(j, 7) / 255.0f) * 0.7f;
    const float q = (p - start) / 0.25f;
    if (q < 0.0f || q >= 1.0f) continue;
    const float k = (q < 0.5f) ? q * 2.0f : (1.0f - q) * 2.0f;     
    const int r = (int)(k * 3.99f);
    const int x = 8 + (hash8(j, 11) * 112) / 255;
    const int y = 14 + (hash8(j, 13) * 40) / 255;
    drawSparkle(x, y, r);
  }
}

static void runBoot() {
  const uint32_t t0 = millis();
  bool chimed = false, glitched = false, jingled = false;

  for (;;) {
    input.update();
    if (input.anyPressed()) { sound.stop(); break; }      // any button skips

    const uint32_t t = millis() - t0;
    if (t >= BootT::End) break;

    if (!chimed   && t >= BootT::SubStart)     { chimed   = true; sound.play(SFX_BOOT_CHIME, ARRAY_LEN(SFX_BOOT_CHIME)); }
    if (!glitched && t >= BootT::GlitchStart)  { glitched = true; sound.play(SFX_GLITCH,     ARRAY_LEN(SFX_GLITCH)); }
    if (!jingled  && t >= BootT::SparkleStart) { jingled  = true; sound.play(SFX_SPARKLE,    ARRAY_LEN(SFX_SPARKLE)); }

    display.clearDisplay();
    if      (t < BootT::GlitchStart)  bootIntro(t);
    else if (t < BootT::GlitchEnd)    bootGlitch(t);
    else if (t < BootT::SparkleStart) bootHold(t);
    else                              bootDissolve(t);
    display.display();
  }

  display.clearDisplay();
  display.display();
}

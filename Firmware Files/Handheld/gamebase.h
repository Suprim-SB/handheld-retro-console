#pragma once
#include <stdio.h>
#include <math.h>
#include "gfx.h"
#include "input.h"
#include "sound.h"

// ============================================================
//  gamebase.h  -  what every game has in common
//
//  A game is a class with four jobs:
//     reset()   start a fresh run
//     update()  move everything forward by 'dtMs' milliseconds.
//               Return true when the player has died.
//     draw()    draw the playfield (the runner clears the screen first)
//     score()   the current score
//
//  The top 8 pixel rows (HUD_H) belong to the runner: it prints the
//  high score on the left and the score on the right. Games draw in
//  the field below that (FIELD_Y .. 63).
//
//  B is never used by a game: holding B leaves the game (that is
//  handled by the runner in games.h).
// ============================================================

constexpr int HUD_H   = 8;
constexpr int FIELD_Y = HUD_H;
constexpr int FIELD_H = SCREEN_H - HUD_H;     // 56 rows

// Sounds shared by every game
static const NoteStep SFX_GAMEOVER[] = { {Hz::G5, 110}, {Hz::E5, 110}, {Hz::C5, 110}, {Hz::G4, 380} };
static const NoteStep SFX_NEWBEST[]  = { {Hz::C5, 90}, {Hz::E5, 90}, {Hz::G5, 90}, {Hz::C6, 120},
                                         {0, 40}, {Hz::G5, 80}, {Hz::C6, 320} };

// Draw a little picture made of text rows: '#' = pixel on.
static void drawSprite(int x, int y, const char* const* rows, int h,
                       uint16_t color = SH110X_WHITE) {
  for (int r = 0; r < h; r++) {
    const char* row = rows[r];
    for (int c = 0; row[c]; c++) {
      if (row[c] == '#') display.drawPixel(x + c, y + r, color);
    }
  }
}

class Game {
public:
  virtual ~Game() {}
  virtual void reset() = 0;
  virtual bool update(uint32_t dtMs) = 0;
  virtual void draw() = 0;
  virtual int  score() const = 0;
  virtual void drawHudExtra() {}      // optional: lives etc. in the middle of the top bar
};

#pragma once
#include <Preferences.h>
#include "gamebase.h"
#include "icons.h"
#include "snake.h"
#include "breakout.h"
#include "racer.h"
#include "shooter.h"
#include "pong.h"

//  games.h  -  the game list, high scores and the "runner"
//
//  The runner plays one game from start to finish:
//    playing : top bar (HI score left, score right), hold B to leave
//    dead   : the screen goes WHITE with GAME OVER
//    score : your score and the best score; A = retry, B = menu
//
//  Game order (must match the menu in icons.h):
//    0 SNAKE  1 BRICK BREAKER   2 BOAT RACER   3 ZOMBIE SHOOTER   4 PONG

static SnakeGame    gSnake;
static BreakoutGame gBreakout;
static RacerGame    gRacer;
static ShooterGame  gShooter;
static PongGame     gPong;

Game* const GAME_TABLE[GAME_COUNT] = { &gSnake, &gBreakout, &gRacer, &gShooter, &gPong };

// ---------------- high scores (kept in flash, survive switching off) ----------------

class HighScores {
public:
  void begin() {
    Preferences p;
    p.begin("handheld", false);
    for (int i = 0; i < GAME_COUNT; i++) best_[i] = p.getInt(key(i), 0);
    p.end();
  }
  int get(int g) const { return best_[g]; }

  // Returns true if this is a new record (and saves it).
  bool submit(int g, int s) {
    if (s <= best_[g]) return false;
    best_[g] = s;
    Preferences p;
    p.begin("handheld", false);
    p.putInt(key(g), s);
    p.end();
    return true;
  }

private:
  int best_[GAME_COUNT] = { 0 };
  static const char* key(int g) {
    static const char* const k[GAME_COUNT] = { "hs0", "hs1", "hs2", "hs3", "hs4" };
    return k[g];
  }
};

HighScores highScores;

// ---------------- runner ----------------

class GameRunner {
public:
  // Start (or restart) game number 'idx'.
  void begin(int idx) {
    idx_ = idx;
    game_ = GAME_TABLE[idx];
    startRun();
  }

  // Call once per frame (after input.update()).
  // Returns true when the player wants to go back to the menu.
  bool frame() {
    const uint32_t now = millis();
    uint32_t dt = now - last_;
    last_ = now;
    if (dt > 50) dt = 50;                              

    if (phase_ == P_PLAY) {
      if (input.heldFor(BTN_B) >= EXIT_HOLD_MS) { sound.stop(); return true; }

      const bool dead = game_->update(dt);

      display.clearDisplay();
      game_->draw();
      drawHud();
      drawExitBar();
      display.display();

      if (dead) {
        finalScore_ = game_->score();
        newBest_ = highScores.submit(idx_, finalScore_);
        phase_ = P_WHITE;
        phaseStart_ = now;
        sound.play(SFX_GAMEOVER, ARRAY_LEN(SFX_GAMEOVER));
      }
      return false;
    }

    if (phase_ == P_WHITE) {                               // white screen, GAME OVER
      display.fillRect(0, 0, SCREEN_W, SCREEN_H, SH110X_WHITE);
      display.setTextColor(SH110X_BLACK);
      printCentered("GAME OVER", 24, 2);
      display.setTextColor(SH110X_WHITE);
      display.display();
      if (now - phaseStart_ >= 1600) {
        phase_ = P_SCORE;
        phaseStart_ = now;
        if (newBest_) sound.play(SFX_NEWBEST, ARRAY_LEN(SFX_NEWBEST));
      }
      return false;
    }

    // P_SCORE
    char buf[24];
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    printCentered("SCORE", 2, 1);
    snprintf(buf, sizeof buf, "%d", finalScore_);
    printCentered(buf, 13, 3);
    if (newBest_) {
      if ((now / 350) % 2 == 0) printCentered("NEW BEST!", 40, 1);
    } else {
      snprintf(buf, sizeof buf, "BEST: %d", highScores.get(idx_));
      printCentered(buf, 40, 1);
    }
    printCentered("A:RETRY   B:MENU", 55, 1);
    display.display();

    if (now - phaseStart_ > 300) {                         // ignore button-mashing from the game
      if (input.pressed(BTN_A)) { sound.play(SFX_SELECT, ARRAY_LEN(SFX_SELECT)); startRun(); }
      else if (input.pressed(BTN_B)) { sound.play(SFX_BACK, ARRAY_LEN(SFX_BACK)); return true; }
    }
    return false;
  }

private:
  enum Phase : uint8_t { P_PLAY, P_WHITE, P_SCORE };

  Game*    game_ = nullptr;
  int      idx_ = 0;
  Phase    phase_ = P_PLAY;
  uint32_t last_ = 0, phaseStart_ = 0;
  int      finalScore_ = 0;
  bool     newBest_ = false;

  void startRun() {
    game_->reset();
    phase_ = P_PLAY;
    last_ = millis();
  }

  void drawHud() {
    char b[20];
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);

    int hi = highScores.get(idx_);
    if (game_->score() > hi) hi = game_->score();           // the HI number keeps up with you
    snprintf(b, sizeof b, "HI:%d", hi);
    display.setCursor(0, 0);
    display.print(b);

    snprintf(b, sizeof b, "SC:%d", game_->score());
    display.setCursor(SCREEN_W - (int)strlen(b) * 6, 0);
    display.print(b);

    game_->drawHudExtra();
  }

  // While B is held, show a bar filling up; when full you leave the game.
  void drawExitBar() {
    const uint32_t held = input.heldFor(BTN_B);
    if (held < 250) return;
    uint32_t h = held > EXIT_HOLD_MS ? EXIT_HOLD_MS : held;
    display.fillRect(40, 54, 48, 10, SH110X_BLACK);
    display.drawRect(42, 56, 44, 6, SH110X_WHITE);
    display.fillRect(44, 58, (int)(h * 40UL / EXIT_HOLD_MS), 2, SH110X_WHITE);
  }
};

GameRunner runner;

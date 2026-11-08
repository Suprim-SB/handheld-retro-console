#pragma once
#include "gamebase.h"


//  snake.h  -  SNEK
//  D-pad steers. Hitting the wall or yourself ends the run.
//  Score = how many pieces of food you have eaten.

static const NoteStep SFX_SNAKE_EAT[] = { {Hz::A5, 25}, {Hz::E6, 35} };

class SnakeGame : public Game {
public:
  void reset() override {
    len_ = 3;
    for (int i = 0; i < 3; i++) { sx_[i] = 15 - i; sy_[i] = 6; }   
    dir_ = 1;          
    qn_ = 0;
    acc_ = 0;
    eaten_ = 0;
    placeFood();
  }

  bool update(uint32_t dt) override {
    readInput();
    acc_ += dt;
    int interval = 140 - eaten_ * 3;          
    if (interval < 70) interval = 70;
    while (acc_ >= (uint32_t)interval) {
      acc_ -= interval;
      if (step()) return true;
    }
    return false;
  }

  void draw() override {
    display.drawRect(2, 9, 124, 54, SH110X_WHITE);         

    for (int i = 1; i < len_; i++) {                       
      display.fillRect(OX + sx_[i] * CELL, OY + sy_[i] * CELL, 3, 3, SH110X_WHITE);
    }
    display.fillRect(OX + sx_[0] * CELL, OY + sy_[0] * CELL, 4, 4, SH110X_WHITE);  

    const int fx = OX + fx_ * CELL, fy = OY + fy_ * CELL;   
    if ((millis() / 220) % 2 == 0) {
      display.fillRect(fx + 1, fy, 1, 3, SH110X_WHITE);
      display.fillRect(fx, fy + 1, 3, 1, SH110X_WHITE);
    } else {
      display.fillRect(fx, fy, 3, 3, SH110X_WHITE);
    }
  }

  int score() const override { return eaten_; }

private:
  static constexpr int COLS = 30, ROWS = 13, CELL = 4, OX = 4, OY = 10;
  static constexpr int MAXLEN = COLS * ROWS;

  int8_t   sx_[MAXLEN], sy_[MAXLEN];
  int      len_ = 3;
  int8_t   dir_ = 1;
  int8_t   q_[2];            
  int      qn_ = 0;
  uint32_t acc_ = 0;
  int      eaten_ = 0;
  int      fx_ = 0, fy_ = 0;

  void readInput() {
    const Button btn[4] = { BTN_UP, BTN_RIGHT, BTN_DOWN, BTN_LEFT }; 
    for (int d = 0; d < 4; d++) {
      if (!input.pressed(btn[d])) continue;
      const int last = qn_ ? q_[qn_ - 1] : dir_;
      if (d != last && d != (last + 2) % 4 && qn_ < 2) q_[qn_++] = d;
    }
  }


  bool step() {
    static const int8_t DX[4] = { 0, 1, 0, -1 };
    static const int8_t DY[4] = { -1, 0, 1, 0 };

    if (qn_) { dir_ = q_[0]; q_[0] = q_[1]; qn_--; }
    const int nx = sx_[0] + DX[dir_];
    const int ny = sy_[0] + DY[dir_];

    if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) return true;   

    const bool eat = (nx == fx_ && ny == fy_);
    const int checkTo = eat ? len_ : len_ - 1;    
    for (int i = 0; i < checkTo; i++) {
      if (sx_[i] == nx && sy_[i] == ny) return true;             
    }

    if (eat && len_ < MAXLEN) len_++;
    for (int i = len_ - 1; i > 0; i--) { sx_[i] = sx_[i - 1]; sy_[i] = sy_[i - 1]; }
    sx_[0] = nx;
    sy_[0] = ny;

    if (eat) {
      eaten_++;
      sound.play(SFX_SNAKE_EAT, ARRAY_LEN(SFX_SNAKE_EAT));
      if (len_ >= MAXLEN) return true;            
      placeFood();
    }
    return false;
  }

  void placeFood() {
    for (int tries = 0; tries < 400; tries++) {
      const int x = random(0, COLS), y = random(0, ROWS);
      if (!onSnake(x, y)) { fx_ = x; fy_ = y; return; }
    }
    for (int y = 0; y < ROWS; y++)                 
      for (int x = 0; x < COLS; x++)
        if (!onSnake(x, y)) { fx_ = x; fy_ = y; return; }
  }

  bool onSnake(int x, int y) const {
    for (int i = 0; i < len_; i++) if (sx_[i] == x && sy_[i] == y) return true;
    return false;
  }
};

#pragma once
#include "gamebase.h"

// ============================================================
//  breakout.h  -  BRICK BREAKER
//  LEFT/RIGHT move the paddle, A launches the ball.
//  3 lives. Clear every brick to go to the next, faster level.
// ============================================================

static const NoteStep SFX_BRK_LOST[]  = { {300, 100}, {220, 120}, {140, 200} };
static const NoteStep SFX_BRK_LEVEL[] = { {Hz::C5, 70}, {Hz::E5, 70}, {Hz::G5, 70}, {Hz::C6, 160} };

class BreakoutGame : public Game {
public:
  void reset() override {
    score_ = 0;
    lives_ = 3;
    level_ = 1;
    dead_ = false;
    padX_ = (SCREEN_W - PAD_W) / 2.0f;
    fillBricks();
    launched_ = false;
  }

  bool update(uint32_t dtMs) override {
    const float dt = dtMs / 1000.0f;

    int dir = 0;
    if (input.held(BTN_LEFT))  dir -= 1;
    if (input.held(BTN_RIGHT)) dir += 1;
    padX_ += dir * PAD_SPEED * dt;
    if (padX_ < 0) padX_ = 0;
    if (padX_ > SCREEN_W - PAD_W) padX_ = SCREEN_W - PAD_W;

    if (!launched_) {                       // ball rides on the paddle until A is pressed
      ballX_ = padX_ + PAD_W / 2.0f - 1.5f;
      ballY_ = PAD_Y - 4.0f;
      if (input.pressed(BTN_A)) {
        launched_ = true;
        const float a = random(-30, 31) * 0.0174533f;
        vx_ = speed() * sinf(a);
        vy_ = -speed() * cosf(a);
        sound.beep(700, 30);
      }
      return dead_;
    }

    // Move the ball in tiny steps so it can never jump through a brick.
    const float sp = sqrtf(vx_ * vx_ + vy_ * vy_);
    int n = (int)ceilf(sp * dt);
    if (n < 1) n = 1;
    const float sdt = dt / n;
    for (int i = 0; i < n && launched_; i++) stepBall(sdt);

    if (!dead_ && bricksLeft_ == 0) nextLevel();
    return dead_;
  }

  void draw() override {
    for (int r = 0; r < BR_ROWS; r++)
      for (int c = 0; c < BR_COLS; c++)
        if (bricks_[r][c])
          display.fillRect(BR_X0 + c * BR_PX, BR_Y0 + r * BR_PY, BR_W, BR_H, SH110X_WHITE);

    display.fillRect((int)padX_, PAD_Y, PAD_W, PAD_H, SH110X_WHITE);
    display.fillRect((int)ballX_, (int)ballY_, 3, 3, SH110X_WHITE);

    if (!launched_ && (millis() / 400) % 2 == 0) {
      display.setTextColor(SH110X_WHITE);
      printCentered("A: LAUNCH", 40, 1);
    }
  }

  void drawHudExtra() override {            // lives as little squares in the top bar
    for (int i = 0; i < lives_; i++) display.fillRect(52 + i * 7, 2, 4, 4, SH110X_WHITE);
  }

  int score() const override { return score_; }

private:
  static constexpr int   BR_ROWS = 5, BR_COLS = 8;
  static constexpr int   BR_W = 14, BR_H = 4, BR_PX = 16, BR_PY = 5, BR_X0 = 1, BR_Y0 = 12;
  static constexpr int   PAD_W = 22, PAD_H = 3, PAD_Y = 59;
  static constexpr float PAD_SPEED = 120.0f;

  bool  bricks_[BR_ROWS][BR_COLS];
  int   bricksLeft_ = 0;
  float padX_ = 0, ballX_ = 0, ballY_ = 0, vx_ = 0, vy_ = 0;
  bool  launched_ = false, dead_ = false;
  int   score_ = 0, lives_ = 3, level_ = 1;

  float speed() const {
    float s = 58.0f + 6.0f * (level_ - 1) + (BR_ROWS * BR_COLS - bricksLeft_) * 0.35f;
    return s > 125.0f ? 125.0f : s;
  }

  void fillBricks() {
    for (int r = 0; r < BR_ROWS; r++)
      for (int c = 0; c < BR_COLS; c++) bricks_[r][c] = true;
    bricksLeft_ = BR_ROWS * BR_COLS;
  }

  void nextLevel() {
    level_++;
    fillBricks();
    launched_ = false;
    sound.play(SFX_BRK_LEVEL, ARRAY_LEN(SFX_BRK_LEVEL));
  }

  // Which brick (if any) is under this point?
  bool brickAt(float x, float y, int& r, int& c) const {
    const float fx = x - BR_X0, fy = y - BR_Y0;
    if (fx < 0 || fy < 0) return false;
    c = (int)(fx / BR_PX);
    r = (int)(fy / BR_PY);
    if (c >= BR_COLS || r >= BR_ROWS) return false;
    if (fx - c * BR_PX >= BR_W || fy - r * BR_PY >= BR_H) return false;   // in the gap
    return bricks_[r][c];
  }

  void hitBrick(int r, int c) {
    bricks_[r][c] = false;
    bricksLeft_--;
    score_ += 10 + 5 * (BR_ROWS - 1 - r);                 // higher rows are worth more
    sound.beep(500 + (BR_ROWS - 1 - r) * 110, 25);        // higher rows sound higher
  }

  void stepBall(float sdt) {
    int r, c;

    // --- horizontal ---
    ballX_ += vx_ * sdt;
    if (ballX_ < 0)                  { ballX_ = 0;                 vx_ =  fabsf(vx_); sound.beep(330, 12); }
    else if (ballX_ > SCREEN_W - 3)  { ballX_ = SCREEN_W - 3.0f;   vx_ = -fabsf(vx_); sound.beep(330, 12); }
    if (brickAt(ballX_ + 1.5f, ballY_ + 1.5f, r, c)) {
      ballX_ -= vx_ * sdt;
      vx_ = -vx_;
      hitBrick(r, c);
    }

    // --- vertical ---
    ballY_ += vy_ * sdt;
    if (ballY_ < FIELD_Y + 1) { ballY_ = FIELD_Y + 1.0f; vy_ = fabsf(vy_); sound.beep(330, 12); }
    if (brickAt(ballX_ + 1.5f, ballY_ + 1.5f, r, c)) {
      ballY_ -= vy_ * sdt;
      vy_ = -vy_;
      hitBrick(r, c);
    }

    // --- paddle ---
    if (vy_ > 0 && ballY_ + 3 >= PAD_Y && ballY_ < PAD_Y + PAD_H &&
        ballX_ + 3 >= padX_ - 1 && ballX_ <= padX_ + PAD_W + 1) {
      float rel = ((ballX_ + 1.5f) - (padX_ + PAD_W / 2.0f)) / (PAD_W / 2.0f);
      if (rel < -1) rel = -1;
      if (rel >  1) rel = 1;
      const float ang = rel * 1.0472f;                    // up to 60 degrees from straight up
      vx_ =  speed() * sinf(ang);
      vy_ = -speed() * cosf(ang);
      ballY_ = PAD_Y - 3.0f;
      sound.beep(520, 18);
    }

    // --- fell out of the bottom ---
    if (ballY_ > SCREEN_H) {
      lives_--;
      sound.play(SFX_BRK_LOST, ARRAY_LEN(SFX_BRK_LOST));
      launched_ = false;
      if (lives_ <= 0) dead_ = true;
    }
  }
};

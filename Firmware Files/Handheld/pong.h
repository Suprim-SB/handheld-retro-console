#pragma once
#include "gamebase.h"

//  pong.h  -  PONGGG
//    - you control the LEFT paddle with UP and DOWN
//    - the computer controls the RIGHT paddle
//    - every time you hit the ball back you score 1 point
//    - if the ball gets past your paddle, the game is over


class PongGame : public Game {
public:
  void reset() override {
    playerY_ = (FIELD_Y + SCREEN_H - PADDLE_H) / 2.0f;
    aiY_     = playerY_;

    ballX_   = SCREEN_W / 2.0f;
    ballY_   = (FIELD_Y + SCREEN_H) / 2.0f;

    ballVX_  = -60.0f;
    ballVY_  = 25.0f;

    score_   = 0;
  }

  bool update(uint32_t dtMs) override {
    const float dt = dtMs / 1000.0f;


    if (input.held(BTN_UP))
      playerY_ -= PADDLE_SPEED * dt;

    if (input.held(BTN_DOWN))
      playerY_ += PADDLE_SPEED * dt;

    if (playerY_ < FIELD_Y)
      playerY_ = FIELD_Y;

    if (playerY_ > SCREEN_H - PADDLE_H)
      playerY_ = SCREEN_H - PADDLE_H;


 
    ballX_ += ballVX_ * dt;
    ballY_ += ballVY_ * dt;
 
    if (ballY_ <= FIELD_Y) {
      ballY_ = FIELD_Y;
      ballVY_ = -ballVY_;
      sound.beep(800, 20);
    }

    if (ballY_ >= SCREEN_H - 3) {
      ballY_ = SCREEN_H - 3;
      ballVY_ = -ballVY_;
      sound.beep(800, 20);
    }
    if (ballX_ > SCREEN_W) {
      score_ += 5;
      ballX_  = SCREEN_W / 2.0f;
      ballY_  = (FIELD_Y + SCREEN_H) / 2.0f;
      ballVX_ = -80.0f;
      ballVY_ = random(0, 2) ? 25.0f : -25.0f;
      sound.beep(1000, 40);
    }
 

    if (ballVX_ < 0 &&
        ballX_ <= 2 + PADDLE_W &&
        ballX_ + 3 >= 2 &&
        ballY_ + 3 >= playerY_ &&
        ballY_ <= playerY_ + PADDLE_H) {

      ballX_ = 2 + PADDLE_W;

      ballVX_ = -ballVX_;
      score_++;

      float hitPosition =
          (ballY_ + 1.5f) - (playerY_ + PADDLE_H / 2.0f);

      ballVY_ = hitPosition * 6.0f;

      sound.beep(600, 20);

 
      ballVX_ *= 1.05f;
      ballVY_ *= 1.05f;
    }
     if (ballVX_ > 200.0f) ballVX_ = 200.0f;

    float aiCenter = aiY_ + PADDLE_H / 2.0f;
    float ballCenter = ballY_ + 1.5f;

    if (ballCenter < aiCenter)
      aiY_ -= AI_SPEED * dt;
    else
      aiY_ += AI_SPEED * dt;


    if (aiY_ < FIELD_Y)
      aiY_ = FIELD_Y;

    if (aiY_ > SCREEN_H - PADDLE_H)
      aiY_ = SCREEN_H - PADDLE_H;


 
    if (ballVX_ > 0 &&
        ballX_ + 3 >= SCREEN_W - 2 - PADDLE_W &&
        ballX_ <= SCREEN_W - 2 &&
        ballY_ + 3 >= aiY_ &&
        ballY_ <= aiY_ + PADDLE_H) {

      ballX_ = SCREEN_W - 2 - PADDLE_W - 3;

      ballVX_ = -ballVX_;

    
      float hitPosition =
          (ballY_ + 1.5f) - (aiY_ + PADDLE_H / 2.0f);

      ballVY_ = hitPosition * 5.0f;

      sound.beep(400, 20);
    }



    if (ballX_ < 0)
      return true;


    return false;
  }


  void draw() override {

    for (int y = FIELD_Y; y < SCREEN_H; y += 4)
      display.drawPixel(SCREEN_W / 2, y, SH110X_WHITE);

    display.fillRect(
        2, (int)playerY_,
        PADDLE_W, PADDLE_H,
        SH110X_WHITE
    );

    display.fillRect(
        SCREEN_W - 2 - PADDLE_W,
        (int)aiY_,
        PADDLE_W, PADDLE_H,
        SH110X_WHITE
    );

    display.fillRect(
        (int)ballX_,
        (int)ballY_,
        3, 3,
        SH110X_WHITE
    );
  }


  int score() const override {
    return score_;
  }


private:
  static constexpr int PADDLE_W = 3;
  static constexpr int PADDLE_H = 14;

  static constexpr float PADDLE_SPEED = 80.0f;
  static constexpr float AI_SPEED = 55.0f;

  float playerY_ = 0;
  float aiY_ = 0;

  float ballX_ = 0;
  float ballY_ = 0;

  float ballVX_ = 0;
  float ballVY_ = 0;

  int score_ = 0;
};

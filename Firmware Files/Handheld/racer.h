#pragma once
#include "gamebase.h"

//  racer.h  -  BOAT RACE
//  You are a boat sailing up the sea. Spiky sea mines drift down
//  the screen - steer around them!
//     LEFT / RIGHT  steer      UP  speed up      DOWN  slow down
//  Score = distance sailed, plus a bonus for every mine you pass.

static const NoteStep SFX_RACE_START[] = { {Hz::C5, 60}, {Hz::E5, 60}, {Hz::G5, 110} };
static const NoteStep SFX_RACE_BOOM[]  = { {220, 35}, {120, 35}, {320, 30}, {90, 45}, {180, 40}, {70, 70}, {50, 120} };

class RacerGame : public Game {
public:
  void reset() override {
    boatX_ = 64;
    dist_ = 0;
    elapsed_ = 0;
    dodged_ = 0;
    spawnAcc_ = 0;
    gap_ = 30;
    scroll_ = 0;
    dyingMs_ = 0;
    for (int i = 0; i < MAX_MINES; i++) mines_[i].active = false;
    for (int i = 0; i < RIPPLES; i++) {
      rx_[i] = random(6, 122);
      ry_[i] = random(FIELD_Y, SCREEN_H);
    }
    sound.play(SFX_RACE_START, ARRAY_LEN(SFX_RACE_START));
  }

  bool update(uint32_t dtMs) override {
    if (dyingMs_ > 0) {                                   
      dyingMs_ = (dtMs >= dyingMs_) ? 0 : dyingMs_ - dtMs;
      return dyingMs_ == 0;
    }

    const float dt = dtMs / 1000.0f;
    elapsed_ += dt;

 
    int dir = 0;
    if (input.held(BTN_LEFT))  dir -= 1;
    if (input.held(BTN_RIGHT)) dir += 1;
    boatX_ += dir * 80.0f * dt;
    if (boatX_ < 8)   boatX_ = 8;
    if (boatX_ > 119) boatX_ = 119;


    float diff = elapsed_ / 100.0f;
    if (diff > 1.0f) diff = 1.0f;
    float throttle = 1.0f;
    if (input.held(BTN_UP))   throttle = 1.5f;
    if (input.held(BTN_DOWN)) throttle = 0.6f;
    const float speed = (36.0f + 54.0f * diff) * throttle;     
    const float adv = speed * dt;
    dist_ += adv;
    scroll_ += adv;

    
    for (int i = 0; i < RIPPLES; i++) {
      ry_[i] += adv;
      if (ry_[i] >= SCREEN_H) { ry_[i] = FIELD_Y; rx_[i] = random(6, 122); }
    }

   
    spawnAcc_ += adv;
    if (spawnAcc_ >= gap_) {
      spawnAcc_ = 0;
      gap_ = random(26, 46) - (int)(10 * diff);
      const float x1 = random(10, 118);
      spawn(x1);
      if (random(0, 100) < 15 + 30 * diff) {             
        float x2 = x1 + (random(0, 2) ? 34 : -34);
        if (x2 < 10) x2 = x1 + 34;
        if (x2 > 118) x2 = x1 - 34;
        spawn(x2);
      }
    }

   
    for (int i = 0; i < MAX_MINES; i++) {
      Mine& m = mines_[i];
      if (!m.active) continue;
      m.y += adv;
      if (!m.passed && m.y > BOAT_Y + 9) {
        m.passed = true;
        dodged_++;
        sound.beep(1300, 12);
      }
      if (m.y > SCREEN_H + 8) { m.active = false; continue; }
      if (hits(m)) {
        dyingMs_ = 750;
        sound.play(SFX_RACE_BOOM, ARRAY_LEN(SFX_RACE_BOOM));
        break;
      }
    }
    return false;
  }

  void draw() override {
  
    const int off = (int)scroll_ % 6;
    for (int y = FIELD_Y; y < SCREEN_H; y++) {
      if ((y + off) % 6 < 3) { display.drawPixel(2, y, SH110X_WHITE); display.drawPixel(125, y, SH110X_WHITE); }
    }
  
    for (int i = 0; i < RIPPLES; i++) {
      const int x = (int)rx_[i], y = (int)ry_[i];
      display.drawPixel(x, y, SH110X_WHITE);
      display.drawPixel(x + 1, y - 1, SH110X_WHITE);
      display.drawPixel(x + 2, y, SH110X_WHITE);
      display.drawPixel(x + 3, y - 1, SH110X_WHITE);
    }
    
    for (int i = 0; i < MAX_MINES; i++) {
      if (mines_[i].active && mines_[i].y > FIELD_Y - 6) drawMine((int)mines_[i].x, (int)mines_[i].y);
    }

    if (dyingMs_ > 0) {
      const int r = (int)((750 - dyingMs_) * 14 / 750);
      display.drawCircle((int)boatX_, BOAT_Y, r, SH110X_WHITE);
      if (r > 4) display.drawCircle((int)boatX_, BOAT_Y, r - 3, SH110X_WHITE);
      for (int i = 0; i < 12; i++) display.drawPixel((int)boatX_ + random(-r, r + 1), BOAT_Y + random(-r, r + 1), SH110X_WHITE);
    } else {
      drawBoat((int)boatX_, BOAT_Y);
    }
  }

  int score() const override { return (int)(dist_ / 8.0f) + dodged_ * 5; }

private:
  struct Mine { float x, y; bool active; bool passed; };

  static constexpr int MAX_MINES = 8, RIPPLES = 10, BOAT_Y = 52;

  Mine     mines_[MAX_MINES];
  float    rx_[RIPPLES], ry_[RIPPLES];
  float    boatX_ = 64, dist_ = 0, elapsed_ = 0, spawnAcc_ = 0, scroll_ = 0;
  int      gap_ = 30, dodged_ = 0;
  uint32_t dyingMs_ = 0;

  void spawn(float x) {
    for (int i = 0; i < MAX_MINES; i++) {
      if (!mines_[i].active) {
        mines_[i].x = x;
        mines_[i].y = FIELD_Y - 6;
        mines_[i].active = true;
        mines_[i].passed = false;
        return;
      }
    }
  }


  bool hits(const Mine& m) const {
    float nx = m.x, ny = m.y;
    if (nx < boatX_ - 3) nx = boatX_ - 3;
    if (nx > boatX_ + 3) nx = boatX_ + 3;
    if (ny < BOAT_Y - 6) ny = BOAT_Y - 6;
    if (ny > BOAT_Y + 7) ny = BOAT_Y + 7;
    const float dx = m.x - nx, dy = m.y - ny;
    return dx * dx + dy * dy < 3.8f * 3.8f;
  }

  void drawBoat(int bx, int by) {
    display.fillTriangle(bx - 3, by - 2, bx + 3, by - 2, bx, by - 8, SH110X_WHITE);  
    display.fillRect(bx - 3, by - 2, 7, 10, SH110X_WHITE);                          
    display.fillRect(bx - 1, by + 1, 3, 3, SH110X_BLACK);                           
    if ((millis() / 120) % 2 == 0) {                                                   
      display.drawPixel(bx - 2, by + 9, SH110X_WHITE);
      display.drawPixel(bx + 2, by + 9, SH110X_WHITE);
    } else {
      display.drawPixel(bx - 1, by + 10, SH110X_WHITE);
      display.drawPixel(bx + 1, by + 10, SH110X_WHITE);
    }
  }


  void drawMine(int mx, int my) {
    display.fillCircle(mx, my, 3, SH110X_WHITE);
    for (int k = 4; k <= 5; k++) {
      display.drawPixel(mx, my - k, SH110X_WHITE);
      display.drawPixel(mx, my + k, SH110X_WHITE);
      display.drawPixel(mx - k, my, SH110X_WHITE);
      display.drawPixel(mx + k, my, SH110X_WHITE);
    }
    display.drawPixel(mx - 3, my - 3, SH110X_WHITE);
    display.drawPixel(mx + 3, my - 3, SH110X_WHITE);
    display.drawPixel(mx - 3, my + 3, SH110X_WHITE);
    display.drawPixel(mx + 3, my + 3, SH110X_WHITE);
    display.drawPixel(mx - 1, my - 1, SH110X_BLACK);                                  
  }
};

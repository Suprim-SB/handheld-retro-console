#pragma once
#include "gamebase.h"


//  shooter.h  -  ZOMBIE SHOOTER
//  You are a man with a gun at the bottom of the screen.
//  Zombies shamble down towards you.
//     LEFT / RIGHT  walk       A  shoot (hold A for automatic fire)
//  If a zombie reaches you, it is over.


static const NoteStep SFX_SHOT[]       = { {1500, 12}, {900, 10} };
static const NoteStep SFX_ZOMBIE_HIT[] = { {500, 20} };
static const NoteStep SFX_ZOMBIE_DIE[] = { {320, 35}, {230, 45}, {160, 60} };
static const NoteStep SFX_MAN_DIE[]    = { {600, 60}, {450, 60}, {300, 80}, {150, 220} };


// imagine this is leon kennedy (does not look like him but still)
static const char* const SPR_MAN[9] = {
  ".......",
  ".###...",
  ".###...",
  "..#..#.",
  ".#####.",
  ".##....",
  "..#....",
  ".#.#...",
  ".#.#..."
};
static const char* const SPR_ZOMBIE_A[9] = { //I tried to make zombies look like the clickers from last of us
  ".###.#.",
  "..####.",
  "###.##.",
  "..###..",
  "#..#..#",
  "#######",
  "...#...",
  ".#####.",
  ".#...#."
};
static const char* const SPR_ZOMBIE_B[9] = {
  ".#.....",
  ".##.#.#",
  "..####.",
  "...##..",
  "#..#..#",
  "#######",
  "...#...",
  ".#####.",
  ".#...#."
};

class ShooterGame : public Game {
public:
  void reset() override {
    manX_ = 60;
    fireCd_ = 0;
    spawnAcc_ = 600;
    kills_ = 0;
    score_ = 0;
    dyingMs_ = 0;
    animMs_ = 0;
    for (int i = 0; i < MAXZ; i++) z_[i].alive = false;
    for (int i = 0; i < MAXB; i++) b_[i].alive = false;
  }

  bool update(uint32_t dtMs) override {
    if (dyingMs_ > 0) {
      dyingMs_ = (dtMs >= dyingMs_) ? 0 : dyingMs_ - dtMs;
      return dyingMs_ == 0;
    }
    const float dt = dtMs / 1000.0f;
    animMs_ += dtMs;

  
    int dir = 0;
    if (input.held(BTN_LEFT))  dir -= 1;
    if (input.held(BTN_RIGHT)) dir += 1;
    manX_ += dir * 70.0f * dt;
    if (manX_ < 0) manX_ = 0;
    if (manX_ > SCREEN_W - 7) manX_ = SCREEN_W - 7;

    // shoot
    fireCd_ = (dtMs >= fireCd_) ? 0 : fireCd_ - dtMs;
    if (input.held(BTN_A) && fireCd_ == 0) {
      for (int i = 0; i < MAXB; i++) {
        if (!b_[i].alive) {
          b_[i].alive = true;
          b_[i].x = manX_ + MUZZLE_X;      
          b_[i].y = MAN_Y + MUZZLE_Y - 3;
          fireCd_ = 230;
          sound.play(SFX_SHOT, ARRAY_LEN(SFX_SHOT));
          break;
        }
      }
    }
    for (int i = 0; i < MAXB; i++) {
      if (!b_[i].alive) continue;
      b_[i].y -= 150.0f * dt;
      if (b_[i].y < FIELD_Y) b_[i].alive = false;
    }

  
    spawnAcc_ += dtMs;
    int interval = 1300 - kills_ * 14;
    if (interval < 380) interval = 380;
    if (spawnAcc_ >= (uint32_t)interval) {
      spawnAcc_ = 0;
      spawnZombie();
    }

  
    for (int i = 0; i < MAXZ; i++) {
      Zombie& z = z_[i];
      if (!z.alive) continue;
      z.y += z.sp * dt;
      const float tx = manX_ - z.x;
      z.x += (tx > 2 ? 5.0f : (tx < -2 ? -5.0f : 0.0f)) * dt;
      if (z.flash > 0) z.flash = (dtMs >= z.flash) ? 0 : z.flash - dtMs;

      for (int k = 0; k < MAXB; k++) {
        if (!b_[k].alive) continue;
        if (b_[k].x >= z.x && b_[k].x < z.x + 7 && b_[k].y + 3 >= z.y && b_[k].y <= z.y + 9) {
          b_[k].alive = false;
          if (--z.hp <= 0) {
            z.alive = false;
            kills_++;
            score_ += z.tank ? 20 : 10;
            sound.play(SFX_ZOMBIE_DIE, ARRAY_LEN(SFX_ZOMBIE_DIE));
          } else {
            z.flash = 90;
            sound.play(SFX_ZOMBIE_HIT, ARRAY_LEN(SFX_ZOMBIE_HIT));
          }
          break;
        }
      }
      if (z.alive && z.y + 9 >= MAN_Y + 2) {
        dyingMs_ = 650;
        sound.play(SFX_MAN_DIE, ARRAY_LEN(SFX_MAN_DIE));
        break;
      }
    }
    return false;
  }

  void draw() override {
    display.drawFastHLine(0, SCREEN_H - 1, SCREEN_W, SH110X_WHITE);

    const bool frameB = (animMs_ / 280) % 2;
    for (int i = 0; i < MAXZ; i++) {
      const Zombie& z = z_[i];
      if (!z.alive || z.flash > 0) continue;
      const char* const* pic = frameB ? SPR_ZOMBIE_B : SPR_ZOMBIE_A;
      drawSprite((int)z.x, (int)z.y, pic, 9);
    
      if (z.tank) drawSprite((int)z.x + 1, (int)z.y, pic, 9);
    }
    for (int i = 0; i < MAXB; i++) {
      if (b_[i].alive) display.fillRect((int)b_[i].x, (int)b_[i].y, 1, 3, SH110X_WHITE);
    }
    if (dyingMs_ == 0 || (dyingMs_ / 80) % 2 == 0) {
      drawSprite((int)manX_, MAN_Y, SPR_MAN, 9);
    }
  }

  int score() const override { return score_; }

private:
  struct Zombie { float x, y, sp; int8_t hp; bool alive; bool tank; uint32_t flash; };
  struct Bullet { float x, y; bool alive; };

  static constexpr int MAXZ = 14, MAXB = 5, MAN_Y = 54;
  static constexpr int MUZZLE_X = 5, MUZZLE_Y = 3;

  Zombie   z_[MAXZ];
  Bullet   b_[MAXB];
  float    manX_ = 60;
  uint32_t fireCd_ = 0, spawnAcc_ = 0, dyingMs_ = 0, animMs_ = 0;
  int      kills_ = 0, score_ = 0;

  void spawnZombie() {
    for (int i = 0; i < MAXZ; i++) {
      if (z_[i].alive) continue;
      float sp = 9.0f + kills_ * 0.3f;
      if (sp > 26.0f) sp = 26.0f;
      int tankChance = kills_ / 2;
      if (tankChance > 35) tankChance = 35;
      z_[i].x = random(0, 121);
      z_[i].y = FIELD_Y;
      z_[i].sp = sp + random(0, 70) / 10.0f;
      z_[i].tank = (random(0, 100) < tankChance);
      z_[i].hp = z_[i].tank ? 2 : 1;
      z_[i].flash = 0;
      z_[i].alive = true;
      return;
    }
  }
};
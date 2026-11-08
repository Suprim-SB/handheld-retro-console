#pragma once
#include "gfx.h"

//  icons.h  -  game names and their icons

//  The icons are made with code (rectangles and circles) instead of bitmaps
//  To change an icon, edit its 'case' below. Each icon lives on a
//  5 x 5 grid of cells; cell(x, y) fills one cell.

static const char* const GAME_NAMES[GAME_COUNT] = {
  "SNEK", "DEMOLISH", "BOAT RACE", "EVIL RESIDENT", "PONGGG"
};

static void drawIcon(uint8_t game, int cx, int cy, int s, uint16_t fg, uint16_t bg) {
  int u = s / 5;                    
  if (u < 2) u = 2;
  const int w  = 5 * u;
  const int ox = cx - w / 2;
  const int oy = cy - w / 2;
  const int cs = (u >= 3) ? (u - 1) : u;   

  auto cell = [&](int gx, int gy) {
    display.fillRect(ox + gx * u, oy + gy * u, cs, cs, fg);
  };

  switch (game) {
    case 0: {   
      const int body[][2] = { {0,3},{1,3},{2,3},{2,2},{2,1},{3,1},{4,1} };
      for (auto& c : body) cell(c[0], c[1]);
      cell(4, 3);                                
      break;
    }
    case 1: {   
      const int R = w / 2;
      display.fillCircle(cx, cy, R, fg);
      display.fillTriangle(cx, cy, cx + R + 1, cy - (R * 7) / 10,
                                   cx + R + 1, cy + (R * 7) / 10, bg);
      display.fillRect(cx + (R * 6) / 10, cy - 1, 2, 2, fg);   
      display.drawPixel(cx - 1, cy - R / 2, bg);              
      break;
    }
    case 2: {  
      const int boat[][2] = { {2,0},{1,1},{2,1},{3,1},{1,2},{2,2},{3,2},{1,3},{2,3},{3,3} };
      for (auto& c : boat) cell(c[0], c[1]);
      cell(0, 2); cell(4, 3); cell(0, 4); cell(4, 4);         
      break;
    }
    case 3: {  
      const int zom[][2] = { {1,0},{2,0},{3,0},
                             {0,1},{1,1},{2,1},{3,1},{4,1},
                             {2,2},{2,3},{1,4},{3,4} };
      for (auto& c : zom) cell(c[0], c[1]);
      break;
    }
    case 4: {  
      for (int y = 1; y <= 3; y++) cell(0, y);
      for (int y = 0; y <= 2; y++) cell(4, y);
      cell(2, 2);
      break;
    }
  }
}

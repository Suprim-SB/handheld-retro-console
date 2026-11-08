//  Keep all the files in the downoald together in a folder named "Handheld"

#include "config.h"
#include "gfx.h"
#include "input.h"
#include "sound.h"
#include "hwtest.h"
#include "boot.h"
#include "icons.h"
#include "menu.h"
#include "games.h"

bool displayOK = false;
bool hwMode    = false;

enum AppState : uint8_t { APP_MENU, APP_GAME };
AppState app         = APP_MENU;
int      currentGame = 0;

static void scanI2C() {
  LOGLN("Scanning I2C bus...");
  uint8_t found = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      LOGF("  device found at 0x%02X\n", addr);
      found++;
    }
  }
  if (found == 0) LOGLN("  nothing found - check SDA/SCL wiring and power");
}


void setup() {
#if DEBUG_SERIAL
  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && millis() - t0 < 800) { delay(10); }   
#endif
  LOGLN("\n=== Handheld ===");

  input.begin();
  sound.begin();

  Wire.begin(Pin::Sda, Pin::Scl);   
  Wire.setClock(I2C_HZ);
  scanI2C();

  displayOK = display.begin(OLED_ADDR, true);
  if (!displayOK) {
    LOGLN("Display NOT found at 0x3C - check wiring. (3 low beeps)");
    sound.play(SFX_ERROR, ARRAY_LEN(SFX_ERROR));
    return;
  }
  LOGLN("Display OK");
  display.clearDisplay();
  display.display();
  highScores.begin();   


  for (int i = 0; i < 3; i++) { input.update(); delay(15); }
  if (input.held(BTN_B)) {
    hwMode = true;
    LOGLN("Hardware test mode");
    return;
  }

  runBoot();            
  menu.begin(true);    
}

void loop() {
  if (!displayOK) { delay(100); return; }  

  input.update();    

  if (hwMode) {
    hwtestFrame();
    return;
  }

  if (app == APP_MENU) {
    const int g = menu.frame();
    if (g >= 0) {
      currentGame = g;
      app = APP_GAME;
      runner.begin(g);
      LOGF("Starting game %d (%s)\n", g, GAME_NAMES[g]);
    }
  } else {
    if (runner.frame()) {          
      app = APP_MENU;
      menu.begin(false);           
    }
  }
}

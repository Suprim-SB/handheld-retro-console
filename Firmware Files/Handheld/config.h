#pragma once
#include <Arduino.h>

//  If you ever need move a wire on the PCB, this is the only file you have to edit

// ---------- Debug output ----------
// 1 = print to the Serial Monitor (needs the USB cable plugged in)
// 0 = no serial output at all (use this for battery-powered runs)
#define DEBUG_SERIAL 1

#if DEBUG_SERIAL
  #define LOG(x)    Serial.print(x)
  #define LOGLN(x)  Serial.println(x)
  #define LOGF(...) Serial.printf(__VA_ARGS__)
#else
  #define LOG(x)    do {} while (0)
  #define LOGLN(x)  do {} while (0)
  #define LOGF(...) do {} while (0)
#endif

#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

// Pin map (matches the KiCad schematic)
namespace Pin {
  constexpr uint8_t Up     = 0;    // BTN_UP
  constexpr uint8_t Down   = 1;    // BTN_DOWN
  constexpr uint8_t Left   = 3;    // BTN_LEFT
  constexpr uint8_t Right  = 4;    // BTN_RIGHT
  constexpr uint8_t BtnA   = 5;    // BTN_A
  constexpr uint8_t BtnB   = 10;   // BTN_B

  constexpr uint8_t Sda    = 6;    // I2C_SDA 
  constexpr uint8_t Scl    = 7;    // I2C_SCL  

  constexpr uint8_t Buzzer = 21;   // BUZZER (through the 220 ohm resistor)
}

// Display
constexpr uint8_t  SCREEN_W  = 128;
constexpr uint8_t  SCREEN_H  = 64;
constexpr uint8_t  OLED_ADDR = 0x3C;     
constexpr uint32_t I2C_HZ    = 400000;   

// ---------- Buttons ----------
constexpr uint8_t  DEBOUNCE_MS  = 12;   
constexpr uint16_t EXIT_HOLD_MS = 5000;  // hold B this long inside a game to go back to the menu (I put 5 seconds since that should be good enough)

// ---------- Games ----------
constexpr uint8_t GAME_COUNT = 5;

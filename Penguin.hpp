#ifndef Penguin_h
#define Penguin_h

#include "Arduino.h"
#include <LovyanGFX.hpp>

#define DEPTH_LEVELS 10
#define PENGUIN_BASE_Y lcd.height() * 0.8
#define MAX_HORIZONTAL_MOVE 2
#define JUMP_UP 2
#define SHIFT_RANGE 1.6
#define SHIFT_SPEED 0.05
#define DISABLED_SHIFT -666
#define MAX_ENERGY 1000

class Penguin{
  public:
    void setup();
    void loop();
  private:
    int rotaryCount;
    int timeCount;
    int points;
    bool gameIsOver;
    float shift;
    float logPosA;
    float logPosAShift;
    float logPosB;
    float logPosBShift;
    float fishPos;
    float fishPosShift;
    int penguinFlip;
    int penguinX;
    int penguinY;
    float jump;
    bool jumping;
    int horizontalMove;
    int energy = MAX_ENERGY;
    bool hit;

    void move();
    void paint();
    void reset();
};
#endif

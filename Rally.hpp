#ifndef Rally_h
#define Rally_h

#include "Arduino.h"
#include <LovyanGFX.hpp>

#define MAX_TILES 4
#define MAX_SPEED 12
#define ACCELERATION 0.3
#define ROAD_ZOOM 1.8
#define CAR_ZOOM 0.7
#define MAX_FUEL 1000
#define STAGE_TILES 64
#define OFFROAD_COLOR 0x001100

class Rally{
  public:
    void setup();
    void loop();
  private:
    struct TILE{
      byte tileId;
      float relativeX;
      float relativeY;
      float pivotX;
      float pivotY;
      float angle;
    };
    int rotaryCount;
    int timeCount;
    int points;
    float carSpeed = 1;
    float steerAngle;
    float directionAngle;
    bool gameIsOver;
    float terrainX;
    float terrainY;
    float terrainXPrev;
    float terrainYPrev;
    int first = 0;//er these represent the different onscreen tiles... an array would be nicer and tidier (TODO?)
    int second = 1;
    int third = 2;
    int fourth = 3;
    int fuel = MAX_FUEL;
    int tilesPassed = 0;
    RGBColor roadColor;
    TILE tiles[MAX_TILES];
    //bool night;

    void move();
    void paint();
    void reset();
};
#endif

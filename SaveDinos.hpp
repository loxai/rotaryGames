#ifndef SaveDino_h
#define SaveDino_h

#include "Arduino.h"
#include <LovyanGFX.hpp>

#define MAX_METEORS 10

class SaveDinos{
  public:
    void setup();
    void loop();
  private:
    struct METEOR_DATA{
      bool active;
      float x;
      float y;
      float speed;
      float rotation;
      float rotationSpeed;
      float zoom;
    };

    METEOR_DATA meteors[MAX_METEORS];
    int rotaryCount;
    int timeCount;
    int orbitDistance;
    int centerX;
    int centerY;
    int ufoX;
    int ufoY;
    int earthShift;
    float earthLife = 10;
    int points;
    float zoom = 0.6;
  	bool gameIsOver;

    void move();
    void paint();
    void reset();
};
#endif

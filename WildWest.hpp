#ifndef WildWest_h
#define WildWest_h

#include "Arduino.h"
#include <LovyanGFX.hpp>

#define NUM_WINDOWS 7
#define MAX_BULLETS 5
#define ENEMY_SHOW_SPEED 0.1
#define ENEMY_BULLET_SPEED 0.01
#define ENEMY_INACCURACY 30
#define PLAYER_BULLET_SPEED 3

class WildWest{
  public:
    void setup();
    void loop();
  private:
    int rotaryCount;
    int timeCount;
    int points;
    bool gameIsOver;
    struct WINDOW{
      int x;
      int y;
      bool active;
      float currentZoomX;
      float currentZoomY;
      float zoomXInc;
      float zoomYInc;
    };
    struct BULLET{
      float x;
      float y;
      float incX;
      float incY;
      bool active;
    };
    WINDOW windows[NUM_WINDOWS];
    BULLET enemyBullets[MAX_BULLETS];
    BULLET playerBullets[MAX_BULLETS];
    int playerX;
    int playerY;
    float playerAngle;
    
    void move();
    void paint();
    void reset();
};
#endif

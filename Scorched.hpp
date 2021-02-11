#ifndef Scorched_h
#define Scorched_h

#include "Arduino.h"
#include "Utils.hpp"
#include <LovyanGFX.hpp>

#define BLAST_RADIUS 9
#define CANNON_SIZE 10
#define MAX_STEPS 15
#define MAX_POWER 15
#define MAX_ENERGY 3
#define GRAVITY 0.4
#define GRAVITY_ACC 1.4
#define POWER_DECREASE 0.99

class Scorched{
  public:
    void setup();
    void loop();
  private:
    int rotaryCount;
    int timeCount;
    bool gameIsOver;

    bool soil[WIDTH][HEIGHT];
    enum STATE{p1_move, p1_point, p1_power, p1_shoot, p2_move, p2_point, p2_power, p2_shoot};
    STATE currentState;
    //float power;
    float gravityPower;
    int p1X;
    int p1Y;
    int p1Energy;
    int p1Steps;
    float p1Angle;
    float p1Power;
    int p2X;
    int p2Y;
    int p2Energy;
    int p2Steps;
    float p2Angle;
    float p2Power;
    int bulletX;
    int bulletY;

    void powerSelect(float& power, int& x, int& y);
    void aim(float& angle, int minAngle, int maxAngle);
    void moveTank(int& steps, int& x, int& y);
    bool shooting(float& power, float& angle, bool againstP1);
    bool checkBulletHit(int x, int y, bool againstP1);
    void moveTank(int& x, int& y);
    void blastSoil(int x, int y);
    void fallSoil(int x, int y);
    void move();
    void paint();
    void reset();
};
#endif

#ifndef SpaceClash_h
#define SpaceClash_h

#include "Arduino.h"
#include <LovyanGFX.hpp>

#define MAX_SHIPS 100
#define SHIP_TYPES 6
#define NUM_LANES 3
#define ALPHA 0x00ff00U
#define SELECTION_ZOOM 0.6
#define PLAY_ZOOM 0.4
#define DEFAULT_ROTATION 270
#define PLAYER_HEALTH 1000
#define PLAYER_BUDGET 150
#define PLAYER_BUDGET_INCREASE 30
#define ACTION_MAX_COUNT 10
#define ATTACK_RANGE 0.3

class SpaceClash{
  public:
    void setup();
    void loop();
  private:
    int rotaryCount;
    int timeCount;
    int points;
    bool gameIsOver;

    enum STATE {p1_intro, p1_selectShip, p1_selectLane, p2_intro, p2_selectShip, p2_selectLane, action_intro, action};
    struct SHIP_TYPE{
      String name;
      LGFX_Sprite *sprite;
      float speed;
      int health;
      int damage;
      int cost;
      int special;//0 normal, 1 only targets base, 2 torpedo
    };
    struct SHIP{
      bool active;
      bool player1;
      float shoot;
      int lane;
      float position;
      int health;
      SHIP_TYPE type;
    };

    SHIP_TYPE types[SHIP_TYPES];//={{"None",0,0,0,0,0,-1}};
    SHIP ships[MAX_SHIPS];
    STATE state;

    int selectedShip;
    int selectedLane;
    int motherShipRotation;
    int actionCount;

    int p1Budget;
    int p1Health;
    int p2Budget;
    int p2Health;


    void intro(int& budget);
    void selectShip(int& budget);
    void selectLane(bool isPlayer1);
    void move();
    void paint();
    void reset();
};
#endif

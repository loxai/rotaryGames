
#include "SpaceClash.hpp"
#include "Utils.hpp"

#include "space.bmp.h"
#include "disabled.bmp.h"
#include "fighterShip.bmp.h"
#include "destroyerShip.bmp.h"
#include "portalShip.bmp.h"
#include "battleShip.bmp.h"
#include "torpedo.bmp.h"
#include "motherShip.bmp.h"

static LGFX lcd;
static LGFX_Sprite frameBuffer;
static LGFX_Sprite spaceSprite;
static LGFX_Sprite noneSprite;
static LGFX_Sprite fighterSprite;
static LGFX_Sprite destroyerSprite;
static LGFX_Sprite tankSprite;
static LGFX_Sprite battlerSprite;
static LGFX_Sprite photonSprite;
static LGFX_Sprite motherShipSprite;

void SpaceClash::setup()
{
  lcd.init();
  lcd.setRotation(3);
  lcd.fillScreen(0xFFFFFFU);//doesn't work without this... need to set black as transparent, hmmm?

  frameBuffer.createSprite(lcd.width(), lcd.height());
  Utils::initSprite(&spaceSprite, width_space_bmp, height_space_bmp, palette_space_bmp, image_space_bmp);
  Utils::initSprite(&noneSprite, width_disabled_bmp, height_disabled_bmp, palette_disabled_bmp, image_disabled_bmp);
  Utils::initSprite(&fighterSprite, width_fighterShip_bmp, height_fighterShip_bmp, palette_fighterShip_bmp, image_fighterShip_bmp);
  Utils::initSprite(&destroyerSprite, width_destroyerShip_bmp, height_destroyerShip_bmp, palette_destroyerShip_bmp, image_destroyerShip_bmp);
  Utils::initSprite(&tankSprite, width_portalShip_bmp, height_portalShip_bmp, palette_portalShip_bmp, image_portalShip_bmp);
  Utils::initSprite(&battlerSprite, width_battleShip_bmp, height_battleShip_bmp, palette_battleShip_bmp, image_battleShip_bmp);
  Utils::initSprite(&photonSprite, width_torpedo_bmp, height_torpedo_bmp, palette_torpedo_bmp, image_torpedo_bmp);
  Utils::initSprite(&motherShipSprite, width_motherShip_bmp, height_motherShip_bmp, palette_motherShip_bmp, image_motherShip_bmp);

  noneSprite.setPivot(width_disabled_bmp / 2, height_disabled_bmp / 2);
  fighterSprite.setPivot(width_fighterShip_bmp / 2, height_fighterShip_bmp / 2);
  destroyerSprite.setPivot(width_destroyerShip_bmp / 2, height_destroyerShip_bmp / 2);
  tankSprite.setPivot(width_portalShip_bmp / 2, height_portalShip_bmp / 2);
  battlerSprite.setPivot(width_battleShip_bmp / 2, height_battleShip_bmp / 2);
  photonSprite.setPivot(width_torpedo_bmp / 2, height_torpedo_bmp / 2);
  motherShipSprite.setPivot(width_motherShip_bmp / 2, height_motherShip_bmp / 2);

  reset();
}
void SpaceClash::loop() {
  rotaryCount += Utils::rotaryInput();

  if (millis() - timeCount > 32){
    if (gameIsOver){
      if (Utils::gameOver(lcd, frameBuffer, p2Health <= 0))
        reset();
    } else{
      move();
      paint();
    }
    timeCount = millis();
  }
}
void SpaceClash::move(){
  switch(state){
    case p1_intro:
      intro(p1Budget);
    break;
    case p1_selectShip:
      selectShip(p1Budget);
    break;
    case p1_selectLane:
      selectLane(true);
    break;
    case p2_intro:
      intro(p2Budget);
    break;
    case p2_selectShip:
      selectShip(p2Budget);
    break;
    case p2_selectLane:
      selectLane(false);
    break;
    case action_intro:
      if (Utils::releaseA()){
        state = static_cast<STATE>(state + 1);
      }
    break;
    case action:
      for(int i = 0; i < MAX_SHIPS; i++){
        if (ships[i].active){
          ships[i].shoot = 0;
          //check against enemy ships
          for(int n = 0; n < MAX_SHIPS; n++){
            if (ships[n].active){
              if (ships[i].type.special == 0){//if enemy in range, shoot instead of move
                if (ships[i].player1 != ships[n].player1 && ships[i].lane == ships[n].lane && abs(ships[i].position - (1 - ships[n].position)) < ATTACK_RANGE){
                  ships[i].shoot = ships[i].position - (1 - ships[n].position);
                  ships[n].health -= ships[i].type.damage;
                  if (ships[n].health < 0)
                    ships[n].active = false;//TODO destroy animation
                  break;
                }
              }
            }
          }
          if (ships[i].shoot == 0){//if ship didn't shoot enemy, check near mothership
            if (ships[i].position >= 1){
              if (ships[i].type.special == 2)//torpedo, gets destroyed
                ships[i].active = false;
              ships[i].shoot = -0.2;
              if (ships[i].player1)
                p2Health -= ships[i].type.damage;
              else
                p1Health -= ships[i].type.damage;
            }
          }
          if (!ships[i].shoot){//not shooting? then move
            ships[i].position += ships[i].type.speed / ACTION_MAX_COUNT;
            if (ships[i].position > 1)
              ships[i].position = 1;
          }
        }
      }
      if (p1Health <= 0 || p2Health <= 0){
        gameIsOver = true;
      }
      
      actionCount++;
      if (actionCount >= ACTION_MAX_COUNT){
        actionCount = 0;
        state = p1_intro;
      }
    break;
  }
}
void SpaceClash::paint(){
  spaceSprite.pushSprite(&frameBuffer, 0, 0);

  motherShipRotation++;
  motherShipRotation%=360;
  motherShipSprite.pushRotateZoom(&frameBuffer, 0, HEIGHT / 2, motherShipRotation, 0.8, 0.8, ALPHA);
  motherShipSprite.pushRotateZoom(&frameBuffer, WIDTH, HEIGHT / 2, 360 - motherShipRotation, 0.8, 0.8, ALPHA);

  for(int i = 0; i < MAX_SHIPS; i++){
    if (ships[i].active){
      int x = ships[i].player1? WIDTH * 0.9 - ships[i].position * WIDTH * 0.8 : WIDTH * 0.1 + ships[i].position * WIDTH * 0.8;
      int y = (ships[i].lane * 0.3 + 0.2) * HEIGHT;
      int rotation = ships[i].player1? DEFAULT_ROTATION : 360 - DEFAULT_ROTATION;
      ships[i].type.sprite->pushRotateZoom(&frameBuffer, x, y, rotation, PLAY_ZOOM, PLAY_ZOOM, ALPHA);
      if (ships[i].shoot != 0){
        //ships[i].shoot = 0;
        int tx = x - (ships[i].shoot * WIDTH * 0.4) * (ships[i].player1? -1 : 1) + random(6) - 3;
        int ty = y + random(6) - 3;
        frameBuffer.drawLine(x, y, tx, ty, ships[i].player1? lcd.color888(255,0,0) : lcd.color888(0,0,255));
      }
    }
  }

  frameBuffer.fillRect(WIDTH - 3, 0, WIDTH, HEIGHT * (float)p1Health / PLAYER_HEALTH, lcd.color888(0,255,0));
  frameBuffer.fillRect(0, 0, 3, HEIGHT * (float)p2Health / PLAYER_HEALTH, lcd.color888(0,255,0));

  switch(state){
    case p1_intro:
      frameBuffer.setCursor(20, HEIGHT * 0.01);
      frameBuffer.setFont(&fonts::Font2);
      frameBuffer.setTextColor(0xAAAAFFU);
      frameBuffer.printf("PLAYER 1 PRESS A");
    break;
    case p1_selectShip:
      frameBuffer.setCursor(WIDTH / 2.5, HEIGHT * 0.01);
      frameBuffer.setFont(&fonts::Font2);
      frameBuffer.setTextColor(0xAAAAFFU);
      frameBuffer.printf("%s", types[selectedShip].name);

      frameBuffer.fillArc(WIDTH / 2, HEIGHT / 2, 15, 1, 0, 180 * (float)types[selectedShip].cost/PLAYER_BUDGET, lcd.color888(255,0,0));
      frameBuffer.fillArc(WIDTH, HEIGHT / 2, 15, 1, 90, 90 + 180 * (float)p1Budget/PLAYER_BUDGET, lcd.color888(255,0,0));

      types[selectedShip].sprite->pushRotateZoom(&frameBuffer, WIDTH / 2, HEIGHT * 0.3, DEFAULT_ROTATION, SELECTION_ZOOM, SELECTION_ZOOM, ALPHA);
    break;
    case p1_selectLane:
      //int y = (selectedLane * 0.3 + 0.2) * HEIGHT;
      types[selectedShip].sprite->pushRotateZoom(&frameBuffer, WIDTH * 0.85, (selectedLane * 0.3 + 0.2) * HEIGHT, DEFAULT_ROTATION, SELECTION_ZOOM, SELECTION_ZOOM, ALPHA);
      //frameBuffer.fillCircle(WIDTH * 0.85, y, 5, lcd.color888(255,255,255));
    break;
    case p2_intro:
      frameBuffer.setCursor(20, HEIGHT * 0.01);
      frameBuffer.setFont(&fonts::Font2);
      frameBuffer.setTextColor(0xAAAAFFU);
      frameBuffer.printf("PLAYER 2 PRESS A");
    break;
    case p2_selectShip:
      frameBuffer.setCursor(WIDTH / 2.5, HEIGHT * 0.01);
      frameBuffer.setFont(&fonts::Font2);
      frameBuffer.setTextColor(0xAAAAFFU);
      frameBuffer.printf("%s", types[selectedShip].name);

      frameBuffer.fillArc(WIDTH / 2, HEIGHT / 2, 15, 1, 0, 180 * (float)types[selectedShip].cost/PLAYER_BUDGET, lcd.color888(255,0,0));
      frameBuffer.fillArc(0, HEIGHT / 2, 15, 1, 270, 270 + 180 * (float)p2Budget/PLAYER_BUDGET, lcd.color888(0,0,255));

      types[selectedShip].sprite->pushRotateZoom(&frameBuffer, WIDTH / 2, HEIGHT * 0.3, 360 - DEFAULT_ROTATION, SELECTION_ZOOM, SELECTION_ZOOM, ALPHA);
    break;
    case p2_selectLane:
      types[selectedShip].sprite->pushRotateZoom(&frameBuffer, WIDTH * 0.15, (selectedLane * 0.3 + 0.2) * HEIGHT, 360 - DEFAULT_ROTATION, SELECTION_ZOOM, SELECTION_ZOOM, ALPHA);
    break;
    case action_intro:
      frameBuffer.setCursor(20, HEIGHT * 0.01);
      frameBuffer.setFont(&fonts::Font2);
      frameBuffer.setTextColor(0xAAAAFFU);
      frameBuffer.printf("PRESS A TO START");
    break;
    case action:
    break;
  }

  lcd.pushPixelsDMA(frameBuffer.getBuffer(), lcd.width() * lcd.height());
}
void SpaceClash::reset(){
  rotaryCount = 0;
  timeCount = 0;
  points = 0;
  gameIsOver = false;

  state = p1_intro;
  //completely unbalanced ship stats (TODO)
  types[0] = {"None",&noneSprite,0,0,0,0,-1};
  types[1] = {"Fighter",&fighterSprite,0.15,10,4,5,0};
  types[2] = {"Destroyer",&destroyerSprite,0.08,100,5,100,1};
  types[3] = {"Tank",&tankSprite,0.08,300,1,80,0};
  types[4] = {"Battleship",&battlerSprite,0.1,50,4,60,0};
  types[5] = {"Photon",&photonSprite,0.3,1,100,50,2};
  //name,sprite,speed,health,damage,cost,special;//0 normal, 1 only targets base, 2 torpedo

  p1Budget = PLAYER_BUDGET;
  p1Health = PLAYER_HEALTH;
  p2Budget = PLAYER_BUDGET;
  p2Health = PLAYER_HEALTH;

  selectedShip = 0;
  selectedLane = 0;
  actionCount = 0;

}
void SpaceClash::intro(int& budget){
  if (Utils::releaseA()){
    selectedLane = 0;
    selectedShip = 0;
    budget += PLAYER_BUDGET_INCREASE;
    if (budget > PLAYER_BUDGET)
      budget = PLAYER_BUDGET;
    state = static_cast<STATE>(state + 1);
  }
}
void SpaceClash::selectShip(int& budget){
  selectedShip += Utils::sign(rotaryCount);
  if (selectedShip < 0)
    selectedShip = SHIP_TYPES - 1;
  if (selectedShip >= SHIP_TYPES)
    selectedShip = 0;
  while(types[selectedShip].cost > budget){
    if (rotaryCount != 0)
      selectedShip += Utils::sign(rotaryCount);
    else
      selectedShip = 0;
    if (selectedShip < 0)
      selectedShip = SHIP_TYPES - 1;
    if (selectedShip >= SHIP_TYPES)
      selectedShip = 0;
  }

  if (Utils::releaseA())
    if (selectedShip == 0)
      state = static_cast<STATE>(state + 2);
      //state = action;
    else{
      int cost = types[selectedShip].cost;
      if (budget - cost >= 0){
        budget -= cost;
        state = static_cast<STATE>(state + 1);
      }
    }
  rotaryCount = 0;
}
void SpaceClash::selectLane(bool isPlayer1){
  selectedLane += Utils::sign(rotaryCount);
  rotaryCount = 0;
  if (selectedLane < 0)
    selectedLane = NUM_LANES - 1;
  if (selectedLane >= NUM_LANES)
    selectedLane = 0;
  if (Utils::releaseA()){
    for(int i = 0; i < MAX_SHIPS; i++){
      if (!ships[i].active){
        ships[i].type = types[selectedShip];
        ships[i].active = true;
        ships[i].player1 = isPlayer1;
        ships[i].lane = selectedLane;
        ships[i].position = 0;
        ships[i].health = ships[i].type.health;
        break;
      }
    }
    state = static_cast<STATE>(state - 1);
    //state = p1_selectShip;
  }
}

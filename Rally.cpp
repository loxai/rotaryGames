
#include "Rally.hpp"
#include "Utils.hpp"

#include "car.bmp.h"
//#include "headlights.bmp.h"//night test not looking nice
#include "sandTile1.bmp.h"
#include "sandTile2.bmp.h"
#include "sandTile3.bmp.h"

#define ROTATE_SPEED 6

static LGFX lcd;
static LGFX_Sprite carSprite;
static LGFX_Sprite headlightsSprite;
static LGFX_Sprite tileSprites[3];
static LGFX_Sprite frameBuffer;


void Rally::setup()
{
  lcd.init();
  lcd.setRotation(3);
  lcd.fillScreen(0xFFFFFFU);//doesn't work without this... need to set black as transparent, hmmm?

  frameBuffer.createSprite(lcd.width(), lcd.height());
  
  Utils::initSprite(&carSprite, width_car_bmp, height_car_bmp, palette_car_bmp, image_car_bmp);
  Utils::initSprite(&tileSprites[0], width_sandTile1_bmp, height_sandTile1_bmp, palette_sandTile1_bmp, image_sandTile1_bmp);
  Utils::initSprite(&tileSprites[1], width_sandTile1_bmp, height_sandTile1_bmp, palette_sandTile2_bmp, image_sandTile2_bmp);
  Utils::initSprite(&tileSprites[2], width_sandTile1_bmp, height_sandTile1_bmp, palette_sandTile3_bmp, image_sandTile3_bmp);
  //Utils::initSprite(&headlightsSprite, width_headlights_bmp, height_headlights_bmp, palette_headlights_bmp, image_headlights_bmp);

  carSprite.setPivot(width_car_bmp / 2, height_car_bmp * 0.1);
  //headlightsSprite.setPivot(width_headlights_bmp / 2, height_headlights_bmp * 0.77);

  reset();
}
void Rally::loop() {
  rotaryCount += Utils::rotaryInput() * ROTATE_SPEED;
  rotaryCount %= 360;

  if (millis() - timeCount > 32){
    if (gameIsOver){
      if (Utils::gameOver(lcd, frameBuffer, points))
        reset();
    } else{
      move();
      paint();
    }
    timeCount = millis();
  }
}

void Rally::move(){
  carSpeed+= ACCELERATION;
  if (carSpeed >= MAX_SPEED)
    carSpeed = MAX_SPEED;

  //only dark (tarmac) color allows high speed
  if (roadColor.r + roadColor.g + roadColor.b >= 300){
    carSpeed *= 0.8;
    if (carSpeed < 1)
      carSpeed = 1;
  }

  //TODO not the most effective way to avoid offroading. also doing roadColor.b == 0xAA and setting frameBuffer.fillScreen(0x____AAU) doesn't work
  if (roadColor.r + roadColor.g + roadColor.b >= 750){
    carSpeed = -carSpeed * 2;
    terrainX = terrainXPrev;
    terrainY = terrainYPrev;
    fuel--;
  }
  fuel--;
  if (fuel <= 0){
    gameIsOver = true;    
  }
  steerAngle += rotaryCount * (max((float)MAX_SPEED / 2,carSpeed) / MAX_SPEED);
  rotaryCount = 0;
  float driftFactor = 1 - (carSpeed / MAX_SPEED);
  directionAngle = directionAngle * driftFactor + steerAngle * (1 - driftFactor);//directionAngle *0.97 + steerAngle * 0.03;

  terrainXPrev = terrainX;
  terrainYPrev = terrainY;
  
  terrainX += cos(directionAngle) * carSpeed;
  terrainY += sin(directionAngle) * carSpeed;

  tiles[first].relativeX += cos(directionAngle * DEG_TO_RAD) * carSpeed;
  tiles[first].relativeY += sin(directionAngle * DEG_TO_RAD) * carSpeed;// -lcd.height()/2;
  tiles[second].relativeX = tiles[first].relativeX + tiles[second].pivotX;
  tiles[second].relativeY = tiles[first].relativeY + tiles[second].pivotY;
  tiles[third].relativeX = tiles[second].relativeX + tiles[third].pivotX;
  tiles[third].relativeY = tiles[second].relativeY + tiles[third].pivotY;
  tiles[fourth].relativeX = tiles[third].relativeX + tiles[fourth].pivotX;
  tiles[fourth].relativeY = tiles[third].relativeY + tiles[fourth].pivotY;

  if (tiles[first].relativeX > lcd.width() + width_sandTile1_bmp * ROAD_ZOOM / 1.5){
    tilesPassed++;
    points += 10;
    if (tilesPassed == STAGE_TILES){
      fuel = MAX_FUEL;
      points += 100;
      tilesPassed = 0;
      //night = !night;
    }

    tiles[first].tileId = random(3);
    tiles[first].angle = random(100) - 50;
    tiles[first].pivotX = -(width_sandTile1_bmp * ROAD_ZOOM / 2) * cos(tiles[fourth].angle * DEG_TO_RAD);
    tiles[first].pivotY = -(height_sandTile1_bmp * ROAD_ZOOM / 2) * sin(tiles[fourth].angle * DEG_TO_RAD);
    first++;
    if (first == MAX_TILES)
      first = 0;
    second++;
    if (second == MAX_TILES)
      second = 0;
    third++;
    if (third == MAX_TILES)
      third = 0;
    fourth++;
    if (fourth == MAX_TILES)
      fourth = 0;
  }
}
void Rally::paint(){
  frameBuffer.fillScreen(0xFFFFFFU);

  tileSprites[tiles[fourth].tileId].pushRotateZoom(&frameBuffer, tiles[fourth].relativeX, tiles[fourth].relativeY, tiles[fourth].angle, ROAD_ZOOM, ROAD_ZOOM, ALPHA_sandTile1_bmp);
  tileSprites[tiles[third].tileId].pushRotateZoom(&frameBuffer, tiles[third].relativeX, tiles[third].relativeY, tiles[third].angle,ROAD_ZOOM, ROAD_ZOOM, ALPHA_sandTile1_bmp);
  tileSprites[tiles[second].tileId].pushRotateZoom(&frameBuffer, tiles[second].relativeX, tiles[second].relativeY, tiles[second].angle,ROAD_ZOOM, ROAD_ZOOM, ALPHA_sandTile1_bmp);
  tileSprites[tiles[first].tileId].pushRotateZoom(&frameBuffer, tiles[first].relativeX, tiles[first].relativeY, tiles[first].angle,ROAD_ZOOM, ROAD_ZOOM, ALPHA_sandTile1_bmp);

  roadColor = frameBuffer.readPixelRGB(lcd.width() * 0.8, lcd.height() / 2);

  carSprite.pushRotateZoom(&frameBuffer, lcd.width() * 0.8, lcd.height() / 2, steerAngle - 90, CAR_ZOOM, CAR_ZOOM, ALPHA_car_bmp);
//  if (night)
//    headlightsSprite.pushRotateZoom(&frameBuffer, lcd.width() * 0.8, lcd.height() / 2, steerAngle - 90, 4, 4, ALPHA_headlights_bmp);

  frameBuffer.fillRect(0,0,lcd.width() * ((float)fuel / MAX_FUEL),4,lcd.color888(227,117,35));
  frameBuffer.fillRect(0,lcd.height() - 3,lcd.width() * ((float)tilesPassed / STAGE_TILES),lcd.height(),lcd.color888(35,117,227));

  lcd.pushPixelsDMA(frameBuffer.getBuffer(), lcd.width() * lcd.height());
}
void Rally::reset(){
  rotaryCount = 0;
  timeCount = 0;
  points = 0;
  carSpeed = 1;
  steerAngle = 0;
  directionAngle = 0;
  gameIsOver = false;
  terrainX = 0;
  terrainY = 0;
  terrainXPrev = 0;
  terrainYPrev = 0;
  first = 0;
  second = 1;
  third = 2;
  fourth = 3;
  fuel = MAX_FUEL;
  tilesPassed = 0;
  //night = true;
  roadColor = lcd.color888(0,0,0);
  tiles[first].relativeX = width_sandTile1_bmp * 2;
  tiles[first].relativeY = height_sandTile1_bmp / 4;//height_sandTile_bmp * ROAD_ZOOM;//height_sandTile_bmp * ROAD_ZOOM / 2;//-lcd.height();
  tiles[first].pivotX = - width_sandTile1_bmp * ROAD_ZOOM / 2;
  tiles[first].pivotY = 0;
  tiles[first].angle = 0;
  tiles[first].tileId = 0;
  tiles[second].relativeX = tiles[first].relativeX - width_sandTile1_bmp * ROAD_ZOOM / 2;
  tiles[second].relativeY = tiles[first].relativeY;
  tiles[second].pivotX = - width_sandTile1_bmp * ROAD_ZOOM / 2;
  tiles[second].pivotY = 0;
  tiles[second].angle = 0;
  tiles[second].tileId = 0;
  tiles[third].relativeX = tiles[second].relativeX - width_sandTile1_bmp * ROAD_ZOOM / 2;
  tiles[third].relativeY = tiles[second].relativeY;
  tiles[third].pivotX = - width_sandTile1_bmp * ROAD_ZOOM / 2;
  tiles[third].pivotY = 0;
  tiles[third].angle = 0;
  tiles[third].tileId = 0;
  tiles[fourth].relativeX = tiles[third].relativeX - width_sandTile1_bmp * ROAD_ZOOM / 2;
  tiles[fourth].relativeY = tiles[third].relativeY;
  tiles[fourth].pivotX = - width_sandTile1_bmp * ROAD_ZOOM / 2;
  tiles[fourth].pivotY = 0;
  tiles[fourth].angle = 0;
  tiles[fourth].tileId = 0;
}

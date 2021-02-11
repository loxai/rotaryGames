
#include "SaveDinos.hpp"
#include "Utils.hpp"

#include "earth.bmp.h"
#include "ufo.bmp.h"
#include "meteor.bmp.h"
#include "space.bmp.h"

#define ROTATE_SPEED 8

//static Utils utils;

static LGFX lcd;
static LGFX_Sprite earthSprite;
static LGFX_Sprite ufoSprite;
static LGFX_Sprite meteorSprite;
static LGFX_Sprite spaceSprite;
static LGFX_Sprite frameBuffer;


void SaveDinos::setup()
{
  lcd.init();
  lcd.setRotation(3);
  lcd.fillScreen(0x000000U);//doesn't work without this... need to set black as transparent, hmmm?

  frameBuffer.createSprite(lcd.width(), lcd.height());
  earthSprite.createSprite(width_earth_bmp, height_earth_bmp);//we load it in reset call, as this sprite changes during playtime (so requires reset)
  //Utils::initSprite(&earthSprite, width_earth_bmp, height_earth_bmp, palette_earth_bmp, image_earth_bmp);
  Utils::initSprite(&ufoSprite, width_ufo_bmp, height_ufo_bmp, palette_ufo_bmp, image_ufo_bmp);
  Utils::initSprite(&meteorSprite, width_meteor_bmp, height_meteor_bmp, palette_meteor_bmp, image_meteor_bmp);
  Utils::initSprite(&spaceSprite, width_space_bmp, height_space_bmp, palette_space_bmp, image_space_bmp);
  reset();
}
void SaveDinos::reset(){
  Utils::initSprite(&earthSprite, width_earth_bmp, height_earth_bmp, palette_earth_bmp, image_earth_bmp, false);
  for(int i = 0; i< MAX_METEORS; i ++)
    meteors[i].active = false;
  rotaryCount = 0;
  timeCount;
  orbitDistance = 0;
  centerX = 0;
  centerY = 0;
  ufoX = 0;
  ufoY = 0;
  earthShift = 0;
  earthLife = 10;
  points = 0;
  zoom = 0.6;
  gameIsOver = false;
}

void SaveDinos::loop() {
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
void SaveDinos::move(){
  orbitDistance = (width_earth_bmp - width_ufo_bmp) * zoom * 0.6;
  centerX = lcd.width() / 2;
  centerY = lcd.height() / 2;
  earthShift+=3;
  earthShift%=360;
  centerY += (lcd.height() / 4) * cos(earthShift * DEG_TO_RAD);

  ufoX = centerX - ufoSprite.width()/2 + orbitDistance * cos(rotaryCount * DEG_TO_RAD);
  ufoY = centerY - ufoSprite.height()/2 + orbitDistance * sin(rotaryCount * DEG_TO_RAD);

  for(int i = 0; i < MAX_METEORS; i++){
    if (meteors[i].active){
      int diffX = centerX - meteors[i].x;
      int diffY = centerY - meteors[i].y;

      float len = sqrt(diffX * diffX + diffY * diffY);

      if (len <= width_earth_bmp / 4){
        earthLife -= meteors[i].zoom * 10;
        if (earthLife <= 0){
          gameIsOver = true;
        }
        meteors[i].active = false;
        earthSprite.fillCircle(meteors[i].x - centerX + width_earth_bmp/2, meteors[i].y - centerY + height_earth_bmp/2, meteors[i].zoom*width_meteor_bmp * 1.2, lcd.color888(255,30,30));
        earthSprite.fillCircle(meteors[i].x - centerX + width_earth_bmp/2, meteors[i].y - centerY + height_earth_bmp/2, meteors[i].zoom*width_meteor_bmp, ALPHA_earth_bmp);
      } else {
        float normalX = diffX / len;
        float normalY = diffY / len;

        meteors[i].x += normalX * meteors[i].speed;
        meteors[i].y += normalY * meteors[i].speed;
        meteors[i].rotation+=meteors[i].rotationSpeed;

        if (abs(meteors[i].x - (ufoX + width_ufo_bmp / 2)) < width_ufo_bmp / 2 &&
          abs(meteors[i].y - (ufoY + height_ufo_bmp / 2)) < height_ufo_bmp / 2){
            points += 10;
            meteors[i].active = false;
          }
      }
    } else {
      bool activate = random(5000) < 25;
      if (activate){
          meteors[i].active = true;
          meteors[i].zoom = random(20, 70) / (float)200;
          meteors[i].x = 0;
          if (random(2) == 1)
            meteors[i].x = lcd.width();
          meteors[i].y = random(lcd.height());
          meteors[i].speed = random(10, 150) / (float)100;
          meteors[i].rotationSpeed = random(1, 500) / (float)100;
      }
    }
  }
}
void SaveDinos::paint(){
  //frameBuffer.clear();
  spaceSprite.pushSprite(&frameBuffer, 0, 0);

  frameBuffer.setCursor(lcd.width() / 2.2, lcd.height() * 0.01);
  frameBuffer.setFont(&fonts::Font2);
  frameBuffer.setTextColor(0xAAAAFFU);
  frameBuffer.printf("%d", points);

  earthSprite.setPivot(width_earth_bmp/2, height_earth_bmp/2);
  earthSprite.pushRotateZoom(&frameBuffer, centerX, centerY, 0, zoom, zoom, ALPHA_earth_bmp);

  for(int i = 0; i < MAX_METEORS; i++)
    if (meteors[i].active)
      meteorSprite.pushRotateZoom(&frameBuffer, meteors[i].x, meteors[i].y, meteors[i].rotation, meteors[i].zoom, meteors[i].zoom, ALPHA_meteor_bmp);

  ufoSprite.pushSprite(&frameBuffer, ufoX, ufoY, ALPHA_ufo_bmp);

  lcd.pushPixelsDMA(frameBuffer.getBuffer(), lcd.width() * lcd.height());
}

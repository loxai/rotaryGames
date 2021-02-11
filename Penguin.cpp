
#include "Penguin.hpp"
#include "Utils.hpp"

//#include "snowTile.bmp.h"
#include "forestTile.bmp.h"
//#include "caveTile.bmp.h"
#include "log.bmp.h"
#include "penguinLeft.bmp.h"
#include "penguinRight.bmp.h"
#include "horizon.bmp.h"
#include "fish.bmp.h"

static LGFX lcd;
static LGFX_Sprite tileSprite;
static LGFX_Sprite blockSprite;
static LGFX_Sprite frameBuffer;
static LGFX_Sprite penguinLeft;
static LGFX_Sprite penguinRight;
static LGFX_Sprite horizon;
static LGFX_Sprite fishSprite;

void Penguin::setup()
{
  lcd.init();
  lcd.setRotation(3);
  lcd.fillScreen(0xFFFFFFU);//doesn't work without this... need to set black as transparent, hmmm?

  frameBuffer.createSprite(lcd.width(), lcd.height());
  Utils::initSprite(&blockSprite, width_log_bmp, height_log_bmp, palette_log_bmp, image_log_bmp);
  Utils::initSprite(&fishSprite, width_fish_bmp, height_fish_bmp, palette_fish_bmp, image_fish_bmp);
  Utils::initSprite(&tileSprite, width_forestTile_bmp, height_forestTile_bmp, palette_forestTile_bmp, image_forestTile_bmp);
  Utils::initSprite(&horizon, width_horizon_bmp, height_horizon_bmp, palette_horizon_bmp, image_horizon_bmp);
  Utils::initSprite(&penguinLeft, width_penguinLeft_bmp, height_penguinLeft_bmp, palette_penguinLeft_bmp, image_penguinLeft_bmp);
  Utils::initSprite(&penguinRight, width_penguinRight_bmp, height_penguinRight_bmp, palette_penguinRight_bmp, image_penguinRight_bmp);

  tileSprite.setPivot(width_forestTile_bmp / 2, height_forestTile_bmp / 2);
  blockSprite.setPivot(width_log_bmp / 2, height_log_bmp / 2);
  //tileSprite.setPivot(width_snowTile_bmp / 2, height_snowTile_bmp / 2);
  penguinLeft.setPivot(width_penguinLeft_bmp / 2, height_penguinLeft_bmp);
  penguinRight.setPivot(width_penguinRight_bmp / 2, height_penguinRight_bmp);

  reset();
}
void Penguin::loop() {
  rotaryCount += Utils::rotaryInput();

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

void Penguin::move(){
  points++;
  energy-=2;
  if (energy <= 0){
    gameIsOver = true;
  }
  float speed = (hit? SHIFT_SPEED / 2.0 : SHIFT_SPEED);
  hit = false;
  shift+= speed;
  if (shift > SHIFT_RANGE){
    shift = 0;
  }
  
  if (logPosA != DISABLED_SHIFT){
    logPosAShift+=speed;
    if (logPosAShift > SHIFT_RANGE){
      logPosAShift = 0;
      logPosA = DISABLED_SHIFT;
    }
  } else {
      if (random(1000) < (logPosB == DISABLED_SHIFT? 40 : 20))
        logPosA = (random(10) - 5)/ 10.0;
  }
  if (logPosB != DISABLED_SHIFT){
    logPosBShift+=speed;
    if (logPosBShift > SHIFT_RANGE){
      logPosBShift = 0;
      logPosB = DISABLED_SHIFT;
    }
  } else {
      if (random(1000) < (logPosA == DISABLED_SHIFT? 50 : 30))
        logPosB = (random(10) - 5)/ 10.0;
  }
  if (fishPos != DISABLED_SHIFT){
    fishPosShift+=SHIFT_SPEED;
    if (fishPosShift > SHIFT_RANGE){
      fishPosShift = 0;
      fishPos = DISABLED_SHIFT;
    }
  } else {
      if (random(1000) < 15 + (1-(energy / MAX_ENERGY)) * 10){
        fishPos = (random(10))/ 10.0;
        fishPosShift = 0;
      }
  }

  penguinX += horizontalMove;
  if (penguinX < width_penguinLeft_bmp / 3 )
    penguinX = width_penguinLeft_bmp / 3;
  if (penguinX > lcd.width() - width_penguinLeft_bmp / 3)
    penguinX = lcd.width() - width_penguinLeft_bmp / 3;
  if (penguinY > PENGUIN_BASE_Y){
    penguinY = PENGUIN_BASE_Y;
    jumping = false;
  }
  if (!jumping){
    horizontalMove += rotaryCount;
//    if (Utils::sign(horizontalMove) != Utils::sign(rotaryCount)){
//      horizontalMove = 0;
//      rotaryCount = 0;
//    }
    if (horizontalMove > MAX_HORIZONTAL_MOVE){
      horizontalMove = MAX_HORIZONTAL_MOVE;
      //rotaryCount = 0;
    }
    if (horizontalMove < -MAX_HORIZONTAL_MOVE){
      horizontalMove = -MAX_HORIZONTAL_MOVE;
      //rotaryCount = 0;
    }
    if (Utils::pressA()){
      energy--;
      jump = 5;
      jumping = true;
    }
  } else{
    rotaryCount = 0;
    penguinY -= jump;
    jump -= 0.6;
  }
}
void Penguin::paint(){
  //frameBuffer.fillScreen(0x1111FFU);
  horizon.pushSprite(&frameBuffer, 0, 0);

  float turn = 0;
  for(int i = 0; i < DEPTH_LEVELS; i++){
    float zoom = (i / (DEPTH_LEVELS / 2.0)) + shift / 4;
    tileSprite.pushRotateZoom(&frameBuffer, (sin(180 * DEG_TO_RAD * (10 -i) / (float)DEPTH_LEVELS) * turn) + lcd.width() / 2, lcd.height() / 2, 0,zoom * zoom, zoom * zoom, ALPHA_forestTile_bmp);
  }

  if (logPosA != DISABLED_SHIFT){
    float zoom = 0.5 + logPosAShift / 4;
    int x = lcd.width() / 2 + logPosAShift * logPosA * lcd.width() / 3;
    int y = lcd.height() / 2 + (lcd.height() / 2) * logPosAShift * 0.6;
    blockSprite.pushRotateZoom(&frameBuffer, x, y, 0,zoom * zoom * 2, zoom * zoom * 2, ALPHA_log_bmp);

    if (!jumping && abs(y - PENGUIN_BASE_Y - height_log_bmp) < 10 && abs(x - penguinX) < width_penguinLeft_bmp * 1.5){
      frameBuffer.fillCircle((x + penguinX) / 2, (y + penguinY) / 2, 4, lcd.color888(255,100,45));
      frameBuffer.fillCircle((x + penguinX) / 2, (y + penguinY) / 2, 2, lcd.color888(255,200,90));
      energy-=10;
      hit = true;
    }
  }
  if (logPosB != DISABLED_SHIFT){
    float zoom = 0.5 + logPosBShift / 4;
    int x = lcd.width() / 2 + logPosBShift * logPosB * lcd.width() / 3;
    int y = lcd.height() / 2 + (lcd.height() / 2) * logPosBShift * 0.6;
    blockSprite.pushRotateZoom(&frameBuffer, x, y, 0,zoom * zoom * 2, zoom * zoom * 2, ALPHA_log_bmp);
    //if (!jumping && abs(y - PENGUIN_BASE_Y - height_log_bmp) < 5 && abs(x - penguinX) < width_penguinLeft_bmp){
    if (!jumping && abs(y - PENGUIN_BASE_Y - height_log_bmp) < 10 && abs(x - penguinX) < width_penguinLeft_bmp * 1.5){
      frameBuffer.fillCircle((x + penguinX) / 2, (y + penguinY) / 2, 4, lcd.color888(255,100,45));
      frameBuffer.fillCircle((x + penguinX) / 2, (y + penguinY) / 2, 2, lcd.color888(255,200,90));
      energy-=10;
      hit = true;
    }
  }
  if (fishPos != DISABLED_SHIFT){
    float zoom = 1;//0.5 + fishPosShift / 3;
    //int x = lcd.width() / 2 + fishPosShift * fishPos * lcd.width() / 3;
    //int y = lcd.height() / 2 + (lcd.height() / 6) * fishPosShift * 0.6 - sin(PI * (fishPosShift / 1.1) / SHIFT_RANGE) * 40;
    int x = 0;
    if (fishPos < 0.5)
       x = fishPos * lcd.width() + fishPosShift * lcd.width() / 4;
    else
       x = fishPos * lcd.width() - fishPosShift * lcd.width() / 4;
    int y = lcd.height() / 2 + (lcd.height() / 3) * fishPosShift * 0.6 - sin(PI * (fishPosShift) / SHIFT_RANGE) * 40;
    fishSprite.pushRotateZoom(&frameBuffer, x, y, 0,zoom * zoom, zoom * zoom, ALPHA_fish_bmp);
    if (jumping && abs(y - penguinY) < height_fish_bmp && abs(x - penguinX) < width_fish_bmp){
      frameBuffer.fillCircle((x + penguinX) / 2, (y + penguinY) / 2, 10, lcd.color888(0,100,45));
//      frameBuffer.fillCircle((x + penguinX) / 2, (y + penguinY) / 2, 2, lcd.color888(0,200,90));
      energy += MAX_ENERGY / 10;
      fishPos = DISABLED_SHIFT;
      if (energy > MAX_ENERGY)
        energy = MAX_ENERGY;
    }
  }

  if (penguinFlip < 2)
    penguinLeft.pushSprite(&frameBuffer, penguinX - width_penguinLeft_bmp / 2, penguinY - height_penguinLeft_bmp / 2, ALPHA_penguinLeft_bmp);
  else
    penguinRight.pushSprite(&frameBuffer, penguinX - width_penguinRight_bmp / 2, penguinY - height_penguinRight_bmp / 2, ALPHA_penguinRight_bmp);
  penguinFlip++;
  if (penguinFlip > 4)
    penguinFlip = 0;

  frameBuffer.fillRect(lcd.width() - 3, lcd.height() * (1-(float)energy / MAX_ENERGY), 3, lcd.height() * (float)energy / MAX_ENERGY, lcd.color888(227,117,35));

  if (!gameIsOver){
    frameBuffer.setCursor(2, 0);
    frameBuffer.setFont(&fonts::Font2);
    frameBuffer.setTextColor(0x000000U);
    frameBuffer.printf("%d", points);
    frameBuffer.setCursor(2, 1);
    frameBuffer.setFont(&fonts::Font2);
    frameBuffer.setTextColor(0xCCDDFFU);
    frameBuffer.printf("%d", points);
  }
    
  lcd.pushPixelsDMA(frameBuffer.getBuffer(), lcd.width() * lcd.height());
}
void Penguin::reset(){
  rotaryCount = 0;
  timeCount = 0;
  points = 0;
  gameIsOver = false;
  shift = 0;
  logPosA = 0;
  logPosAShift = 0;
  logPosB = 0;
  logPosBShift = 0;
  fishPos = 0;
  fishPosShift = 0;
  penguinFlip = 0;
  penguinX = lcd.width() / 2;
  penguinY = PENGUIN_BASE_Y;
  jump = 0;
  jumping = 0;
  horizontalMove = 0;
  energy = MAX_ENERGY;
  hit = false;
}

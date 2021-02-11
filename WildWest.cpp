
#include "WildWest.hpp"
#include "Utils.hpp"

#include "village.bmp.h"
#include "gunner.bmp.h"
#include "bullet.bmp.h"
#include "cowboy.bmp.h"
#include "aim.bmp.h"

static LGFX lcd;
static LGFX_Sprite frameBuffer;
static LGFX_Sprite landscape;
static LGFX_Sprite gunnerSprite;
static LGFX_Sprite bulletSprite;
static LGFX_Sprite cowboySprite;
static LGFX_Sprite aimSprite;

void WildWest::setup()
{
  lcd.init();
  lcd.setRotation(3);
  lcd.fillScreen(0xFFFFFFU);//doesn't work without this... need to set black as transparent, hmmm?

  frameBuffer.createSprite(lcd.width(), lcd.height());
  Utils::initSprite(&landscape, width_village_bmp, height_village_bmp, palette_village_bmp, image_village_bmp);
  Utils::initSprite(&gunnerSprite, width_gunner_bmp, height_gunner_bmp, palette_gunner_bmp, image_gunner_bmp);
  Utils::initSprite(&bulletSprite, width_bullet_bmp, height_bullet_bmp, palette_bullet_bmp, image_bullet_bmp);
  Utils::initSprite(&cowboySprite, width_cowboy_bmp, height_cowboy_bmp, palette_cowboy_bmp, image_cowboy_bmp);
  Utils::initSprite(&aimSprite, width_aim_bmp, height_aim_bmp, palette_aim_bmp, image_aim_bmp);

  gunnerSprite.setPivot(0, height_gunner_bmp);
  
  windows[0].x = 25;
  windows[0].y = 22;
  windows[1].x = 42;
  windows[1].y = 22;
  windows[2].x = 59;
  windows[2].y = 22;
  windows[3].x = 150;
  windows[3].y = 22;
  windows[4].x = 37;
  windows[4].y = 48;
  windows[5].x = 58;
  windows[5].y = 47;
  windows[6].x = 93;
  windows[6].y = 48;

  reset();
}
void WildWest::loop() {
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

void WildWest::move(){
  //decide if enemy should appear
  for(int i = 0; i < NUM_WINDOWS; i++){
    if (!windows[i].active){
      windows[i].active = random(1000) > 995;
      if (i == 0 || i == 1 || i == 2 || i == 5){
        windows[i].zoomXInc = 0;
        windows[i].zoomYInc = ENEMY_SHOW_SPEED;
        windows[i].currentZoomX = 1;
        windows[i].currentZoomY = 0;
      } else
      if (i == 3 || i == 4 || i == 6){
        windows[i].zoomXInc = ENEMY_SHOW_SPEED;
        windows[i].zoomYInc = 0;
        windows[i].currentZoomX = 0;
        windows[i].currentZoomY = 1;
      }
    }
  }
  //show/hide enemies on windows, decide shooting or leaving
  for(int i = 0; i < NUM_WINDOWS; i++){
    if (windows[i].active){
      windows[i].currentZoomX += windows[i].zoomXInc;
      windows[i].currentZoomY += windows[i].zoomYInc;
      if (windows[i].currentZoomX >= 1 && windows[i].currentZoomY >= 1){
        windows[i].currentZoomX = 1;
        windows[i].currentZoomY = 1;

        //decide if enemy shoots
        if (random(1000) > 980){
          for (int b = 0; b < MAX_BULLETS; b++){
            if (!enemyBullets[b].active){//if no bullets slot, we don't shoot
              enemyBullets[b].active = true;
              enemyBullets[b].x = windows[i].x + width_gunner_bmp / 2;
              enemyBullets[b].y = windows[i].y - height_gunner_bmp / 2;

              int tx = playerX + random(ENEMY_INACCURACY) - ENEMY_INACCURACY / 2;
              int ty = playerY;
              enemyBullets[b].incX = (tx - enemyBullets[b].x) * ENEMY_BULLET_SPEED;
              enemyBullets[b].incY = (ty - enemyBullets[b].y) * ENEMY_BULLET_SPEED;
              break;
            }
          }
        } else
        //no shoot, decide if enemy leaves
        if (random(1000) > 995){
          windows[i].zoomXInc = -abs(windows[i].zoomXInc);
          windows[i].zoomYInc = -abs(windows[i].zoomYInc);
        }
      }
      //enemy gone
      if (windows[i].currentZoomX < 0 || windows[i].currentZoomY < 0){
        windows[i].active = false;
      }      
    }
  }
  //move bullets
  for (int b = 0; b < MAX_BULLETS; b++){
    if (enemyBullets[b].active){
      enemyBullets[b].x += enemyBullets[b].incX;
      enemyBullets[b].y += enemyBullets[b].incY;
      //check out of screen
      if (enemyBullets[b].x < 0 || enemyBullets[b].x > WIDTH || enemyBullets[b].y < 0 || enemyBullets[b].y > HEIGHT)
        enemyBullets[b].active = false;
      else
      //check player hit
      if (enemyBullets[b].x > playerX && enemyBullets[b].x < playerX + width_cowboy_bmp * 0.9 && enemyBullets[b].y < HEIGHT && enemyBullets[b].y > playerY){
        gameIsOver = true;
        enemyBullets[b].active = false;
      }
    }
  }
  for (int b = 0; b < MAX_BULLETS; b++){
    if (playerBullets[b].active){
      playerBullets[b].x += playerBullets[b].incX;
      playerBullets[b].y += playerBullets[b].incY;
      //check out of screen
      if (playerBullets[b].x < 0 || playerBullets[b].x > WIDTH || playerBullets[b].y < 0 || playerBullets[b].y > HEIGHT)
        playerBullets[b].active = false;
      else
      //check enemy hit
      for(int i = 0; i < NUM_WINDOWS; i++)
        if (windows[i].active){
          if (playerBullets[b].x > windows[i].x && playerBullets[b].x < windows[i].x + width_gunner_bmp / 1.5 && playerBullets[b].y < windows[i].y - height_gunner_bmp / 2 && playerBullets[b].y > windows[i].y - height_gunner_bmp){
            points+=10;
            windows[i].active = false;
            playerBullets[b].active = false;
            break;
          }
        }
    }
  }

  //move/shoot player
  if (!Utils::pressA()){
    if (rotaryCount != 0)//make aim imprecise when moving
      playerAngle += random(abs(rotaryCount * 5)) - abs(rotaryCount * 5)/2;
    playerX += rotaryCount * 2;
    rotaryCount = 0;
    if (playerX > WIDTH - width_cowboy_bmp)
      playerX = WIDTH - width_cowboy_bmp;
    if (playerX < 0)
      playerX = 0;
  } else {
    playerAngle += rotaryCount * 5;
    rotaryCount = 0;
    if (playerAngle < 180)
      playerAngle = 180;
    if (playerAngle > 360)
      playerAngle = 360;
  }
  if (Utils::releaseA()){
    for (int b = 0; b < MAX_BULLETS; b++){
      if (!playerBullets[b].active){//if no bullets slot, we don't shoot
        playerBullets[b].active = true;
        playerBullets[b].x = playerX + width_cowboy_bmp / 2;
        playerBullets[b].y = playerY;// - height_cowboy_bmp / 2;

        playerBullets[b].incX = cos(DEG_TO_RAD * playerAngle) * PLAYER_BULLET_SPEED;
        playerBullets[b].incY = sin(DEG_TO_RAD * playerAngle) * PLAYER_BULLET_SPEED;
        break;
      }
    }
  }
}
void WildWest::paint(){
  frameBuffer.fillScreen(0x000000U);
  for(int i = 0; i < NUM_WINDOWS; i++){
    if (windows[i].active){
      gunnerSprite.pushRotateZoom(&frameBuffer, windows[i].x, windows[i].y, 0, windows[i].currentZoomX, windows[i].currentZoomY, ALPHA_gunner_bmp);
    }
  }
  landscape.pushSprite(&frameBuffer, 0, 0, ALPHA_village_bmp);

  frameBuffer.setCursor(WIDTH / 2, HEIGHT * 0.01);
  frameBuffer.setFont(&fonts::Font2);
  frameBuffer.setTextColor(0xAAAAFFU);
  frameBuffer.printf("%d", points);

  for (int b = 0; b < MAX_BULLETS; b++){
    if (enemyBullets[b].active){
      bulletSprite.pushSprite(&frameBuffer, enemyBullets[b].x + width_bullet_bmp / 2, enemyBullets[b].y + height_bullet_bmp / 2, ALPHA_bullet_bmp);
    }
    if (playerBullets[b].active){
      bulletSprite.pushSprite(&frameBuffer, playerBullets[b].x + width_bullet_bmp / 2, playerBullets[b].y + height_bullet_bmp / 2, ALPHA_bullet_bmp);
    }
  }

  //frameBuffer.fillCircle(playerX + width_cowboy_bmp / 2 + cos(DEG_TO_RAD * playerAngle) * 8, playerY + sin(DEG_TO_RAD * playerAngle) * 8, 2, lcd.color888(0,0,128));
  aimSprite.pushSprite(&frameBuffer, playerX + width_cowboy_bmp / 2 + cos(DEG_TO_RAD * playerAngle) * 8 - width_aim_bmp / 2, playerY + sin(DEG_TO_RAD * playerAngle) * 8 - height_aim_bmp / 2, ALPHA_aim_bmp);
  cowboySprite.pushSprite(&frameBuffer, playerX, playerY, ALPHA_bullet_bmp);
  
  lcd.pushPixelsDMA(frameBuffer.getBuffer(), lcd.width() * lcd.height());
}
void WildWest::reset(){
  rotaryCount = 0;
  timeCount = 0;
  points = 0;
  gameIsOver = false;

  for(int i = 0; i < NUM_WINDOWS; i++){
    windows[i].active = false;
  }
  for(int i = 0; i < MAX_BULLETS; i++){
    enemyBullets[i].active = false;
    playerBullets[i].active = false;
  }
  playerX = WIDTH / 2;
  playerY = HEIGHT * 0.9;
  playerAngle = 270;
}


#include "Scorched.hpp"
#include "Utils.hpp"

#include "leftTank.bmp.h"
#include "rightTank.bmp.h"
#include "horizon.bmp.h"


static LGFX lcd;
static LGFX_Sprite leftTankSprite;
static LGFX_Sprite rightTankSprite;
static LGFX_Sprite frameBuffer;
static LGFX_Sprite horizon;


void Scorched::setup()
{
  lcd.init();
  lcd.setRotation(3);
  lcd.fillScreen(0xFFFFFFU);//doesn't work without this... need to set black as transparent, hmmm?

  frameBuffer.createSprite(lcd.width(), lcd.height());
  leftTankSprite.createSprite(width_leftTank_bmp, height_leftTank_bmp);
  rightTankSprite.createSprite(width_rightTank_bmp, height_rightTank_bmp);
  Utils::initSprite(&horizon, width_horizon_bmp, height_horizon_bmp, palette_horizon_bmp, image_horizon_bmp);


  reset();
}
void Scorched::loop() {
  rotaryCount += Utils::rotaryInput();
  //rotaryCount %= 360;

  if (millis() - timeCount > 32){
    if (gameIsOver){
      if (Utils::gameOver(lcd, frameBuffer, p2Energy <= 0))
        reset();
    } else{
      move();
      paint();
    }
    timeCount = millis();
  }
}

void Scorched::move(){
  switch(currentState){
    case p1_move:
      moveTank(p1Steps, p1X, p1Y);
    break;
    case p1_point:
      aim(p1Angle, 270, 360);
    break;
    case p1_power:
      powerSelect(p1Power, p1X, p1Y);
    break;
    case p1_shoot:
      if (shooting(p1Power, p1Angle, false))
        currentState = static_cast<STATE>(currentState + 1);
    break;
    case p2_move:
      moveTank(p2Steps, p2X, p2Y);
    break;
    case p2_point:
      aim(p2Angle, 180, 270);
    break;
    case p2_power:
      powerSelect(p2Power, p2X, p2Y);
    break;
    case p2_shoot:
      if (shooting(p2Power, p2Angle, true))
        currentState = p1_move;
    break;
  }
}
void Scorched::powerSelect(float& power, int& x, int& y){
  power += Utils::sign(rotaryCount);
  rotaryCount = 0;
  if (power < 1)
    power = 1;
  if (power > MAX_POWER)
    power = MAX_POWER;
  if (Utils::releaseA()){
    bulletX = x;// + width_leftTank_bmp / 2;
    bulletY = y - height_leftTank_bmp;
    gravityPower = GRAVITY;
    currentState = static_cast<STATE>(currentState + 1);
  }
}
void Scorched::aim(float& angle, int minAngle, int maxAngle){
  angle += rotaryCount;
  rotaryCount = 0;
  if (angle < minAngle)
    angle = minAngle;
  if (angle > maxAngle)
    angle = maxAngle;
  if (Utils::releaseA()){
    currentState = static_cast<STATE>(currentState + 1);
  }
}
void Scorched::moveTank(int& steps, int& x, int& y){
  if (abs(rotaryCount) > steps)
    rotaryCount = steps * Utils::sign(rotaryCount);
  steps -= abs(rotaryCount);
  x += rotaryCount;
  rotaryCount = 0;
  if (x < 0)
    x = 0;
  if (x > WIDTH)// - width_leftTank_bmp)
    x = WIDTH;
  moveTank(x, y);
  
  if (Utils::releaseA()){
    currentState = static_cast<STATE>(currentState + 1);
    steps = MAX_STEPS;
  }  
}
bool Scorched::shooting(float& power, float& angle, bool againstP1){
  bool finished = false;
  float deltaX;
  float deltaY;
  float tempPower = power / 10;
  float tempGravityPower = gravityPower / 10;

  power *= POWER_DECREASE;
  gravityPower *= GRAVITY_ACC;

  //doing step checks, as bullet might go too fast to register proper hit
  for(int i = 0; i < 10; i ++){
    deltaX = cos(DEG_TO_RAD * angle) * tempPower * i;
    deltaY = sin(DEG_TO_RAD * angle) * tempPower * i + tempGravityPower * i;
    
    finished = checkBulletHit(bulletX + deltaX, bulletY + deltaY, againstP1);

    if (finished){
      power = MAX_POWER / 2;
      break;
    }
  }
  bulletX += deltaX;
  bulletY += deltaY;
  return finished;
}
bool Scorched::checkBulletHit(int x, int y, bool againstP1){
  bool result = false;
  if (y > HEIGHT - 1){
    y = HEIGHT - 1;
    //return true;
  }
  if (againstP1){
    if (abs(x - p1X) < width_leftTank_bmp / 2 && abs(y - p1Y) < height_leftTank_bmp / 2){
      p1Energy--;
      leftTankSprite.fillCircle(width_leftTank_bmp / 2, height_leftTank_bmp / 2, (1 - ((float)p1Energy / MAX_ENERGY)) * height_leftTank_bmp * 0.8, lcd.color888(0,0,0));
      if (p1Energy <= 0)
        gameIsOver = true;
      result = true;
    }
  } else {
    if (abs(x - p2X) < width_rightTank_bmp / 2 && abs(y - p2Y) < height_rightTank_bmp / 2){
    //if (true){
      p2Energy--;
      rightTankSprite.fillCircle(width_rightTank_bmp / 2, height_rightTank_bmp / 2, (1 - ((float)p2Energy / MAX_ENERGY)) * height_rightTank_bmp * 0.8, lcd.color888(0,0,0));
      if (p2Energy <= 0)
        gameIsOver = true;
      result = true;
    }
  }
  if (x < 0 || x > WIDTH)
    result = true;
  if ((y > 0 && soil[x][y])){
    blastSoil(x, y);
    result = true;
  }
  return result;
}
void Scorched::blastSoil(int x, int y){
  int minX = WIDTH;
  int maxX = 0;
  int maxY = 0;
  for(float a = 0; a < PI * 2; a+=0.4)
  for (int r = 1; r < BLAST_RADIUS; r++){
    int rx = x + cos(a) * r;
    int ry = y + sin(a) * r;
    if (rx >= WIDTH - 1 || rx < 0 || ry >= HEIGHT - 1 || ry < 0)
      continue;
    if (rx < minX)
      minX = rx;
    if (rx > maxX)
      maxX = rx;
    if (ry > maxY)
      maxY = ry;
    soil[rx][ry] = false;
  }
  for(int fx = minX; fx <= maxX; fx++){
    for(int fy = maxY; fy >= 0; fy--)
      fallSoil(fx, fy);
  }
  //check if soil vanished beneath tanks
  moveTank(p1X, p1Y);
  moveTank(p2X, p2Y);
}
void Scorched::fallSoil(int x, int y){
  if (y < HEIGHT){
    if (!soil[x][y + 1]){
      soil[x][y + 1] = soil[x][y];
      soil[x][y] = false;
    }
    if (y > 0)// && soil[x][y - 1])
      fallSoil(x, y - 1);
    
  }
}
void Scorched::paint(){
  //frameBuffer.fillScreen(0xFFFFFFU);
  horizon.pushSprite(&frameBuffer, 0, 0);

  for(int x = 0; x < WIDTH; x++)
    for(int y = 0; y < HEIGHT; y++)
      if (soil[x][y])
        frameBuffer.drawPixel (x, y, LGFX::color888 ( x,(x+y) / 2, y));

  int x = p1X;// + width_leftTank_bmp / 2;
  int y = p1Y - height_leftTank_bmp * 0.8;
  frameBuffer.drawLine(x, y, x + cos(p1Angle * DEG_TO_RAD) * CANNON_SIZE, y + sin(p1Angle * DEG_TO_RAD) * CANNON_SIZE, lcd.color888(0,0,0));
  leftTankSprite.pushSprite(&frameBuffer, p1X - width_leftTank_bmp / 2, p1Y - height_leftTank_bmp, ALPHA_leftTank_bmp);

  x = p2X;// + width_leftTank_bmp / 2;
  y = p2Y - height_rightTank_bmp * 0.8;
  frameBuffer.drawLine(x, y, x + cos(p2Angle * DEG_TO_RAD) * CANNON_SIZE, y + sin(p2Angle * DEG_TO_RAD) * CANNON_SIZE, lcd.color888(0,0,0));
  rightTankSprite.pushSprite(&frameBuffer, p2X - width_rightTank_bmp / 2, p2Y - height_rightTank_bmp, ALPHA_rightTank_bmp);

  frameBuffer.setCursor(WIDTH * 0.2, HEIGHT * 0.8);
  frameBuffer.setFont(&fonts::Font2);
  frameBuffer.setTextColor(0xAAAAFFU);
  switch(currentState){
    case p1_move:
      frameBuffer.printf("PLAYER 1 MOVE");
      frameBuffer.fillRect(0, HEIGHT - 2, WIDTH * ((float)p1Steps / MAX_STEPS), 2, lcd.color888(0,0,255 * ((float)p1Steps / MAX_STEPS)));
    break;
    case p1_point:
      frameBuffer.printf("PLAYER 1 AIM");
    break;
    case p1_power:
      frameBuffer.printf("PLAYER 1 POWER");
      frameBuffer.fillRect(0, HEIGHT - 2, WIDTH * ((float)p1Power / MAX_POWER), 2, lcd.color888(255 * ((float)p1Power / MAX_POWER), 0, 0));
    break;
    case p1_shoot:
      frameBuffer.fillCircle(bulletX, bulletY, 2, lcd.color888(255,0,255));
    break;
    case p2_move:
      frameBuffer.printf("PLAYER 2 MOVE");
      frameBuffer.fillRect(0, HEIGHT - 2, WIDTH * ((float)p2Steps / MAX_STEPS), 2, lcd.color888(0,0,255 * ((float)p2Steps / MAX_STEPS)));
    break;
    case p2_point:
      frameBuffer.printf("PLAYER 2 AIM");
    break;
    case p2_power:
      frameBuffer.printf("PLAYER 2 POWER");
      frameBuffer.fillRect(0, HEIGHT - 2, WIDTH * ((float)p2Power / MAX_POWER), 2, lcd.color888(255 * ((float)p2Power / MAX_POWER), 0, 0));
    break;
    case p2_shoot:
      frameBuffer.fillCircle(bulletX, bulletY, 2, lcd.color888(255,0,255));
    break;
  }

  lcd.pushPixelsDMA(frameBuffer.getBuffer(), lcd.width() * lcd.height());
}
void Scorched::reset(){
  rotaryCount = 0;
  timeCount = 0;
  gameIsOver = false;

  Utils::initSprite(&leftTankSprite, width_leftTank_bmp, height_leftTank_bmp, palette_leftTank_bmp, image_leftTank_bmp, false);
  Utils::initSprite(&rightTankSprite, width_rightTank_bmp, height_rightTank_bmp, palette_rightTank_bmp, image_rightTank_bmp, false);

  gravityPower = 
  currentState = p1_move;

  int lastVal = HEIGHT / 2;
  int hills = random(6) + 1;
  int hillsB = random(4) + 1;
  for(int x = 0; x < WIDTH; x++)
    for(int y = 0; y < HEIGHT; y++)
      soil[x][y] = y > cos(PI * x / WIDTH * hills * 2) * HEIGHT / 8 + HEIGHT / 2 + (cos(PI * (x - WIDTH / 2) / WIDTH * hillsB) * HEIGHT / 8);

  p1X = random(WIDTH / 4) + width_leftTank_bmp;
  moveTank(p1X, p1Y);
  p1Angle = 315;
  p1Steps = MAX_STEPS;
  p1Power = MAX_POWER / 2;
  p1Energy = MAX_ENERGY;

  p2X = random(WIDTH / 4) - width_rightTank_bmp + WIDTH * 3/4.0;
  moveTank(p2X, p2Y);
  p2Angle = 225;
  p2Steps = MAX_STEPS;
  p2Power = MAX_POWER / 2;
  p2Energy = MAX_ENERGY;
}
void Scorched::moveTank(int& x, int& y){
//  x = random(WIDTH / 4) + width_leftTank_bmp;
  y = 0;
  while(!soil[x][y])
    y++;
}

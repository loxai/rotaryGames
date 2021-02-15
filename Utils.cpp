#include "Utils.hpp"
#include <esp_task_wdt.h>
#include "gameOver.bmp.h"
#include <M5StickC.h>

static LGFX_Sprite gameOverSprite;
//static LGFX_Sprite frameBuffer;
//static LGFX lcd;
static int prevPinClk;
static float gameOverZoom;

static bool pressedA;
static bool pressedB;  
static bool pressedC;

static float accumulatedIMU;

int Utils::inputMode = 0;

int Utils::sign(float s){
 if (s < 0) return -1;
 if (s == 0) return 0;
 return 1;
}
void Utils::checkInputs(){
  if (pressA())
    pressedA = true;
  if (pressB())
    pressedB = true;
  if (pressC())
    pressedC = true;
}
void Utils::led(bool off){
  digitalWrite(PIN_LED, !off);
}
bool Utils::releaseA(){
  bool result = digitalRead(PIN_A) == 1 && pressedA;
  if (result)
    pressedA = false;
  return result;
}
bool Utils::releaseB(){
  bool result = digitalRead(PIN_B) == 1 && pressedB;
  if (result)
    pressedB = false;
  return result;
}
bool Utils::releaseC(){
  bool result = digitalRead(PIN_SW) == 1 && pressedC;
  if (result)
    pressedC = false;
  return result;
}
bool Utils::pressA(){
  return digitalRead(PIN_A) == 0;
}
bool Utils::pressB(){
  return digitalRead(PIN_B) == 0;
}
  //TODO PIN_SW not working? bad solder?
bool Utils::pressC(){
  return digitalRead(PIN_SW) == 0;
}
int Utils::rotaryInput(){
  int rotaryCount = 0;
  switch(Utils::inputMode){
    case 0:
    {
      int pinClk = digitalRead(PIN_CLK);
      int pinDt = digitalRead(PIN_DT);
    
      //if (pinClk != prevPinClk){
      //some encoders count twice per step, avoiding the double by only accepting DT = 1
      if (pinClk != prevPinClk and pinDt == 1){
        if (pinClk != pinDt)
          rotaryCount++;
        else
          rotaryCount--;
      }
      prevPinClk = pinClk;
    }
    break;
    case 1:
    case 2:
      float pitch;
      float roll;
      float yaw;
      int result = 0;
      float imu;
    
      //M5.IMU.getAhrsData(&pitch,&roll,&yaw);
      M5.IMU.getGyroData(&pitch,&roll,&yaw);
      if (Utils::inputMode == 1)
        imu = roll;
      else
        imu = pitch;
    
      if (abs(imu) > IMU_SENSITIVITY)
       accumulatedIMU += Utils::sign(imu);
    
      if (abs(accumulatedIMU) > IMU_SENSITIVITY * 5){
        rotaryCount = Utils::sign(accumulatedIMU);
        accumulatedIMU = 0;
      }
    break;
  }
  return rotaryCount;
}

void Utils::reboot(){
  esp_task_wdt_init(1, true);
  esp_task_wdt_add(NULL);
  while(1){}
}
bool Utils::gameOver(LGFX& lcd, LGFX_Sprite& frameBuffer, int points){

  gameOverSprite.pushRotateZoom(&frameBuffer, gameOverZoom * WIDTH / 1.5, HEIGHT / 2, gameOverZoom * 90 - 90, gameOverZoom, gameOverZoom, ALPHA_gameOver_bmp);
  gameOverZoom += 0.01;
  if (gameOverZoom >= 1){
   gameOverZoom = 1;
  }
  if (Utils::pressB())
    reboot();
  bool restartGame = Utils::pressA();
  if (restartGame)
    gameOverZoom = 0;

  //TODO save hi-scores to eprom (for each game, so need to pass game id)
  if (points >= 0){
    frameBuffer.setCursor(2, 2);
    frameBuffer.setFont(&fonts::Font2);
    frameBuffer.setTextColor(0x000000U);
    frameBuffer.printf("SCORE");
    frameBuffer.setCursor(1, 3);
    frameBuffer.setFont(&fonts::Font2);
    frameBuffer.setTextColor(0xDDDDDDU);
    frameBuffer.printf("SCORE");
    frameBuffer.setCursor(6, SCORE_VERT_SEPARATION);
    frameBuffer.setFont(&fonts::Font2);
    frameBuffer.setTextColor(0x000000U);
    frameBuffer.printf("%d", points);
    frameBuffer.setCursor(5, SCORE_VERT_SEPARATION + 1);
    frameBuffer.setFont(&fonts::Font2);
    frameBuffer.setTextColor(0xAAAAFFU);
    frameBuffer.printf("%d", points);
  
    frameBuffer.setCursor(2, SCORE_VERT_SEPARATION * 2);
    frameBuffer.setFont(&fonts::Font2);
    frameBuffer.setTextColor(0x000000U);
    frameBuffer.printf("HI-SCORE");
    frameBuffer.setCursor(1, SCORE_VERT_SEPARATION * 2 + 1);
    frameBuffer.setFont(&fonts::Font2);
    frameBuffer.setTextColor(0xDDDDDDU);
    frameBuffer.printf("HI-SCORE");
    frameBuffer.setCursor(6, SCORE_VERT_SEPARATION * 3);
    frameBuffer.setFont(&fonts::Font2);
    frameBuffer.setTextColor(0x000000U);
    frameBuffer.printf("%d", 666);//TODO eeprom saved high scores
    frameBuffer.setCursor(5, SCORE_VERT_SEPARATION * 3 + 1);
    frameBuffer.setFont(&fonts::Font2);
    frameBuffer.setTextColor(0xAAAAFFU);
    frameBuffer.printf("%d", 666);
  }

  lcd.pushPixelsDMA(frameBuffer.getBuffer(), WIDTH * HEIGHT);

  return restartGame;
}
bool Utils::gameOver(LGFX& lcd, LGFX_Sprite& frameBuffer, bool p1Wins){

  gameOverSprite.pushRotateZoom(&frameBuffer, gameOverZoom * lcd.width() / 1.5, lcd.height() / 2, gameOverZoom * 90 - 90, gameOverZoom, gameOverZoom, ALPHA_gameOver_bmp);
  gameOverZoom += 0.01;
  if (gameOverZoom >= 1){
   gameOverZoom = 1;
  }
  if (Utils::pressB())
    reboot();
  bool restartGame = Utils::pressA();
  if (restartGame)
    gameOverZoom = 0;

  frameBuffer.setCursor(2, 2);
  frameBuffer.setFont(&fonts::Font2);
  frameBuffer.setTextColor(0x000000U);
  frameBuffer.printf(p1Wins?"PLAYER 1" : "PLAYER 2");
  frameBuffer.setCursor(1, 3);
  frameBuffer.setFont(&fonts::Font2);
  frameBuffer.setTextColor(0xDDDDDDU);
  frameBuffer.printf(p1Wins?"PLAYER 1" : "PLAYER 2");
  frameBuffer.setCursor(6, SCORE_VERT_SEPARATION);
  frameBuffer.setFont(&fonts::Font2);
  frameBuffer.setTextColor(0x000000U);
  frameBuffer.printf("WINS!!!");
  frameBuffer.setCursor(5, SCORE_VERT_SEPARATION + 1);
  frameBuffer.setFont(&fonts::Font2);
  frameBuffer.setTextColor(0xAAAAFFU);
  frameBuffer.printf("WINS!!!");

  lcd.pushPixelsDMA(frameBuffer.getBuffer(), lcd.width() * lcd.height());

  return restartGame;
}
void Utils::initSprite(LGFX_Sprite *sprite, int width, int height,const byte *palette, const byte *image){
  initSprite(sprite, width, height, palette, image, true);
}
void Utils::initSprite(LGFX_Sprite *sprite, int width, int height,const byte *palette, const byte *image, bool create){
  if (create)
    sprite->createSprite(width,height);
  for (uint32_t x = 0; x < width; x++) {
    for (uint32_t y = 0; y < height; y++) {
      uint32_t index = image[y*width+x] * 3;
      byte r = palette[index];
      byte g = palette[index+1];
      byte b = palette[index+2];
      sprite->drawPixel (x, y, LGFX::color888 ( r,g,b ));
    }
  }
}
//LGFX_Sprite Utils::getBuffer(){
//  return frameBuffer;
//}
//LGFX Utils::getLcd(){
//  return lcd;
//}
void Utils::init(){
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_CLK, INPUT);
  pinMode(PIN_DT, INPUT);
  pinMode(PIN_SW, INPUT);
  pinMode(PIN_A, INPUT);
  pinMode(PIN_B, INPUT);
  pinMode(32, ANALOG);

  randomSeed(analogRead(32));

  M5.begin();
  M5.IMU.Init();
//  inputMode = 1;

//  lcd.init();
//  lcd.clear();
//  lcd.fillScreen(0xAABBCCU);

  Utils::initSprite(&gameOverSprite, width_gameOver_bmp, height_gameOver_bmp, palette_gameOver_bmp, image_gameOver_bmp);
  //frameBuffer.createSprite(WIDTH, HEIGHT);

  Utils::led(false);
#ifdef SERIAL
    while(!Serial)
      delay(100);
    Serial.begin(9600);
    Serial.println("Serial start");
#endif
}

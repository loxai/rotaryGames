#include "Utils.hpp"
#include "SaveDinos.hpp"
#include "Rally.hpp"
#include "Penguin.hpp"
#include "Scorched.hpp"
#include "WildWest.hpp"
#include "SpaceClash.hpp"

#define NUM_GAMES 6
#define MENU_VERTICAL_SEPARATION 20

String gameTitles[NUM_GAMES] = {"Meteor", "Rally","Penguin","Scorched","WildWest","Clash"};

SaveDinos saveDinos;
Rally rally;
Penguin penguin;
Scorched scorched;
WildWest wildWest;
SpaceClash spaceClash;

int gameSelect = 0;
boolean isSetup = false;
int selectedIndex = 0;
int prevSelectedIndex = -1;
int timeCount;

static LGFX lcd;
//static LGFX_Sprite frameBuffer;

void setup() {
  lcd.init();
  lcd.clear();
  lcd.fillScreen(0xAABBCCU);
//  lcd.setRotation(3);
  Utils::init();
//  lcd = Utils::getLcd();
//  frameBuffer.createSprite(lcd.width(), lcd.height());
}

void loop() {
  Utils::checkInputs();
  if (!isSetup){
      switch(gameSelect){
        case 0:
          menuLoop();
        break;
        case 1:
          saveDinos.setup();
          isSetup = true;
        break;
        case 2:
          rally.setup();
          isSetup = true;
        break;
        case 3:
          penguin.setup();
          isSetup = true;
        break;
        case 4:
          scorched.setup();
          isSetup = true;
        break;
        case 5:
          wildWest.setup();
          isSetup = true;
        break;
        case 6:
          spaceClash.setup();
          isSetup = true;
        break;
        default:
           gameSelect = 0;
        break;
    }
  }  
  else
  switch(gameSelect){
    case 1:
      saveDinos.loop();
    break;
    case 2:
      rally.loop();
    break;
    case 3:
      penguin.loop();
    break;
    case 4:
      scorched.loop();
    break;
    case 5:
      wildWest.loop();
    break;
    case 6:
      spaceClash.loop();
    break;
  }
}

void menuLoop(){
  selectedIndex += Utils::rotaryInput();

  if (selectedIndex < 0)
    selectedIndex = 0;
  if (selectedIndex >= NUM_GAMES)
    selectedIndex = NUM_GAMES - 1;

  bool change = selectedIndex != prevSelectedIndex;
  if (Utils::releaseB()){
    Utils::inputMode++;
    if (Utils::inputMode > 2)
      Utils::inputMode = 0;
    change = true;    
  }  
  if (change){// && millis() - timeCount > 64){
    lcd.fillScreen(0x112233U);
    for(int i = 0; i < NUM_GAMES; i++){
        lcd.setCursor(0, MENU_VERTICAL_SEPARATION * i);
        lcd.setFont(&fonts::Font2);
        lcd.setTextColor(0xFFAABBU);
        if (i == selectedIndex)
          lcd.printf("%d>%s", i + 1, gameTitles[i]);
        else
          lcd.printf("%d %s", i + 1, gameTitles[i]);
    }
    prevSelectedIndex = selectedIndex;

    timeCount = millis();

    lcd.setCursor(0, WIDTH * 0.9);
    lcd.setFont(&fonts::Font2);
    lcd.setTextColor(0x00FF00U);
    switch(Utils::inputMode){
      case 0:
        lcd.printf("IN: Rotary");
      break;
      case 1:
        lcd.printf("IN: IMU Ver.");
      break;
      case 2:
        lcd.printf("IN: IMU Hor.");
      break;
    }

  }
  if (Utils::pressA()){
    gameSelect = selectedIndex + 1;
  }
}

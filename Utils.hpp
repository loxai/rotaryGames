#ifndef Utils_h
#define Utils_h

#include "Arduino.h"
#include <LovyanGFX.hpp>

#define PIN_CLK 0
#define PIN_DT 36
#define PIN_SW 26
#define PIN_A 37
#define PIN_B 39
#define PIN_LED 10
//#define SERIAL
#define WIDTH 160
#define HEIGHT 80
#define SCORE_VERT_SEPARATION HEIGHT * 0.2
#define IMU_SENSITIVITY 10

class Utils{
	public:
    static void init();
    static int rotaryInput();
		static void reboot();
    static bool gameOver(LGFX& lcd, LGFX_Sprite& frameBuffer, int points);
    static bool gameOver(LGFX& lcd, LGFX_Sprite& frameBuffer, bool p1Wins);
    static void initSprite(LGFX_Sprite *sprite, int width, int height,const byte *palette, const byte *image);
    static void initSprite(LGFX_Sprite *sprite, int width, int height,const byte *palette, const byte *image, bool create);
    static void led(bool on);
    static bool pressA();
    static bool pressB();
    static bool pressC();
    static void checkInputs();
    static void clearInputs();
    static bool releaseA();
    static bool releaseB();
    static bool releaseC();
    static int sign(float s);
    static int inputMode;//0=rotary encoder, 1 imu vertical, 2 imu horizontal
//  private:
    //static float prevImu;
//    static bool pressedA;
//    static bool pressedB;  
//    static bool pressedC;  
//    Utils();
//      static int prevPinClk;
//	    static float gameOverZoom;

};
#endif

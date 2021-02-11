#ifndef Utils_h
#define Utils_h

#include "Arduino.h"
#include <LovyanGFX.hpp>

#define WIDTH 160
#define HEIGHT 80
#define SCORE_VERT_SEPARATION HEIGHT * 0.2

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
    //static LGFX getLcd();
    //static LGFX_Sprite getBuffer();
    //static LGFX_Sprite& getFrameBuffer();
//  private:
//    static bool pressedA;
//    static bool pressedB;  
//    static bool pressedC;  
//    Utils();
//      static int prevPinClk;
//	    static float gameOverZoom;

};
#endif

#include <Arduino.h>
#include <stdio.h>
#include <string>

#include <FastLED.h>
#include "fl/json.h"
#include "fl/slice.h"
#include "fx/fx_engine.h"

#include "myAnimartrix.hpp"
#include "fl/ui.h"

//#define BIG_BOARD
bool nextFxIndexRandom = false;
bool rotateAnimations = true;
uint8_t initialFxIndex = 4;
uint8_t fxIndex = initialFxIndex;

#ifdef BIG_BOARD
    #include <matrixMap_32x48_3pin.h>
#else
    #include <matrixMap_22x22.h>
#endif

#define DATA_PIN_1 2

#ifdef BIG_BOARD
    #define DATA_PIN_2 3
    #define DATA_PIN_3 4
#endif

#ifdef BIG_BOARD
    #define HEIGHT 32 
    #define WIDTH 48
    #define NUM_SEGMENTS 3
    #define NUM_LEDS_PER_SEGMENT 512
#else
    #define HEIGHT 22 
    #define WIDTH 22
    #define NUM_SEGMENTS 1
    #define NUM_LEDS_PER_SEGMENT 484
#endif

#define NUM_LEDS ( WIDTH * HEIGHT )
#define IS_SERPINTINE true

CRGB leds[NUM_LEDS];
uint16_t ledNum = 0;

uint8_t BRIGHTNESS = 75;

using namespace fl;

// MAPPINGS **********************************************************************************

extern const uint16_t loc2indSerpByRow[HEIGHT][WIDTH] PROGMEM;
extern const uint16_t loc2indProgByRow[HEIGHT][WIDTH] PROGMEM;
extern const uint16_t loc2indSerp[NUM_LEDS] PROGMEM;
extern const uint16_t loc2indProg[NUM_LEDS] PROGMEM;
extern const uint16_t loc2indProgByColBottomUp[WIDTH][HEIGHT] PROGMEM;

/*
uint16_t XY(uint8_t x, uint8_t y) {
    ledNum = loc2indProgByColBottomUp[x][y];
    return ledNum;
}
*/

// For XYMap custom mapping
uint16_t myXYFunction(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    width = WIDTH;
    height = HEIGHT;
    if (x >= width || y >= height) return 0;
    ledNum = loc2indProgByRow[y][x];
    return ledNum;
}

uint16_t myXYFunction(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

XYMap myXYmap = XYMap::constructWithUserFunction(WIDTH, HEIGHT, myXYFunction);
//XYMap xyRect = XYMap::constructRectangularGrid(WIDTH, HEIGHT);

//XYMap xyMap = XYMap::constructRectangularGrid(WIDTH, HEIGHT);
//XYMap xyMap(WIDTH, HEIGHT, IS_SERPINTINE);

//************************************************************************************************************

#define FIRST_ANIMATION RINGS
#define SECONDS_PER_ANIMATION 10

Animartrix myAnimartrix(myXYmap, FIRST_ANIMATION);
FxEngine fxEngine(NUM_LEDS);

UISlider brightness("Brightness", BRIGHTNESS, 0, 255);
//UINumberField fxIndex("Animartrix - index", 0, 0, NUM_ANIMATIONS - 1);
//UINumberField colorOrder("Color Order", 0, 0, 5);
//UISlider timeSpeed("Time Speed", 1, -10, 10, .1);

//bleControl variables ***********************************************************************
//elements that must be set before #include "bleControl.h" 


bool debug = true;
bool displayOn = true;
uint8_t colorOrder = 0;
double timeSpeed = 1;

#include "bleControl.h"


//**********************************************************************************************

void setColorOrder(int value) {
    switch(value) {
        case 0: value = RGB; break;
        case 1: value = RBG; break;
        case 2: value = GRB; break;
        case 3: value = GBR; break;
        case 4: value = BRG; break;
        case 5: value = BGR; break;
    }
    myAnimartrix.setColorOrder(static_cast<EOrder>(value));
}

//**********************************************************************************************

void setup() {

    Serial.begin(115200);

    FastLED.addLeds<WS2812B, DATA_PIN_1, GRB>(leds, 0, NUM_LEDS_PER_SEGMENT)
     .setCorrection(TypicalLEDStrip);

    #ifdef DATA_PIN_2
        FastLED.addLeds<WS2812B, DATA_PIN_2, GRB>(leds, NUM_LEDS_PER_SEGMENT, NUM_LEDS_PER_SEGMENT)
        .setCorrection(TypicalLEDStrip);
    #endif
    
    #ifdef DATA_PIN_3
    FastLED.addLeds<WS2812B, DATA_PIN_3, GRB>(leds, NUM_LEDS_PER_SEGMENT * 2, NUM_LEDS_PER_SEGMENT)
        .setCorrection(TypicalLEDStrip);
    #endif

    fxEngine.addFx(myAnimartrix);

    setColorOrder(colorOrder);
  
    bleSetup();

 }

//************************************************************************************************************



//************************************************************************************************************

void loop() {

    if (!displayOn){
      FastLED.clear();
    }
    else {
   
        FastLED.setBrightness(brightness);
        fxEngine.setSpeed(timeSpeed);
        myAnimartrix.fxSet(fxIndex);
        setColorOrder(colorOrder);

        fxEngine.draw(millis(), leds);
        
        if (rotateAnimations) {
            EVERY_N_SECONDS (SECONDS_PER_ANIMATION) { 
                if (nextFxIndexRandom) {fxIndex = random(0, NUM_ANIMATIONS - 1);}
                else {fxIndex += 1 % (NUM_ANIMATIONS - 1);}
                pAnimationCharacteristic->setValue(String(fxIndex).c_str());
                pAnimationCharacteristic->notify();
            }
        }
    }
    
    FastLED.show();


    // Readvertise if BLE disconnected
    if (!deviceConnected && wasConnected) {
    if (debug) {Serial.println("Device disconnected.");}
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();
    if (debug) {Serial.println("Start advertising");}
    wasConnected = false;
    }


}

//************************************************************************************************************

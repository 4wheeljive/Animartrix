#include <stdio.h>
#include <string>

#include <FastLED.h>
#include "fl/json.h"
#include "fl/slice.h"
#include "fx/fx_engine.h"

#include "fx/2d/animartrix.hpp"
#include "fl/ui.h"

//#include <matrixMap_22x22.h>
//#include <matrixMap_32x48_3pin.h>

#define FASTLED_HAS_UI_BUTTON 0

#define DATA_PIN_1 2
//#define DATA_PIN_2 3
//#define DATA_PIN_3 4

#define HEIGHT 22  
#define WIDTH 22
#define NUM_SEGMENTS 1
#define NUM_LEDS_PER_SEGMENT 484

#define NUM_LEDS ( WIDTH * HEIGHT )

CRGB leds[NUM_LEDS];
//uint16_t ledNum = 0;

using namespace fl;

// MAPPINGS **********************************************************************************
/*
extern const uint16_t loc2indSerpByRow[HEIGHT][WIDTH] PROGMEM;
extern const uint16_t loc2indProgByRow[HEIGHT][WIDTH] PROGMEM;
extern const uint16_t loc2indSerp[NUM_LEDS] PROGMEM;
extern const uint16_t loc2indProg[NUM_LEDS] PROGMEM;
extern const uint16_t loc2indProgByColBottomUp[WIDTH][HEIGHT] PROGMEM;

uint16_t XY(uint8_t x, uint8_t y) {
    ledNum = loc2indProgByColBottomUp[x][y];
    return ledNum;
}

// For XYMap custom mapping
uint16_t myXYFunction(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    width = WIDTH;
    height = HEIGHT;
    if (x >= width || y >= height) return 0;
    ledNum = loc2indProgByColBottomUp[x][y];
    return ledNum;
}

uint16_t myXYFunction(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

XYMap myXYmap = XYMap::constructWithUserFunction(WIDTH, HEIGHT, myXYFunction);
XYMap xyRect = XYMap::constructRectangularGrid(WIDTH, HEIGHT);
*/

XYMap xyMap = XYMap::constructRectangularGrid(WIDTH, HEIGHT);

//************************************************************************************************************

#define FIRST_ANIMATION POLAR_WAVES

Animartrix animartrix(xyMap, FIRST_ANIMATION);
FxEngine fxEngine(NUM_LEDS);

UISlider brightness("Brightness", 255, 0, 255);
UINumberField fxIndex("Animartrix - index", 0, 0, NUM_ANIMATIONS - 1);
UINumberField colorOrder("Color Order", 0, 0, 5);
UISlider timeSpeed("Time Speed", 1, -10, 10, .1);

//************************************************************************************************************

void setup() {

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

    fxEngine.addFx(animartrix);

    colorOrder.onChanged([](int value) {
        switch(value) {
            case 0: value = RGB; break;
            case 1: value = RBG; break;
            case 2: value = GRB; break;
            case 3: value = GBR; break;
            case 4: value = BRG; break;
            case 5: value = BGR; break;
        }
        animartrix.setColorOrder(static_cast<EOrder>(value));
    });

 }

//************************************************************************************************************

void loop() {

    FastLED.setBrightness(brightness);
    fxEngine.setSpeed(timeSpeed);
    static int lastFxIndex = -1;
    if (fxIndex.value() != lastFxIndex) {
        lastFxIndex = fxIndex;
        animartrix.fxSet(fxIndex);
    }
    fxEngine.draw(millis(), leds);
    FastLED.show();

}

//************************************************************************************************************
/* Be sure to set numHandles = 60 in this file:
C:\Users\Jeff\.platformio\packages\framework-arduinoespressif32\libraries\BLE\src\BLEServer.h
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string>

//BLE configuration *************************************************************

BLEServer* pServer = NULL;
BLECharacteristic* pAnimationCharacteristic = NULL;
BLECharacteristic* pColorCharacteristic = NULL;
//BLECharacteristic* pBrightnessCharacteristic = NULL;
BLECharacteristic* pSpeedCharacteristic = NULL;
//BLECharacteristic* pPaletteCharacteristic = NULL;
BLECharacteristic* pControlCharacteristic = NULL;
bool deviceConnected = false;
bool wasConnected = false;

#define SERVICE_UUID                  	"19b10000-e8f2-537e-4f6c-d104768a1214"
#define ANIMATION_CHARACTERISTIC_UUID  "19b10001-e8f2-537e-4f6c-d104768a1214"
#define COLOR_CHARACTERISTIC_UUID   	"19b10002-e8f2-537e-4f6c-d104768a1214"
#define SPEED_CHARACTERISTIC_UUID      "19b10003-e8f2-537e-4f6c-d104768a1214"
//#define SPEED_CHARACTERISTIC_UUID 		"19b10004-e8f2-537e-4f6c-d104768a1214"
//#define PALETTE_CHARACTERISTIC_UUID 	"19b10005-e8f2-537e-4f6c-d104768a1214"
#define CONTROL_CHARACTERISTIC_UUID    "19b10006-e8f2-537e-4f6c-d104768a1214"

BLEDescriptor pAnimationDescriptor(BLEUUID((uint16_t)0x2900));
BLEDescriptor pColorDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor pSpeedDescriptor(BLEUUID((uint16_t)0x2902));
//BLEDescriptor pSpeedDescriptor(BLEUUID((uint16_t)0x2903));
//BLEDescriptor pPaletteDescriptor(BLEUUID((uint16_t)0x2904));
//BLEDescriptor pControlDescriptor(BLEUUID((uint16_t)0x2905));

//Control functions***************************************************************

void animationAdjust(int newAnimation) {
   fxIndex = newAnimation;
   pAnimationCharacteristic->setValue(String(newAnimation).c_str());
   pAnimationCharacteristic->notify();
   if (debug) {
      Serial.print("Animation: ");
      Serial.println(newAnimation);
   }
}

void colorOrderAdjust(int newColorOrder) {
   colorOrder = newColorOrder;
   pColorCharacteristic->setValue(String(colorOrder).c_str());
   pColorCharacteristic->notify();
   if (debug) {
      Serial.print("Color order: ");
      Serial.println(colorOrder);
   }
}

/*void brightnessAdjust(uint8_t newBrightness) {
   BRIGHTNESS = newBrightness;
   //brightnessChanged = true;
   FastLED.setBrightness(BRIGHTNESS);
   pBrightnessCharacteristic->setValue(String(BRIGHTNESS).c_str());
   pBrightnessCharacteristic->notify();
   if (debug) {
      Serial.print("Brightness: ");
      Serial.println(BRIGHTNESS);
   }
}
   */

void speedAdjust(double newSpeed) {
   timeSpeed = newSpeed;
   pSpeedCharacteristic->setValue(String(timeSpeed).c_str());
   pSpeedCharacteristic->notify();
   if (debug) {
      Serial.print("Speed: ");
      Serial.println(timeSpeed);
   }
}
/*
void startWaves() {
   if (rotateWaves) { gCurrentPaletteNumber = random(0,gGradientPaletteCount); }
   CRGBPalette16 gCurrentPalette( gGradientPalettes[gCurrentPaletteNumber] );
   pPaletteCharacteristic->setValue(String(gCurrentPaletteNumber).c_str());
   pPaletteCharacteristic->notify();
   if (debug) {
      Serial.print("Color palette: ");
      Serial.println(gCurrentPaletteNumber);
   }
   gCurrentPaletteNumber = addmod8( gCurrentPaletteNumber, 1, gGradientPaletteCount);
   gTargetPalette = gGradientPalettes[ gCurrentPaletteNumber ];
}
*/

//Callbacks***********************************************************************

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    wasConnected = true;
    if (debug) {Serial.println("Device Connected");}
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    wasConnected = true;
  }
};

class AnimationCharacteristicCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
       uint8_t receivedValue = value[0];
       if (debug) {
         Serial.print("Animation: ");
         Serial.println(receivedValue);
       }
       
       if (receivedValue != 99) {
       
         if (receivedValue == 1) { //polar waves
            fxIndex = 0;
         }
         if (receivedValue == 2) { // spiralus
            fxIndex = 1;
         }
         if (receivedValue == 3) { // caleido1
            fxIndex = 2;
         }
         if (receivedValue == 4) { // waves
            fxIndex = 3;
         }
         if (receivedValue == 5) { // chasing spirals
            fxIndex = 4;
         }
         if (receivedValue == 6) { // rings
            fxIndex = 5;
         }
         if (receivedValue == 7) { // complex kaleido 6 
            fxIndex = 6;
         }
         if (receivedValue == 8) { // experiment 10
            fxIndex = 7;
         }
         if (receivedValue == 9) { // experiment sm1
            fxIndex = 8;
         }
         
         displayOn = true;
         animationAdjust(fxIndex);
       
       }

       if (receivedValue == 99) { //off
          displayOn = false;
       }
	
      }
   }
};

class ColorCharacteristicCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
       int receivedValue = value[0]; 
       if (debug) {
         Serial.print("Color order: ");
         Serial.println(receivedValue);
       }
       colorOrder = receivedValue;
       colorOrderAdjust(colorOrder);
	}
 }
};

/*
class BrightnessCharacteristicCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
       uint8_t receivedValue = value[0]; 
       if (debug) {
         Serial.print("Brightness adjust: ");
         Serial.println(receivedValue);
       }
       if (receivedValue == 1) {
          uint8_t newBrightness = min(BRIGHTNESS + brightnessInc,255);
          brightnessAdjust(newBrightness);
       }
       if (receivedValue == 2) {
          uint8_t newBrightness = max(BRIGHTNESS - brightnessInc,0);
          brightnessAdjust(newBrightness);
       }
    }
 }
};
*/

class SpeedCharacteristicCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
       double receivedValue = value[0]; 
       if (debug) {
         Serial.print("Speed adjust: ");
         Serial.println(receivedValue);
       }
       timeSpeed = receivedValue;
       speedAdjust(timeSpeed);
    }
 }
};


/*
class SpeedCharacteristicCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
       uint8_t receivedValue = value[0]; 
       if (debug) {
         Serial.print("Speed adjust: ");
         Serial.println(receivedValue);
       }
       if (receivedValue == 1) {
          uint8_t newSpeed = min(SPEED+1,10);
          speedAdjust(newSpeed);
       }
       if (receivedValue == 2) {
          uint8_t newSpeed = max(SPEED-1,0);
          speedAdjust(newSpeed);
       }
    }
 }
};
*/

/*
class PaletteCharacteristicCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
       uint8_t receivedValue = value[0]; 
       if (debug) {
         Serial.print("Palette: ");
         Serial.println(receivedValue);
       }
       gTargetPalette = gGradientPalettes[ receivedValue ];
       pPaletteCharacteristic->setValue(String(receivedValue).c_str());
       pPaletteCharacteristic->notify();
    }
 }
};
*/


class ControlCharacteristicCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
      uint8_t receivedValue = value[0];
      if (debug) {
         Serial.print("Control: ");
         Serial.println(receivedValue);
       } 
      if (receivedValue == 100) {
         rotateAnimations = true;
      }
      if (receivedValue == 101) {
         rotateAnimations = false;
	  }
    }
 }
};


//*******************************************************************************************

void bleSetup() {

 BLEDevice::init("AnimARTrix Playground");

 pServer = BLEDevice::createServer();
 pServer->setCallbacks(new MyServerCallbacks());

 BLEService *pService = pServer->createService(SERVICE_UUID);

 pAnimationCharacteristic = pService->createCharacteristic(
                      ANIMATION_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE |
					       BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
 pAnimationCharacteristic->setCallbacks(new AnimationCharacteristicCallbacks());
 pAnimationCharacteristic->setValue(String(fxIndex).c_str()); 
 pAnimationCharacteristic->addDescriptor(new BLE2902());

 pColorCharacteristic = pService->createCharacteristic(
                      COLOR_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE |
					       BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
 pColorCharacteristic->setValue(String(colorOrder).c_str()); 
 pColorCharacteristic->setCallbacks(new ColorCharacteristicCallbacks());
 pColorCharacteristic->addDescriptor(new BLE2902());

 /*
 pBrightnessCharacteristic = pService->createCharacteristic(
                      BRIGHTNESS_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
 pBrightnessCharacteristic->setCallbacks(new BrightnessCharacteristicCallbacks());
 pBrightnessCharacteristic->setValue(String(BRIGHTNESS).c_str()); 
 pBrightnessCharacteristic->addDescriptor(new BLE2902());
 pBrightnessDescriptor.setValue("Brightness");
 */

 pSpeedCharacteristic = pService->createCharacteristic(
                      SPEED_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
 pSpeedCharacteristic->setCallbacks(new SpeedCharacteristicCallbacks());
 pSpeedCharacteristic->setValue(String(timeSpeed).c_str());
 pSpeedCharacteristic->addDescriptor(new BLE2902());
 pSpeedDescriptor.setValue("Speed"); 

/*
pPaletteCharacteristic = pService->createCharacteristic(
                      PALETTE_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
 pPaletteCharacteristic->setCallbacks(new PaletteCharacteristicCallbacks());
 pPaletteCharacteristic->setValue(String(gCurrentPaletteNumber).c_str());
 pPaletteCharacteristic->addDescriptor(new BLE2902());
 pPaletteDescriptor.setValue("Palette"); 
*/
 
 pControlCharacteristic = pService->createCharacteristic(
                      CONTROL_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE |
					       BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
 pControlCharacteristic->setCallbacks(new ControlCharacteristicCallbacks());
 pControlCharacteristic->addDescriptor(new BLE2902());


//**************************************************************************************

 pService->start();

 BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
 pAdvertising->addServiceUUID(SERVICE_UUID);
 pAdvertising->setScanResponse(false);
 pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
 BLEDevice::startAdvertising();
 if (debug) {Serial.println("Waiting a client connection to notify...");}

}
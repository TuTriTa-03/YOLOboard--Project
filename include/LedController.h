#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>
#include <Adafruit_Neopixel.h>

class LedController 
{
public:
    LedController();

    void begin();
    void turnOn();
    void turnOff();

private:
    Adafruit_NeoPixel neopix;
};


#endif
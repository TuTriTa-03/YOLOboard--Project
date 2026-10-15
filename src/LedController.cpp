#include "LedController.h"

#define NEOPIXEL_PIN 45
#define NUM_PIXELS 1

LedController::LedController()
    : neopix(NUM_PIXELS,
            NEOPIXEL_PIN,
            NEO_GRB + NEO_KHZ800)
{
}

void LedController::begin()
{
    neopix.begin();
    neopix.clear();
    neopix.show();
}

void LedController::turnOn()
{
    neopix.setPixelColor(0, neopix.Color(255, 0, 0));
    neopix.show();
}

void LedController::turnOff()
{
    neopix.clear();
    neopix.show();
}
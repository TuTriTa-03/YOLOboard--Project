
#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>
#include "EnvironmentMonitor.h"

class LedController
{
public:
    // Initialize the LED hardware
    void begin();

    // Update the LED behavior based on the environment status
    void update(EnvironmentStatus status);

private:
    // Turn the LED on
    void turnOn();

    // Turn the LED off
    void turnOff();
};

#endif // LED_CONTROLLER_H

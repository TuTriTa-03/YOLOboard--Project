
#include "LedController.h"
#include "AppConfig.h"

// Initialize the LED hardware
void LedController::begin()
{
    pinMode(LED_PIN, OUTPUT);
    turnOff();
}

// Update the LED behavior based on the environment status
void LedController::update(EnvironmentStatus status)
{
    switch (status)
    {
        case EnvironmentStatus::NORMAL:
            // Keep the LED off when the environment is normal
            turnOff();
            break;

        case EnvironmentStatus::WARNING:
            // Turn the LED on to indicate a warning
            turnOn();
            break;

        case EnvironmentStatus::TOO_COLD:
            // Blink the LED to indicate low temperature
            turnOn();
            delay(300);
            turnOff();
            delay(300);
            break;

        case EnvironmentStatus::UNCOMFORTABLE:
            // Blink the LED faster when the environment is uncomfortable
            turnOn();
            delay(150);
            turnOff();
            delay(150);
            break;

        case EnvironmentStatus::SENSOR_ERROR:
            // Keep the LED on to indicate a sensor error
            turnOn();
            break;
    }
}

// Turn the LED on
void LedController::turnOn()
{
    digitalWrite(LED_PIN, HIGH);
}

// Turn the LED off
void LedController::turnOff()
{
    digitalWrite(LED_PIN, LOW);
}

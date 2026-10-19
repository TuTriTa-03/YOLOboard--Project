#include <Arduino.h>

#include "LedController.h"
#include "RtosManager.h"
#include "MqttController.h"

LedController led;
RtosManager rtos;
MqttController mqtt;

void setup()
{
    Serial.begin(115200);
    delay(10000);

    Serial.println();
    Serial.println("===== PINKPULSE START =====");

    led.begin();
    Serial.println("LED CONTROLLER INITIALIZED");

    if (!rtos.begin(led))
    {
        Serial.println("ERROR : RTOS initialization failed!");
        while(true)
        {
            delay(1000);
        }
    }

    Serial.println("RTOS INITIALIZED");

    if (!mqtt.begin(rtos))
    {
        Serial.println("ERROR : RTOS initialization failed!");
        while(true)
        {
            delay(1000);
        }
    }

    Serial.println("MQTT INITIALIZED");
    Serial.println("===== SETUP COMPLETE =====");
}

void loop()
{
    mqtt.loop();
    delay(2);
}

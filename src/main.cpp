
#include <Arduino.h>

#include "LedController.h"
#include "RtosManager.h"
#include "WebController.h"

// 1. Tao cac doi tuong cua he thong
LedController led;
RtosManager rtos;
WebController web;

void setup()
{
    Serial.begin(115200);
    delay(10000);

    Serial.println();
    Serial.println("===== SETUP START =====");

    // 2. Khoi tao bo dieu khien NeoPixel
    led.begin();

    Serial.println("LED CONTROLLER INITIALIZED");

    // 3. Khoi tao FreeRTOS
    // Truyen doi tuong LED cho RtosManager su dung
    if (!rtos.begin(led))
    {
        Serial.println("ERROR: RTOS initialization failed!");

        while (true)
        {
            delay(1000);
        }
    }

    Serial.println("RTOS INITIALIZED");

    // 4. Khoi tao WebServer
    // Truyen doi tuong RTOS cho WebController
    if (!web.begin(rtos))
    {
        Serial.println("ERROR: WebController initialization failed!");

        while (true)
        {
            delay(1000);
        }
    }

    Serial.println("WEBSERVER INITIALIZED");
    Serial.println("===== SETUP COMPLETE =====");
}

void loop()
{
    // Xu ly cac HTTP request tu trinh duyet
    web.handleClient();

    // Nhuong thoi gian CPU
    delay(2);
}

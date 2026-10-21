#include <Arduino.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "AppTasks.h"

// I2C1 pin configuration
// Replace these values only after verifying the board pinout.
constexpr uint8_t I2C1_SDA_PIN = 11;
constexpr uint8_t I2C1_SCL_PIN = 12;

void setup()
{
    Serial.begin(115200);
    delay(10000);

    Serial.println();
    Serial.println("================================");
    Serial.println(" Environmental Monitoring System");
    Serial.println("================================");

    // Initialize the I2C bus only once.
    Serial.println("[MAIN] Initializing I2C1...");

    if (!Wire.begin(I2C1_SDA_PIN, I2C1_SCL_PIN))
    {
        Serial.println("[MAIN] I2C initialization failed!");
        return;
    }

    Wire.setClock(100000);

    Serial.println("[MAIN] I2C1 initialized.");

    // Start the application and FreeRTOS tasks.
    Serial.println("[MAIN] Starting application...");
    startAppTasks();

    Serial.println("[MAIN] Application startup completed.");
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}

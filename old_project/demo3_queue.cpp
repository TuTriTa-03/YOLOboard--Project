#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define BOOT_BUTTON_GPIO GPIO_NUM_0
#define LED_GPIO GPIO_NUM_48

QueueHandle_t pressQueue;

uint32_t pressCount = 0;


// =====================================================
// Task 1: Monitor Button
// =====================================================
void task_monitor_button(void *pvParameters)
{
    while (1)
    {
        if (gpio_get_level(BOOT_BUTTON_GPIO) == 0)
        {
            // Debounce
            vTaskDelay(pdMS_TO_TICKS(50));

            if (gpio_get_level(BOOT_BUTTON_GPIO) == 0)
            {
                pressCount++;

                if (xQueueSend(pressQueue, &pressCount, 0) == pdTRUE)
                {
                    Serial.println("Queue Send -> OK");
                    Serial.print("Sent count: ");
                    Serial.println(pressCount);
                }
                else
                {
                    Serial.println("Queue Send -> FAILED (queue full)");
                }

                // Chờ nhả nút
                while (gpio_get_level(BOOT_BUTTON_GPIO) == 0)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


// =====================================================
// Task 2: Blink LED
// =====================================================
void task_blink_led(void *pvParameters)
{
    uint32_t receivedCount;

    while (1)
    {
        if (xQueueReceive(
                pressQueue,
                &receivedCount,
                portMAX_DELAY) == pdTRUE)
        {
            Serial.print("Received count: ");
            Serial.println(receivedCount);

            for (int i = 0; i < receivedCount; i++)
            {
                gpio_set_level(LED_GPIO, 1);
                vTaskDelay(pdMS_TO_TICKS(200));

                gpio_set_level(LED_GPIO, 0);
                vTaskDelay(pdMS_TO_TICKS(200));
            }
        }
    }
}


// =====================================================
// Setup
// =====================================================
void setup()
{
    Serial.begin(115200);

    // Button
    pinMode(BOOT_BUTTON_GPIO, INPUT_PULLUP);

    // LED
    pinMode(LED_GPIO, OUTPUT);

    pressQueue = xQueueCreate(
        5,
        sizeof(uint32_t)
    );

    if (pressQueue == NULL)
    {
        Serial.println("Queue creation FAILED");
        return;
    }

    Serial.println("Queue creation OK");

    xTaskCreate(
        task_monitor_button,
        "MonitorButton",
        4096,
        NULL,
        10,
        NULL
    );

    xTaskCreate(
        task_blink_led,
        "BlinkLED",
        4096,
        NULL,
        5,
        NULL
    );
}


// =====================================================
// Arduino loop
// =====================================================
void loop()
{
}
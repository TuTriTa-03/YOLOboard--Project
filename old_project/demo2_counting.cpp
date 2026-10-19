#include <Arduino.h>
#include "freertos/semphr.h"

#define BOOT_BUTTON_GPIO GPIO_NUM_0
#define LED_GPIO GPIO_NUM_48

SemaphoreHandle_t xCountingSemaphore;

void task_monitor_button(void *pvParameters)
{
    while (1)
    {
        if (gpio_get_level(BOOT_BUTTON_GPIO) == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(50));

            if (gpio_get_level(BOOT_BUTTON_GPIO) == 0)
            {
                if (xSemaphoreGive(xCountingSemaphore) == pdTRUE)
                    Serial.println("Give -> OK");
                else
                    Serial.println("Give failed (semaphore full)");

                while (gpio_get_level(BOOT_BUTTON_GPIO) == 0)
                    vTaskDelay(pdMS_TO_TICKS(10));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void task_blink_led(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(xCountingSemaphore, portMAX_DELAY) == pdTRUE)
        {
            gpio_set_level(LED_GPIO, 1);
            vTaskDelay(pdMS_TO_TICKS(200));

            gpio_set_level(LED_GPIO, 0);
            vTaskDelay(pdMS_TO_TICKS(200));

            Serial.print("Remaining count: ");
            Serial.println(uxSemaphoreGetCount(xCountingSemaphore));
        }
    }
}

void setup()
{
    Serial.begin(115200);

    pinMode(BOOT_BUTTON_GPIO, INPUT_PULLUP);
    pinMode(LED_GPIO, OUTPUT);

    xCountingSemaphore = xSemaphoreCreateCounting(10, 0);

    if (xCountingSemaphore == NULL)
        return;

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

void loop()
{
}
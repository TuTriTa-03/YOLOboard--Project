#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define BOOT_BUTTON_GPIO GPIO_NUM_0
#define LED_GPIO GPIO_NUM_48

SemaphoreHandle_t xBinarySemaphore;

void task_monitor_button(void *pvParameters)
{
    while(1)
    {
        if (gpio_get_level(BOOT_BUTTON_GPIO) == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(50));
            
            if (gpio_get_level(BOOT_BUTTON_GPIO) == 0)
            {
                if (xSemaphoreGive(xBinarySemaphore) == pdTRUE)
                {
                    Serial.println("Give->OK");
                }
                else
                {
                    Serial.println("Give ignored (semaphore full)");
                }

                while (gpio_get_level(BOOT_BUTTON_GPIO) == 0)
                {
                     vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void task_blink_led(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE)
        {
            for (int i = 0; i < 3; i++)
            {
                gpio_set_level(LED_GPIO, 1);
                vTaskDelay(pdMS_TO_TICKS(200));

                gpio_set_level(LED_GPIO, 0);
                vTaskDelay(pdMS_TO_TICKS(200));
            }
        }
    }
}

void setup()
{
    Serial.begin(115200);

    pinMode(BOOT_BUTTON_GPIO, INPUT_PULLUP);
    pinMode(LED_GPIO, OUTPUT);

    xBinarySemaphore = xSemaphoreCreateBinary();

    if (xBinarySemaphore == NULL)
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
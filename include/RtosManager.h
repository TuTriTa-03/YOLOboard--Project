#ifndef RTOS_MANAGER_H
#define RTOS_MANAGER_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

class LedController;

class RtosManager
{
public:
    RtosManager();

    bool begin(LedController &led);

    bool sendCommand(int command);
private:
    static void ledTask(void *pvParameters);
    static void monitorTask(void *pvParameters);

    void runLedTask();
    void runMonitorTask();

    LedController *ledController;

    QueueHandle_t ledQueue;
    QueueHandle_t ledCommandQueue;
    SemaphoreHandle_t serialMutex;

};

#endif

#include "RtosManager.h"
#include "LedController.h"

RtosManager::RtosManager()
    : ledController(nullptr),
      ledQueue(nullptr),
      ledCommandQueue(nullptr),
      serialMutex(nullptr)
{
}

bool RtosManager::begin(LedController &led)
{
    ledController = &led;

    ledQueue = xQueueCreate(5, sizeof(int));
    ledCommandQueue = xQueueCreate(5, sizeof(int));
    serialMutex = xSemaphoreCreateMutex();

    if (ledQueue == nullptr || ledCommandQueue == nullptr || serialMutex == nullptr)
    {
        Serial.println("ERROR : Failed to create RTOS objects!");
        return false;
    }

    Serial.println("LED QUEUE CREATED");
    Serial.println("LED COMMAND QUEUE CREATED");
    Serial.println("SERIAL MUTEX CREATED");

    BaseType_t ledResult = xTaskCreate(
        ledTask,
        "LED TASK",
        2048,
        this,
        1,
        nullptr
    );

    if (ledResult != pdPASS)
    {
        Serial.println("ERROR : Failedd to creat LED task!");
        return false;
    }

    Serial.println("LED TASK CHEATED");

    BaseType_t monitorResult = xTaskCreate(
        monitorTask,
        "MONITOR TASK",
        2048,
        this,
        1,
        nullptr
    );

    if (monitorResult != pdPASS)
    {
        Serial.println("ERROR: Failed to create Monitor task!");
        return false;
    }

    Serial.println("MONITOR TASK CREATED");

    return true;

};

bool RtosManager::sendCommand(int command)
{
    if (ledCommandQueue == nullptr)
    {
        return false;
    }

    if (command != 0 && command != 1)
    {
        return false;
    }

    return xQueueSend(ledCommandQueue, &command, 0) == pdPASS;
}

void RtosManager::ledTask(void *pvParameters)
{
    RtosManager *manager = 
        static_cast<RtosManager *>(pvParameters);

    manager->runLedTask();
}

void RtosManager::runLedTask()
{
    int command;

    while (true)
    {
        if (xQueueReceive(
                ledCommandQueue, &command, portMAX_DELAY) == pdPASS)
        {
            int ledState;

            if (command == 1)
            {
                ledController->turnOn();
                ledState = 1;
                Serial.println("LED ON");
            }
            else if (command == 0)
            {
                ledController->turnOff();
                ledState = 0;
                Serial.println("LED OFF");
            }
            else
            {
                continue;
            }

            if (xQueueSend(
                    ledQueue, &ledState, portMAX_DELAY) != pdPASS)
            {
                Serial.println("ERROR : Failed to send LED State");
            }
        }
    }
}

void RtosManager::monitorTask(void *pvParameters)
{
    RtosManager *manager = static_cast<RtosManager *>(pvParameters);

    manager->runMonitorTask();
}

void RtosManager::runMonitorTask()
{
    int receivedState;

    while(true)
    {
        if (xQueueReceive(
            ledQueue, &receivedState, portMAX_DELAY) == pdPASS)
        {
            if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE)
            {
                Serial.printf("Received State = %d\r\n", receivedState);

                xSemaphoreGive(serialMutex);
            }
        }
    }
}
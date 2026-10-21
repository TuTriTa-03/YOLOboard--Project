
#ifndef WEB_SERVER_MANAGER_H
#define WEB_SERVER_MANAGER_H

#include <Arduino.h>
#include <WebServer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class WebServerManager
{
public:
    // Initialize Wi-Fi and the web server
    bool begin();

    // Update the latest environmental data
    void updateData(float newTemperature,
                    float newHumidity,
                    const char *newStatus,
                    bool sensorAvailable);

    // Handle incoming HTTP requests
    void handleClient();

private:
    WebServer server{80};
    SemaphoreHandle_t dataMutex = nullptr;

    float temperature = 0.0f;
    float humidity = 0.0f;

    char environmentStatus[24] = "WAITING";
    bool hasValidData = false;
    bool sensorAvailable = false;

    // Register HTTP routes
    void configureRoutes();

    // Return the latest status as JSON
    void handleStatus();
};

#endif
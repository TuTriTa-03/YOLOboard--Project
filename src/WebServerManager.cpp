
#include "WebServerManager.h"
#include "AppConfig.h"

#include <WiFi.h>
#include <LittleFS.h>
#include <cstring>
#include <cstdio>

// Initialize Wi-Fi and the web server
bool WebServerManager::begin()
{
    dataMutex = xSemaphoreCreateMutex();

    if (dataMutex == nullptr)
    {
        Serial.println("Failed to create web data mutex!");
        return false;
    }

    // Mount LittleFS without formatting the filesystem
    if (!LittleFS.begin(false))
    {
        Serial.println("LittleFS mount failed!");
        return false;
    }

    // Connect to the configured Wi-Fi network
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");

    const unsigned long timeoutMs = 15000;
    const unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED &&
           millis() - startTime < timeoutMs)
    {
        Serial.print(".");
        delay(250);
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.print("Wi-Fi connected. IP address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("Wi-Fi connection failed.");
        Serial.println("Check WIFI_SSID and WIFI_PASSWORD.");
    }

    configureRoutes();
    server.begin();

    Serial.println("HTTP server started on port 80");

    return true;
}

// Update the latest environmental data
void WebServerManager::updateData(float newTemperature,
                                  float newHumidity,
                                  const char *newStatus,
                                  bool newSensorAvailable)
{
    if (dataMutex == nullptr)
    {
        return;
    }

    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        temperature = newTemperature;
        humidity = newHumidity;
        sensorAvailable = newSensorAvailable;

        if (newStatus != nullptr)
        {
            snprintf(
                environmentStatus,
                sizeof(environmentStatus),
                "%s",
                newStatus
            );
        }
        else
        {
            snprintf(
                environmentStatus,
                sizeof(environmentStatus),
                "%s",
                "UNKNOWN"
            );
        }

        hasValidData = true;

        xSemaphoreGive(dataMutex);
    }
}

// Handle incoming HTTP requests
void WebServerManager::handleClient()
{
    server.handleClient();
}

// Register HTTP routes
void WebServerManager::configureRoutes()
{
    // Return the latest environmental data
    server.on("/api/status", HTTP_GET, [this]()
    {
        handleStatus();
    });

    // Serve the Eilik dashboard as the home page
    server.on("/", HTTP_GET, [this]()
    {
        File file = LittleFS.open("/eliko.html", "r");

        if (!file)
        {
            server.send(404, "text/plain", "eliko.html not found");
            return;
        }

        server.streamFile(file, "text/html");
        file.close();
    });

    // Serve CSS, JavaScript, and other static files
    server.serveStatic("/", LittleFS, "/");

    // Handle unknown URLs
    server.onNotFound([this]()
    {
        server.send(
            404,
            "application/json",
            "{\"error\":\"Endpoint not found\"}"
        );
    });
}

// Return the latest status as JSON
void WebServerManager::handleStatus()
{
    float currentTemperature = 0.0f;
    float currentHumidity = 0.0f;
    bool currentHasData = false;
    bool currentSensorAvailable = false;
    char currentStatus[24] = "WAITING";

    if (dataMutex != nullptr &&
        xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        currentTemperature = temperature;
        currentHumidity = humidity;
        currentHasData = hasValidData;
        currentSensorAvailable = sensorAvailable;

        snprintf(
            currentStatus,
            sizeof(currentStatus),
            "%s",
            environmentStatus
        );

        xSemaphoreGive(dataMutex);
    }

    char response[256];

    snprintf(
        response,
        sizeof(response),
        "{\"available\":%s,"
        "\"sensorAvailable\":%s,"
        "\"wifiConnected\":%s,"
        "\"temperature\":%.2f,"
        "\"humidity\":%.2f,"
        "\"status\":\"%s\","
        "\"environmentStatus\":\"%s\"}",
        currentHasData ? "true" : "false",
        currentSensorAvailable ? "true" : "false",
        WiFi.status() == WL_CONNECTED ? "true" : "false",
        currentTemperature,
        currentHumidity,
        currentStatus,
        currentStatus
    );

    server.send(200, "application/json", response);
}

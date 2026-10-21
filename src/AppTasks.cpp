#include "AppTasks.h"
#include "AppConfig.h"
#include "DHT20Sensor.h"
#include "EnvironmentMonitor.h"
#include "LedController.h"
#include "WebServerManager.h"

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// LCD configuration
constexpr uint8_t LCD_ADDRESS = 0x21;
constexpr uint8_t LCD_COLUMNS = 16;
constexpr uint8_t LCD_ROWS = 2;

// Create the application modules
static DHT20Sensor dht20Sensor;
static EnvironmentMonitor environmentMonitor;
static LedController ledController;
static WebServerManager webServerManager;
static LiquidCrystal_I2C lcd(
    LCD_ADDRESS,
    LCD_COLUMNS,
    LCD_ROWS
);

// Convert the environment status into readable text
static const char *statusToString(EnvironmentStatus status)
{
    switch (status)
    {
        case EnvironmentStatus::NORMAL:
            return "NORMAL";

        case EnvironmentStatus::WARNING:
            return "WARNING";

        case EnvironmentStatus::TOO_COLD:
            return "TOO COLD";

        case EnvironmentStatus::UNCOMFORTABLE:
            return "UNCOMFORTABLE";

        case EnvironmentStatus::SENSOR_ERROR:
            return "SENSOR ERROR";

        default:
            return "UNKNOWN";
    }
}

// Initialize the LCD
static void initializeLCD()
{
    lcd.init();
    lcd.backlight();
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Environment");

    lcd.setCursor(0, 1);
    lcd.print("Starting...");

    Serial.println("[LCD] Initialized.");
}

// Display sensor readings on the LCD
static void updateLCD(
    float temperature,
    float humidity,
    EnvironmentStatus status
)
{
    char line1[17];
    char line2[17];

    snprintf(
        line1,
        sizeof(line1),
        "TEMP:%5.2f C",
        temperature
    );

    snprintf(
        line2,
        sizeof(line2),
        "HUM:%5.2f%%",
        humidity
    );

    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(line1);

    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(line2);

    Serial.printf(
        "[LCD] Updated | Status: %s\n",
        statusToString(status)
    );
}

// Show a sensor error on the LCD
static void showLCDSensorError()
{
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error    ");

    lcd.setCursor(0, 1);
    lcd.print("Check DHT20     ");
}

// Task responsible for monitoring the environment
static void sensorMonitorTask(void *pvParameters)
{
    bool sensorStarted = dht20Sensor.begin();

    if (!sensorStarted)
    {
        Serial.println("[DHT20] Initialization failed!");
    }

    TickType_t lastWakeTime = xTaskGetTickCount();

    while (true)
    {
        float temperature = 0.0f;
        float humidity = 0.0f;

        bool readSuccess = dht20Sensor.read(
            temperature,
            humidity
        );

        EnvironmentStatus status;

        if (readSuccess)
        {
            status = environmentMonitor.evaluate(
                temperature,
                humidity
            );

            Serial.printf(
                "Temperature: %.2f C | Humidity: %.2f %% | Status: %s\n",
                temperature,
                humidity,
                statusToString(status)
            );

            updateLCD(temperature, humidity, status);

            webServerManager.updateData(
                temperature,
                humidity,
                statusToString(status),
                true
            );
        }
        else
        {
            status = EnvironmentStatus::SENSOR_ERROR;

            Serial.println("DHT20 sensor reading failed!");

            showLCDSensorError();

            webServerManager.updateData(
                temperature,
                humidity,
                statusToString(status),
                false
            );
        }

        ledController.update(status);

        vTaskDelayUntil(
            &lastWakeTime,
            pdMS_TO_TICKS(SENSOR_INTERVAL_MS)
        );
    }
}

// Task responsible for handling web requests
static void webServerTask(void *pvParameters)
{
    while (true)
    {
        webServerManager.handleClient();

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Initialize and start the application tasks
void startAppTasks()
{
    ledController.begin();

    // Initialize the LCD before starting the sensor task
    initializeLCD();

    // Initialize Wi-Fi and the web server
    bool webStarted = webServerManager.begin();

    if (webStarted)
    {
        BaseType_t webResult = xTaskCreate(
            webServerTask,
            "WebServer",
            4096,
            nullptr,
            1,
            nullptr
        );

        if (webResult != pdPASS)
        {
            Serial.println("Failed to create WebServer task!");
        }
    }
    else
    {
        Serial.println("Web server initialization failed!");
    }

    // Create the environment monitoring task
    BaseType_t sensorResult = xTaskCreate(
        sensorMonitorTask,
        "SensorMonitor",
        4096,
        nullptr,
        1,
        nullptr
    );

    if (sensorResult != pdPASS)
    {
        Serial.println("Failed to create SensorMonitor task!");
        ledController.update(EnvironmentStatus::SENSOR_ERROR);
    }
    else
    {
        Serial.println("SensorMonitor task started successfully.");
    }
}

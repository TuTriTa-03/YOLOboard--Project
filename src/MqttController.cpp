#include "MqttController.h"
#include "RtosManager.h"

#include <ArduinoJson.h>

const char *ssid = "Thai Tuan";
const char *password = "06072003";


const char *mqttServer = "app.coreiot.io";
const int mqttPort = 1883;

const char *mqttClientId = "LED_BLINKY";
const char *mqttUsername = "ledblink";
const char *mqttPassword = "06072003";

const char *rpcRequestTopic =
    "v1/devices/me/rpc/request/+";

const char *telemetryTopic =
    "v1/devices/me/telemetry";

MqttController *MqttController::instance = nullptr;

MqttController::MqttController()
    : mqttClient(wifiClient),
      rtosManager(nullptr),
      lastBinkTime(0),
      lastPublishedState(-1)
{
}

bool MqttController::begin(RtosManager &rtos)
{
    rtosManager = &rtos;

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("WiFi connected!");
    Serial.println(WiFi.localIP());

    mqttClient.setServer(mqttServer, mqttPort); 

    instance = this;
    mqttClient.setCallback(MqttController::mqttCallback);

    if (!mqttClient.connect(
        mqttClientId,
        mqttUsername,
        mqttPassword))
    {
        Serial.println("MQTT connection failed");
        return false;
    }

    Serial.println("MQTT connected!");

    if (!mqttClient.subscribe("v1/devices/me/rpc/request/+"))
    {
        Serial.println("MQTT subscribe failed!");
        return false;
    }

    Serial.println("MQTT subscribe to RPC topic!");

    return true;
}


void MqttController::mqttCallback(
    char *topic,
    byte *payload,
    unsigned int length)
{
    if (instance != nullptr)
    {
        instance->handleMqttMessage(
            topic, 
            payload, 
            length
        );
    }
}


void MqttController::handleMqttMessage(
    char *topic,
    byte *payload,
    unsigned int length)
{
    Serial.print("MQTT topic : ");
    Serial.println(topic);

    JsonDocument doc;

    DeserializationError error = deserializeJson(
        doc,
        payload,
        length
    );

    if (error)
    {
        Serial.println("Invalid JSON! : ");
        return;
    }

    const char *method = doc["method"] | "";
    int params = doc["params"] | -1;

    if (strcmp(method, "setLedState") == 0)
    {
        if (rtosManager == nullptr)
        {
            Serial.println("ERROR: RTOS manager unavailable!");
            return;
        }

        if (params == 1)
        {
            rtosManager->sendCommand(1);
            Serial.println("LED ON command sent");
        }

        else if (params == 0)
        {
            rtosManager->sendCommand(0);
            Serial.println("LED OFF command sent");
        }

        else
        {
            Serial.println("Invalid LED state");
        }
    }
}

void MqttController::publishLedState()
{
    if (rtosManager == nullptr || !mqttClient.connected())
    {
        return;
    }

    int currentState;

    if (!rtosManager->getLedState(currentState))
    {
        Serial.println("ERROR : Failed to read LED state");
        return;
    }

    if (currentState == lastPublishedState)
    {
        return;
    }

    char payload[32];

    snprintf(payload,
        sizeof(payload),
        "{\"led\":\"%s\"}",
        currentState == 1 ? "on" : "off"
    );

    if (mqttClient.publish(telemetryTopic, payload))
    {
        lastPublishedState = currentState;

        Serial.printf(
            "Telemetry published: LED %s\r\n",
            currentState == 1 ? "ON" : "OFF"
        );
    }

    else
    {
        Serial.println("ERROR: Failed to publish telemetry");
    }
}

void MqttController::loop()
{
    mqttClient.loop();

    if (rtosManager == nullptr)
    {
        return;
    }

    unsigned long now = millis();

    if (now - lastBinkTime >= 1000)
    {
        lastBinkTime = now;

        int currentState;

        if (!rtosManager->getLedState(currentState))
        {
            Serial.println("ERROR: Failed to read LED state for auto blink");
        }
        else
        {
            int command = currentState == 1 ? 0 : 1;

            if (rtosManager->sendCommand(command))
            {
                Serial.printf(
                    "Auto blink command queued: %s\r\n",
                    command == 1 ? "ON" : "OFF"
                );
            }
            else
            {
                Serial.println(
                    "ERROR: Failed to queue auto blink command"
                );
            }
        }
    }
    
    publishLedState();
}
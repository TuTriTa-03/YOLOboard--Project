#ifndef MQTT_CONTROLLER_H
#define MQTT_CONTROLLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

class RtosManager;

class MqttController
{
public:
    MqttController();

    bool begin(RtosManager &rtos);
    void loop();

private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    RtosManager *rtosManager;

    static MqttController *instance;

    unsigned long lastBinkTime;

    int lastPublishedState;

    static void mqttCallback(
        char *topic,
        byte *payload,
        unsigned int length);

    void handleMqttMessage(
        char *topic,
        byte *payload,
        unsigned int length);

    void publishLedState();
};

#endif
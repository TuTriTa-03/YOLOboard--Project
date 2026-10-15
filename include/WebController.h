#ifndef WEB_CONTROLLER_H
#define WEB_CONTROLLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

class RtosManager;

class WebController
{
public:
    WebController();

    bool begin(RtosManager &rtos);
    void handleClient();
private:
    WebServer server;
    RtosManager *rtosManager;

    void handleRoot();
    void handleLedOn();
    void handleLedOff();
};

#endif

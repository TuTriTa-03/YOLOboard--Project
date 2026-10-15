#include "WebController.h"
#include "RtosManager.h"

#include <WiFiManager.h>
#include <ESPmDNS.h>


WebController::WebController()
    : server(80),
      rtosManager(nullptr)
{
}

bool WebController::begin(RtosManager &rtos)
{
    rtosManager = &rtos;

    // 1. Khoi tao LittleFS
    if (!LittleFS.begin(false))
    {
        Serial.println("ERROR: LittleFS mount failed!");
        return false;
    }

    if (!LittleFS.exists("/index.html"))
    {
        Serial.println("ERROR: /index.html not found!");
        return false;
    }

    Serial.println("LittleFS mounted");
    Serial.println("index.html found");

    // 2. Cau hinh Wi-Fi
    WiFi.mode(WIFI_STA);

    WiFiManager wifiManager;

    // Neu ket noi that bai, thu lai trong toi da 30 giay
    wifiManager.setConnectTimeout(30);

    // Ten AP cau hinh va mat khau AP
    // Mat khau AP can dai it nhat 8 ky tu
    const char *apName = "PinkPulse-Setup";
    const char *apPassword = "PinkPulse123";

    Serial.println("Connecting to saved Wi-Fi...");

    if (!wifiManager.autoConnect(apName, apPassword))
    {
        Serial.println("Wi-Fi setup failed or portal exited");
        return false;
    }

    Serial.println("Wi-Fi connected!");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // 3. Khoi tao mDNS
    if (MDNS.begin("pinkpulse"))
    {
        Serial.println("mDNS started: http://pinkpulse.local");
    }
    else
    {
        Serial.println("mDNS startup failed; use IP address instead");
    }

    // 4. Dang ky cac endpoint WebServer
    server.on("/", HTTP_GET, [this]() {
        handleRoot();
    });

    server.on("/on", HTTP_GET, [this]() {
        handleLedOn();
    });

    server.on("/off", HTTP_GET, [this]() {
        handleLedOff();
    });

    // 5. Bat dau WebServer
    server.begin();
    Serial.println("WebServer started!");

    return true;
}

// Hàm này được main.cpp gọi liên tục trong loop().
void WebController::handleClient()
{
    server.handleClient();
}

// Hiển thị trang HTML.
void WebController::handleRoot()
{
    File file = LittleFS.open("/index.html", "r");

    if (!file)
    {
        server.send(
            500,
            "text/plain; charset=utf-8",
            "Cannot open index.html"
        );
        return;
    }

    server.sendHeader("Cache-Control", "no-cache");
    server.streamFile(file, "text/html");
    file.close();
}

// Gửi lệnh bật LED vào RTOS queue.
void WebController::handleLedOn()
{
    if (rtosManager != nullptr &&
        rtosManager->sendCommand(1))
    {
        server.send(
            200,
            "text/plain; charset=utf-8",
            "LED ON command sent"
        );
    }
    else
    {
        server.send(
            503,
            "text/plain; charset=utf-8",
            "Command queue full or unavailable"
        );
    }
}

// Gửi lệnh tắt LED vào RTOS queue.
void WebController::handleLedOff()
{
    if (rtosManager != nullptr &&
        rtosManager->sendCommand(0))
    {
        server.send(
            200,
            "text/plain; charset=utf-8",
            "LED OFF command sent"
        );
    }
    else
    {
        server.send(
            503,
            "text/plain; charset=utf-8",
            "Command queue full or unavailable"
        );
    }
}

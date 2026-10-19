#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define NEOPIXEL_PIN 45
#define NUM_PIXELS 1

// ====================
// WiFi
// ====================
const char* ssid = "Thai Tuan";
const char* password = "06072003";

// ====================
// MQTT / CoreIoT
// ====================
const char* mqtt_server = "app.coreiot.io";
const int mqtt_port = 1883;

const char* mqtt_client_id = "LED_BLINKY";
const char* mqtt_username = "ledblink";
const char* mqtt_password = "06072003";

// ====================
// NeoPixel
// ====================
Adafruit_NeoPixel neopix(
    NUM_PIXELS,
    NEOPIXEL_PIN,
    NEO_GRB + NEO_KHZ800
);

// ====================
// MQTT Client
// ====================
WiFiClient espClient;
PubSubClient mqttClient(espClient);


// ====================
// Kết nối MQTT
// ====================
void connectMQTT()
{
    Serial.println("Dang ket noi MQTT...");

    while (!mqttClient.connected())
    {
        if (mqttClient.connect(
                mqtt_client_id,
                mqtt_username,
                mqtt_password
            ))
        {
            Serial.println("Da ket noi MQTT!");
        }
        else
        {
            Serial.print("MQTT ket noi that bai, state = ");
            Serial.println(mqttClient.state());

            delay(2000);
        }
    }
}


// ====================
// SETUP
// ====================
void setup()
{
    Serial.begin(115200);

    // Cho Serial Monitor có thời gian kết nối
    delay(10000);

    Serial.println("=== NEW FIRMWARE ===");

    // ====================
    // Khởi động NeoPixel
    // ====================
    neopix.begin();
    neopix.clear();
    neopix.show();

    Serial.println("App started");


    // ====================
    // Kết nối WiFi
    // ====================
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);

        Serial.println("Dang ket noi WiFi...");
    }

    Serial.println("Da ket noi WiFi!");

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());


    // ====================
    // Cấu hình MQTT
    // ====================
    mqttClient.setServer(
        mqtt_server,
        mqtt_port
    );

    connectMQTT();
}


// ====================
// LOOP
// ====================
void loop()
{
    // ====================
    // Duy trì kết nối MQTT
    // ====================
    mqttClient.loop();


    // ====================
    // LED ON - ĐỎ
    // ====================
    neopix.setPixelColor(
        0,
        neopix.Color(255, 0, 0)
    );

    neopix.show();

    Serial.println("LED ON - RED");

    mqttClient.publish(
        "v1/devices/me/telemetry",
        "{\"led\":\"red\"}"
    );

    delay(1000);


    // ====================
    // LED ON - VÀNG
    // ====================
    neopix.setPixelColor(
        0,
        neopix.Color(255, 255, 0)
    );

    neopix.show();

    Serial.println("LED ON - YELLOW");

    mqttClient.publish(
        "v1/devices/me/telemetry",
        "{\"led\":\"yellow\"}"
    );

    delay(1000);


    // ====================
    // LED OFF
    // ====================
    neopix.clear();
    neopix.show();

    Serial.println("LED OFF");

    mqttClient.publish(
        "v1/devices/me/telemetry",
        "{\"led\":\"off\"}"
    );

    delay(1000);
}

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

// ========================================
// 1. Hardware Configuration
// ========================================

// Built-in LED pin on the Yolo UNO S3 board
constexpr int LED_PIN = 48;

// ========================================
// 2. Sensor Configuration
// ========================================

// DHT20 sensor sampling interval in milliseconds (2 seconds)
constexpr unsigned long SENSOR_INTERVAL_MS = 2000;

// ========================================
// 3. Environmental Thresholds
// ========================================

// Upper temperature threshold in degrees Celsius
constexpr float TEMP_HIGH_THRESHOLD = 30.0f;

// Upper relative humidity threshold in percent
constexpr float HUMIDITY_HIGH_THRESHOLD = 75.0f;

// Lower temperature threshold in degrees Celsius
constexpr float TEMP_LOW_THRESHOLD = 18.0f;

 // ========================================
 // 4. Wi-Fi Configuration
 // ========================================

// Replace these values with your Wi-Fi credentials
constexpr char WIFI_SSID[] = "Thai Tuan";
constexpr char WIFI_PASSWORD[] = "06072003";

#endif // APP_CONFIG_H

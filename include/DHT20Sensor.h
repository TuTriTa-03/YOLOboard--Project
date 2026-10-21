#ifndef DHT20_SENSOR_H
#define DHT20_SENSOR_H

#include <Arduino.h>
#include <DHT20.h>

class DHT20Sensor {
public:
    // Initialize the DHT20 sensor
    bool begin();

    // Read temperature and relative humidity
    bool read(float &temperature, float &humidity);

private:
    DHT20 sensor;
};

#endif 
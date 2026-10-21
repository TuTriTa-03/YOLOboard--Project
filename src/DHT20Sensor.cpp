#include "DHT20Sensor.h"

bool DHT20Sensor::begin()
{
    // Initialize the sensor using the existing I2C bus.
    sensor.begin();

    return true;
}

bool DHT20Sensor::read(float &temperature, float &humidity)
{
    int status = sensor.read();

    if (status != DHT20_OK)
    {
        return false;
    }

    temperature = sensor.getTemperature();
    humidity = sensor.getHumidity();

    return true;
}

#include "EnvironmentMonitor.h"
#include "AppConfig.h"

EnvironmentStatus EnvironmentMonitor::evaluate(
    float temperature,
    float humidity)
{
    // Check for invalid sensor readings
    if (temperature < -40.0f || temperature > 80.0f ||
        humidity < 0.0f || humidity > 100.0f)
    {
        return EnvironmentStatus::SENSOR_ERROR;
    }

    // Check for low temperature
    if (temperature < TEMP_LOW_THRESHOLD)
    {
        return EnvironmentStatus::TOO_COLD;
    }

    // Check for uncomfortable environmental conditions
    if (temperature > TEMP_HIGH_THRESHOLD ||
        humidity > HUMIDITY_HIGH_THRESHOLD)
    {
        return EnvironmentStatus::UNCOMFORTABLE;
    }

    // Check for warning conditions
    if (temperature > 28.0f || humidity > 65.0f)
    {
        return EnvironmentStatus::WARNING;
    }

    // The environment is within the configured normal range
    return EnvironmentStatus::NORMAL;
}
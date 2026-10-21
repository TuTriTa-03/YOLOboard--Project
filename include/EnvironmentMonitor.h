#ifndef ENVIRONMENT_MONITOR_H
#define ENVIRONMENT_MONITOR_H

enum class EnvironmentStatus
{
    NORMAL,
    WARNING,
    TOO_COLD,
    UNCOMFORTABLE,
    SENSOR_ERROR
};

class EnvironmentMonitor
{
public:
    // Classify the environment using temperature and humidity
    EnvironmentStatus evaluate(float temperature, float humidity);
};

#endif 
# ESP32 Environmental Monitoring System

An embedded IoT monitoring system built with **ESP32-S3, C++, FreeRTOS, DHT20, and a web-based dashboard**. The system measures temperature and relative humidity, displays sensor readings on an LCD1602, and provides a web interface for monitoring environmental conditions.

## Project Overview

This project explores embedded firmware development, real-time task management, sensor communication, and IoT web interfaces.

### Key Features

- **Environmental sensing:** Read temperature and relative humidity from the DHT20 sensor.
- **Real-time operating system:** Use FreeRTOS tasks to organize application functions.
- **LCD display:** Display sensor readings on an LCD1602.
- **LED status indication:** Provide visual feedback based on environmental conditions.
- **Wi-Fi connectivity:** Connect the ESP32-S3 to a wireless network.
- **Embedded web server:** Serve the dashboard files stored in LittleFS.
- **Web dashboard:** Present environmental data through HTML, CSS, and JavaScript.

## System Architecture

```text
             DHT20 Sensor
                  |
                 I2C
                  |
                  v
              ESP32-S3
                  |
          +-------+-------+
          |       |       |
          v       v       v
       FreeRTOS  LCD1602  LED
          |
          v
     Wi-Fi WebServer
          |
       LittleFS
          |
          v
    HTML / CSS / JS
          |
          v
    Web Dashboard
```

## Hardware

| Component | Purpose |
|---|---|
| Yolo UNO S3 (ESP32-S3) | Main embedded controller |
| DHT20 | Temperature and humidity sensor |
| LCD1602 with I2C interface | Local data display |
| Onboard LED / NeoPixel | Environmental status indication |
| USB cable | Programming, power, and serial debugging |

## Software and Technologies

- C++ and Arduino framework
- ESP32-S3
- FreeRTOS
- I2C communication
- Wi-Fi and HTTP WebServer
- LittleFS
- HTML, CSS, and JavaScript
- PlatformIO and Visual Studio Code

## Repository Structure

```text
ESP32-Environmental-Monitor/
├── include/
│   ├── AppConfig.h
│   ├── AppTasks.h
│   ├── DHT20Sensor.h
│   ├── EnvironmentMonitor.h
│   ├── LedController.h
│   └── WebServerManager.h
├── src/
│   ├── main.cpp
│   ├── AppTasks.cpp
│   ├── DHT20Sensor.cpp
│   ├── EnvironmentMonitor.cpp
│   ├── LedController.cpp
│   └── WebServerManager.cpp
├── data/
│   ├── eliko.html
│   ├── style.css
│   └── app.js
├── platformio.ini
├── .gitignore
└── README.md
```

*The structure above describes the intended project layout. Keep the actual filenames consistent with your firmware and dashboard implementation.*

## Getting Started

### 1. Requirements

Install:

- Visual Studio Code
- PlatformIO IDE extension
- USB drivers required by your development board

Connect the Yolo UNO S3 to your computer.

### 2. Open the Firmware Project

Open this repository in Visual Studio Code and allow PlatformIO to install the required dependencies.

### 3. Configure Wi-Fi

Set the Wi-Fi configuration required by the firmware. Do not commit passwords, access tokens, or other secrets to a public repository.

### 4. Build and Upload Firmware

Use PlatformIO to build the project and upload the firmware to the ESP32-S3.

### 5. Upload the Web Filesystem

Place the dashboard assets in the `data/` directory, then run **Upload Filesystem Image** from PlatformIO.

This uploads the web interface to LittleFS, the filesystem used by the ESP32 firmware.

### 6. Open the Dashboard

Connect your computer to the network required by the board and open the IP address printed by the firmware in Serial Monitor.

The local dashboard is served by the ESP32-S3 web server.

## GitHub Pages Demo

GitHub Pages can host a static version of the HTML, CSS, and JavaScript interface.

To publish the interface:

1. Create a `docs/` directory in the repository.
2. Copy the dashboard HTML into `docs/index.html`.
3. Copy `style.css` and `app.js` into `docs/`, preserving the asset paths.
4. Push the changes to GitHub.
5. Open **Settings → Pages**.
6. Select **Deploy from a branch**, choose `main` and the `/docs` folder.
7. Save and wait for GitHub Pages to publish the site.

The published website URL will be displayed in the repository's Pages settings.

**Important:** GitHub Pages hosts the frontend only. Live sensor readings require a reachable data source. An ESP32 running on a private Wi-Fi network is not automatically accessible from a public website. A public API or cloud IoT service is needed for remote live monitoring.

## Engineering Concepts

This project demonstrates practical experience with:

- Embedded C++ application structure
- FreeRTOS task creation and scheduling
- Sensor integration over I2C
- Hardware and software debugging
- Wi-Fi connectivity and HTTP communication
- Serving static assets from embedded flash storage
- Separating hardware drivers from application logic

## Future Improvements

- Add more environmental sensors.
- Improve sensor error handling and recovery.
- Add historical data visualization.
- Integrate a cloud IoT platform for remote monitoring.
- Improve dashboard responsiveness and accessibility.
- Explore edge AI for local environmental event detection.

## Author

**Thái Tuấn**

Embedded Systems | C++ | IoT | Edge AI

GitHub: [TuTriTa-03](https://github.com/TuTriTa-03)

---

*This project is developed as a hands-on learning project focused on embedded systems, firmware architecture, and IoT integration.*
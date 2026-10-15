# LED Blinky — ESP32-S3 Embedded Project

## Overview

LED Blinky is a learning-oriented embedded systems project developed on the Yolo UNO S3 development board based on the ESP32-S3 microcontroller.

The project starts with basic LED control and serves as a foundation for learning embedded C++, FreeRTOS, and peripheral programming.

## Hardware

- **Board:** Yolo UNO S3
- **Microcontroller:** ESP32-S3
- **Onboard LED:** GPIO48, where applicable
- **Addressable RGB LED:** GPIO45, where applicable

Confirm the LED pin against the hardware configuration used by the current firmware.

## Software and Tools

- C++
- Arduino framework for ESP32
- PlatformIO IDE
- FreeRTOS

## Project Scope

The project is intended to explore:

- GPIO output and LED control.
- Embedded firmware development and deployment.
- FreeRTOS tasks and periodic execution.
- Task delays and scheduling.
- Queues and semaphores for inter-task communication and synchronization.
- Wi-Fi connectivity as a foundation for future remote-control features.

Only features implemented and tested in the current firmware should be considered completed.

## FreeRTOS Concepts

| Concept | Purpose |
|---|---|
| Task | An independent unit of execution managed by FreeRTOS. |
| `xTaskCreate()` | Creates a FreeRTOS task. |
| `vTaskDelay()` | Blocks a task for a specified number of ticks. |
| Queue | Transfers data or commands between tasks. |
| Semaphore | Coordinates task execution or access to shared resources. |

These concepts may first be tested in separate exercises before being integrated into the LED application.

## Build and Upload

1. Open the project folder in Visual Studio Code.
2. Install the PlatformIO IDE extension.
3. Connect the Yolo UNO S3 board using a USB data cable.
4. Check the board, framework, and upload settings in `platformio.ini`.
5. Build the firmware using PlatformIO.
6. Upload the firmware to the board.
7. Open the Serial Monitor if the firmware outputs diagnostic messages.

## Development Roadmap

- [ ] Organize LED control code into reusable modules.
- [ ] Integrate FreeRTOS tasks into the LED application.
- [ ] Add queue-based command handling.
- [ ] Add web-based LED control.
- [ ] Document and test the integrated system.

## Learning Outcomes

This project supports the development of practical skills in embedded C++, microcontroller programming, real-time task management, and embedded software organization.

## Related Documentation

See the [main repository README](README.md) for the overall collection of embedded and IoT projects.

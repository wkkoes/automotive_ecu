# Automotive ECU Prototype (Zephyr RTOS)

Modular embedded firmware written in C for Zephyr RTOS simulating an automotive Electronic Control Unit (ECU) and parking assistance system.

## Key Features
* **Ultrasonic Parking Assistant (HC-SR04):** Distance measurement with dynamic, distance-proportional buzzer and LED alarm frequency handled by the dashboard module.
* **Environmental Telemetry (BME280):** Real-time temperature and atmospheric pressure acquisition.
* **System Diagnostics & UI:** Heartbeat status indicator, button input handling, and structured output via the native `zephyr/logging` subsystem.
* **Modular Architecture:** Clean separation between hardware abstraction and application logic (`sensors`, `dashboard`, and `imu` modules).

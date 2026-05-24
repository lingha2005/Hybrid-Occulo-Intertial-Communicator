# HOIC - Hybrid Occulo Inertial Communicator

> A hands-free, zero-latency assistive communication device designed for completely immobilized users, utilizing 6-axis head tracking for UI navigation and IR double-blink detection for selection.

## 📖 Overview
The **Hybrid Occulo Inertial Communicator (HOIC)** is an open-source assistive technology prototype. It is engineered specifically for individuals with conditions such as ALS, Locked-in Syndrome, or severe spinal cord injuries who lack the fine motor skills required for traditional physical communication boards.

Instead of relying on expensive proprietary eye-tracking cameras, HOIC uses accessible, low-cost hardware. An ESP32 microcontroller serves a local WebSocket dashboard, an MPU6050 tracks head movements to control the UI cursor, and a digital IR sensor detects deliberate eye blinks to act as a hardware "click."

## ✨ Key Features
* **Zero-Latency Wireless Navigation:** Bidirectional, full-duplex UI control via WebSockets.
* **Hands-Free Selection:** Hardware-debounced double-blink detection using an IR obstacle avoidance sensor.
* **Standalone Local Hosting:** The HTML/JS UI is served directly from the ESP32's PROGMEM; no external internet connection or cloud backend is required.
* **Text-to-Speech Integration:** Automatic audio feedback via the browser's native `SpeechSynthesis` API.
* **Hardware Emergency Alarms:** Physical buzzer triggered remotely by the UI for critical "Pain" and "Help" requests to alert off-screen caregivers.

## 🛠️ Hardware Requirements (BOM)
* **Microcontroller:** 1x ESP32 Development Board (38-pin DevKit recommended)
* **IMU:** 1x MPU6050 (6-Axis Gyroscope/Accelerometer)
* **Trigger Sensor:** 1x Digital IR Sensor (Obstacle Avoidance Module)
* **Actuator:** 1x Active 3-Pin Buzzer Module
* **Power:** 5V USB Power Bank
* **Mounting:** Lightweight glasses frame

### Wiring & Pinout
| Component | Pin (on Module) | ESP32 Connection | Notes |
| :--- | :--- | :--- | :--- |
| **MPU6050** | VCC | 3.3V | Clean power for I2C |
| **MPU6050** | GND | GND | Common ground |
| **MPU6050** | SDA | GPIO 21 | I2C Data |
| **MPU6050** | SCL | GPIO 22 | I2C Clock |
| **IR Sensor** | VCC | 3.3V / VIN | Refer to module voltage specs |
| **IR Sensor** | GND | GND | Common ground |
| **IR Sensor** | DATA | GPIO 27 | Utilizes internal `INPUT_PULLUP` |
| **Buzzer** | VCC | 3.3V / VIN | 
| **Buzzer** | GND | GND | Common ground |
| **Buzzer** | I/O / SIG | GPIO 25 | Controlled via `OUTPUT` HIGH |

### System Architecture
```mermaid
graph TD
    subgraph Hardware Inputs
        MPU(MPU6050 IMU) -->|I2C Data: Head Tracking| ESP[ESP32 Web Server]
        IR(IR Blink Sensor) -->|Digital Input: Double-Blinks| ESP
    end

    subgraph Hosting & Communication
        ESP <-->|WebSockets: Real-Time Data| Dash[Local Device Browser]
    end

    subgraph Outputs
        Dash -->|Web Speech API| TTS((Audio Text-to-Speech))
        Dash -->|Emergency Command| ESP
        ESP -->|GPIO HIGH| Buz(Active Buzzer)
    end

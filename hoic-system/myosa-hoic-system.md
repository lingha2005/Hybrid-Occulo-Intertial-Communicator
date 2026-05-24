publishDate: 2026-05-24
title: HOIC - Hybrid Occulo Inertial Communicator
excerpt: A hands-free assistive communication device for immobilized users, utilizing 6-axis head tracking for zero-latency UI navigation and IR double-blink detection for selection.
image: hoic-system/myosa-hoic-cover.jpg
tags:
- ESP32
- MPU6050
- WebSockets
- AssistiveTech
- IoT
> Empowering communication through intuitive head tracking and hardware-debounced blink detection.

## Acknowledgements 
**Creators:** Linghadharinee A S & Lakshana S

We would like to express our deepest gratitude to our mentor, **Mr. Allwyn Ganandas**, for his invaluable guidance, technical insights, and continuous support throughout this project. We also extend our sincere thanks to our institution, **KPR Institute of Engineering and Technology (Coimbatore, India)**, for fostering an environment of innovation and engineering excellence. 

Finally, special thanks to the MYOSA event organizers for providing the foundational hardware kit and I2C modules that made this prototype possible.

## Overview
The Hybrid Occulo Inertial Communicator (HOIC) is an assistive technology prototype engineered specifically for completely immobilized users (such as those with ALS, Locked-in Syndrome, or severe spinal cord injuries) who lack the fine motor skills required for traditional physical communication boards. 

Standard assistive communication devices often require caregiver intervention, physical touch, or expensive proprietary eye-tracking cameras. HOIC solves this problem using accessible, low-cost hardware by utilizing an MPU6050 IMU to translate head movements into on-screen cursor navigation. For the "click" mechanism, a digital IR obstacle avoidance sensor detects deliberate double-blinks. 

The entire system is powered by an ESP32 microcontroller, which acts as a standalone local web server. It hosts a custom HTML/JS dashboard and broadcasts real-time sensor data over a local Wi-Fi network using WebSockets, creating a seamless, zero-latency communication tool.

**Key features:**
* **Zero-Latency Wireless Navigation:** Real-time UI control via WebSockets.
* **Hands-Free Selection:** Hardware-debounced double-blink detection using an IR sensor.
* **Standalone Hosting:** The UI is served directly from the ESP32's memory; no external internet connection is required.
* **Audio Feedback:** Automatic text-to-speech audio via the browser's SpeechSynthesis API.
* **Hardware Emergency Alarms:** Physical buzzer triggered remotely by the UI for critical "Pain" and "Help" requests.

## Demo / Examples

### **Images**
<p align="center">
<img src="/assets/images/hoic-system/myosa-hoic-hardware.jpg" width="800"><br/>
<i>The ESP32 wired to the MPU6050, IR Sensor, and Emergency Buzzer, mounted on a lightweight glasses frame.</i>
</p>

<p align="center">
<img src="/assets/images/hoic-system/myosa-hoic-dashboard.jpg" width="800"><br/>
<i>The real-time WebSocket dashboard interface served locally from the ESP32.</i>
</p>

### **Videos**
<video controls width="100%">
<source src="/myosa-hoic-demo.mp4" type="video/mp4">
</video>

## Features (Detailed)

### **1. 6-Axis Head Tracking Navigation (MPU6050)**
The system uses an MPU6050 IMU mounted on the side arm of a glasses frame. Because mounting the IMU on the side of the head creates a swinging arc motion rather than a fixed pivot, the system ignores raw accelerometer data to prevent false positional triggers. 

Instead, the software relies exclusively on the gyroscope. By integrating the X-axis (Pitch) and Z-axis (Yaw) rotation speeds (rad/s) over time, the ESP32 calculates the precise physical angle of the user's head. The software establishes a mathematical "Neutral Zone" (±5.0 degrees), ensuring that returning the head to a resting position does not accidentally register as a reverse swipe on the UI grid.

### **2. IR Double-Blink Detection**
A digital IR obstacle avoidance sensor is angled toward the outer corner of the user's eye to detect the rapid sweep of eyelashes. To ensure electrical stability and prevent hardware crashes caused by the sensor's IR LED power draw, the data pin is connected to ESP32 Pin 27 utilizing the microcontroller's internal `INPUT_PULLUP` resistor. 

To distinguish between a natural resting blink and a deliberate selection, the software implements a non-blocking hardware timer. A "Click" command is only authorized and sent via WebSocket if two complete, distinct blinks are registered within a strict 600ms window.

<pre><code>```cpp
// Example of hardware-debounced blink detection
if (millis() - lastBlinkTime < 600) {
    triggerSelectCommand();
}
```</code></pre>

### **3. ESP32 Web Server & WebSocket Integration**
The ESP32 is configured in Station Mode (`WIFI_STA`), connecting directly to a local router. The custom HTML, CSS, and JavaScript webpage is stored directly within the ESP32's PROGMEM (program memory), meaning the device acts as its own web server without needing a cloud backend. 

<p align="center">
<img src="/assets/images/hoic-system/myosa-hoic-blockdiagram.png" width="800"><br/>
<i>Block diagram illustrating the functional data, power flows, and WebSocket architecture.</i>
</p>

Real-time head movements (Up, Down, Left, Right) and clicks (Select) are broadcasted continuously via the `WebSocketsServer` library. This creates a bidirectional, full-duplex communication channel between the ESP32 and the client's browser, resulting in instantaneous UI updates.

### **4. Web Speech API & Emergency Audio Routing**
When a user double-blinks to select a word on the dashboard, the browser's built-in `SpeechSynthesis` API immediately reads the word aloud. To bypass modern browser auto-play security policies, the UI features a "Start Screen" that caregivers tap to unlock the audio engine before use. 

Additionally, if the user selects critical needs like "Pain" or "Help," the webpage sends a WebSocket command *back* to the ESP32. The microcontroller parses this command and triggers a physical active 3-pin buzzer module wired to ESP32 Pin 25, creating a loud audible alarm to alert caregivers who may not be looking at the computer screen.

## Usage Instructions
1. **Power On:** Connect the ESP32 to a 5V power bank via micro-USB.
2. **Find IP Address:** Open the Arduino Serial Monitor (9600 baud) to find the assigned local IP Address (e.g., `192.168.1.15`).
3. **Access Dashboard:** Open a web browser on any device (phone, tablet, PC) connected to the same Wi-Fi network and enter the IP address.
4. **Unlock Audio:** Tap the "Tap Here to Unlock Audio & Start HOIC" screen to authorize the browser's text-to-speech engine.
5. **Navigate:** Put the glasses on. Turn your head Left/Right or Up/Down to navigate the grid.
6. **Select:** Perform a rapid double-blink to select an option, which will highlight the cell and speak the phrase aloud.

## Tech Stack
* **Microcontroller:** ESP32 Development Board
* **Sensors:** MPU6050 (6-Axis IMU), Digital IR Sensor (Obstacle Avoidance)
* **Actuators:** Active 3-Pin Buzzer Module
* **Firmware:** C/C++ (Arduino IDE)
* **Frontend:** HTML5, CSS3, Vanilla JavaScript
* **Protocols:** WebSockets (Real-time data), HTTP, I2C, Web Speech API

## Requirements / Installation
**Arduino Board Manager:**
* ESP32 by Espressif Systems (Must be downgraded to version **2.0.17** to ensure `WebSockets` library compatibility).

**Arduino Libraries Required:**
```bash
Adafruit MPU6050
Adafruit Unified Sensor
WebSockets by Markus Sattler

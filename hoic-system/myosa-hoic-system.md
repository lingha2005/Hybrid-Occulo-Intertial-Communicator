publishDate: 2026-05-24T00:00:00Z
title: HOIC - Hybrid Occulo Inertial Communicator
excerpt: An assistive wearable device designed for immobilized users, utilizing precision head-tracking for navigation and IR double-blink detection for hands-free communication.
image: hoic-system/myosa-hoic-prototype.jpeg
tags:
- IoT
- Assistive-Tech
- ESP32
- Wearable
- Hardware

> Empowering immobilized users with hands-free, intuitive communication through precision head-tracking and blink detection.

## Acknowledgements

This project would not have been possible without the immense support and guidance of our mentor, **Dr. Allwyn Gnanadas**, and the incredible resources provided by our college, **KPR Institute of Engineering and Technology**. 

[cite_start]A massive thank you to the **MYOSA Team** for providing the official MYOSA kit, which served as the hardware foundation for this prototype[cite: 280, 296]. Finally, I want to acknowledge my teammate, **Lakshana S**, and myself, **Linghadharinee A S**, for the relentless debugging and engineering effort required to bring this vision to life.

## Overview

[cite_start]The HOIC (Hybrid Occulo Inertial Communicator) is an assistive wearable device engineered to restore independence for immobilized users through intuitive head and eye tracking[cite: 1420]. Standard communication interfaces often require fine motor skills, which can be a barrier for paralyzed individuals or those suffering from severe motor impairments. 

[cite_start]The HOIC system solves this by mapping physical head turns to cursor navigation and utilizing deliberate eye blinks as a "click" selection tool[cite: 1425, 1426]. [cite_start]By mounting a low-profile sensor stack directly onto the side arm of a standard glasses frame, the device avoids bulky headgear and ensures long-term wearability for the user[cite: 1534].

**Key features:**
* [cite_start]**Hands-Free Navigation:** Precision head-tracking maps inertia to cursor movement across a custom digital dashboard[cite: 1464].
* [cite_start]**Blink-to-Click:** Intuitive selection using digital IR proximity sensing[cite: 1465].
* [cite_start]**Zero-Latency Communication:** Real-time data transmission from the wearable to the dashboard via WebSockets[cite: 1510, 1511].
* [cite_start]**Emergency Alerts:** Immediate visual and auditory feedback (buzzer and laptop audio) when urgent options like "Pain" or "Help" are selected[cite: 1466, 1516].

***

## Demo / Examples

### **Images**

<p align="center">
<img src="/assets/images/hoic-system/myosa-hoic-prototype.jpeg" width="800"><br/>
<i>The HOIC Wearable Prototype: ESP32, MPU6050, and IR sensor mounted on the glasses frame.</i>
</p>

<p align="center">
<img src="/assets/images/hoic-system/myosa-hoic-dashboard.jpg" width="800"><br/>
<i>The Custom Web Dashboard: High-contrast grid UI for easy navigation.</i>
</p>

<p align="center">
<img src="/assets/images/hoic-system/myosa-hoic-blockdiagram.png" width="800"><br/>
<i>HOIC System Architecture and Data Flow.</i>
</p>

### **Videos**

<video controls width="100%">
<source src="/myosa-hoic-demo.mp4" type="video/mp4">
</video>

***

## Features (Detailed)

### **1. Hardware Failure & Simulation Note**
The software, logic, WebSocket communication, and hardware wiring for this project were fully completed, successfully compiled, and tested[cite: 964]. However, during the final transition from USB power to a portable battery pack, the ESP32 experienced severe voltage brownouts[cite: 965]. This caused a permanent short in the microcontroller's flash memory (`invalid header: 0xffffffff` and `FlashID=0xffffff`), rendering the board unable to host the Wi-Fi network just hours before the deadline[cite: 966]. Because the physical hardware could no longer send the WebSocket commands, the provided demonstration video uses a "Wizard of Oz" technique[cite: 967]. The user performs the physical gestures while a secondary operator manually triggers the UI to demonstrate the intended functionality of the finalized code[cite: 968].

### **2. Inertial Head-Tracking**
The system uses the MPU6050's gyroscope to track head movement[cite: 1504]. By isolating and integrating the speed of the Z and X axes over time, the microcontroller calculates the precise angle of the user's head[cite: 1505]. Deliberate head turns instantly broadcast navigation commands ("L", "R", "U", "D") to the dashboard to move the cursor.

### **3. IR Double-Blink Detection**
A digital IR obstacle avoidance sensor is mounted facing the outer corner of the eye[cite: 1506]. When the eyelashes sweep down during a deliberate double-blink, the IR beam is interrupted, sending a clear digital High/Low signal to the ESP32[cite: 1507]. A non-blocking debounce timer ensures that normal, resting blinks are ignored, triggering a "Click" (C) command only when intended.

### **4. Local Audio & Emergency Alarms**
When a selection is made, the dashboard utilizes the Web Speech API to perform text-to-speech, vocalizing the chosen option[cite: 1515]. If an emergency option ("Help" or "Pain") is selected, the UI dynamically changes to red and triggers an AudioContext digital buzzer tone through the laptop speakers to immediately alert caregivers[cite: 1516].

***

## Usage Instructions

To replicate and test the HOIC system environment:

**1. Hardware Initialization:**
Power the ESP32 via a stable USB connection (to prevent battery brownouts). Ensure the MPU6050 is seated on the I2C pins (GPIO 21, 22) and the IR sensor is on GPIO 27[cite: 1204, 1205, 1296].

**2. Running the Local Dashboard:**
The system uses a decoupled local dashboard to ensure audio reliability and responsiveness[cite: 1392]. 
Open `index.html` in Google Chrome or Microsoft Edge.

**3. Simulating the UI (Fallback Mode):**
If the hardware sensors are disconnected, you can interact with the dashboard directly via the keyboard fallback[cite: 1414]:
* [cite_start]`Arrow Keys`: Move the cursor across the grid[cite: 1415].
* [cite_start]`Enter`: Select an option (triggers full-screen and audio)[cite: 1416].
* `Enter` (Again): Close the full-screen view and return to the grid[cite: 1417].

***

## Tech Stack

**Hardware Layer**
* [cite_start]**ESP32 Microcontroller:** The main processing unit handling sensor polling and Wi-Fi[cite: 1503].
* [cite_start]**MPU6050 IMU:** Gyroscope used for mapping physical head inertia to grid navigation[cite: 1504].
* **Digital IR Sensor:** Infrared photodiode for digital blink-detection[cite: 1506].
* [cite_start]**Active Buzzer Module:** Hardware alarm for critical alerts[cite: 1508].

**Software & Connectivity Layer**
* **C++ (Arduino Core):** Embedded logic, debouncing, and low-pass filtering.
* [cite_start]**WebSockets / mDNS:** Real-time, bidirectional local wireless communication[cite: 1511, 1512].
* **HTML/CSS/JS:** Frontend UI styling and logic[cite: 1513].
* [cite_start]**Web Speech API & AudioContext:** Browser-native engines for TTS and synthesized alarms[cite: 1515, 1516].

***

## Requirements / Installation

**Hardware Dependencies:**
Ensure your ESP32 is powered via a high-quality USB Power Bank. Standard AA battery packs cannot safely handle the 400mA Wi-Fi power spike during the ESP32 boot sequence[cite: 1050].

**Software Dependencies:**
To compile the C++ code for the ESP32, install the following libraries via the Arduino IDE Library Manager:
```plaintext
Adafruit MPU6050
Adafruit Unified Sensor
WebSockets (by Markus Sattler)

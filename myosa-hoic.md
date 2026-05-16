publishDate: 2026-05-16
title: HOIC - Hybrid Occulo Inertial Communicator
excerpt: A hands-free assistive communication device for immobilized users, utilizing 6-axis head tracking for UI navigation and IR double-blink detection for selection.
image: hoic-system/hoic-cover.jpg
tags:
- ESP32
- MPU6050
- WebSockets
- AssistiveTech
- IoT
> Empowering communication through intuitive head tracking and blink detection.

## Acknowledgements 
Special thanks to the MYOSA event organizers for providing the foundational hardware kit and I2C modules that made this prototype possible.

## Overview
[cite_start]The Hybrid Occulo Inertial Communicator (HOIC) is an assistive technology prototype designed for completely immobilized users who lack the fine motor skills required for traditional communication boards[cite: 212, 340]. 

Standard communication devices often require caregiver intervention or physical touch. [cite_start]HOIC replaces the need for hands by utilizing an MPU6050 IMU to translate left/right and up/down head movements into on-screen cursor navigation[cite: 342, 343]. [cite_start]For the "click" mechanism, a digital IR obstacle avoidance sensor is mounted near the eye to detect deliberate double-blinks[cite: 588, 589]. [cite_start]The entire system runs on an ESP32, which serves a custom HTML/JS dashboard over a local Wi-Fi network and broadcasts real-time movements using WebSockets for zero-latency communication[cite: 489, 549, 558].

**Key features:**
* [cite_start]Wireless, zero-latency UI navigation via WebSockets[cite: 489].
* [cite_start]Hands-free double-blink selection using an IR sensor[cite: 589].
* [cite_start]Automatic text-to-speech audio feedback via the browser's SpeechSynthesis API[cite: 679].
* [cite_start]Hardware emergency buzzer for critical "Pain" and "Help" options[cite: 715, 716].

## Demo / Examples

### **Images**
<p align="center">
<img src="/assets/images/hoic-system/hardware-setup.jpg" width="800"><br/>
<i>The ESP32 wired to the MPU6050, IR Sensor, and Emergency Buzzer mounted on the glasses frame.</i>
</p>

### **Videos**
<video controls width="100%">
<source src="/hoic-horizontal-demo.mp4" type="video/mp4">
</video>

## Features (Detailed)

### **1. 6-Axis Head Tracking Navigation**
[cite_start]The system uses an MPU6050 IMU mounted on the side arm of a glasses frame[cite: 216]. [cite_start]Because mounting the IMU on the side of the head changes its motion profile (creating a swinging arc), the system ignores raw accelerometer data to prevent false triggers[cite: 240, 241, 243]. Instead, it relies exclusively on the gyroscope. 

[cite_start]By integrating the X-axis and Z-axis rotation speeds (rad/s) over time, the ESP32 calculates the precise physical angle of the user's head (Pitch and Yaw)[cite: 469, 470, 693]. [cite_start]The software establishes a mathematical "Neutral Zone" (±5.0 degrees), ensuring that returning the head to a resting position is not accidentally registered as a reverse swipe[cite: 470, 471, 669].

### **2. Hardware-Debounced Double-Blink IR Triggering**
[cite_start]A digital IR obstacle avoidance sensor is angled specifically toward the outer corner of the user's eye to detect the sweep of eyelashes[cite: 598, 602]. [cite_start]To prevent power brownouts and hardware crashes caused by the sensor's IR LED power draw, the sensor's data pin is connected to ESP32 Pin 27 using the internal `INPUT_PULLUP` resistor[cite: 661, 664]. 

To distinguish between a natural resting blink and a deliberate selection, the software implements a non-blocking timer. [cite_start]A "Click" command is only sent if two complete blinks are registered within a strict 600ms window[cite: 589, 703].

### **3. WebSocket UI & Browser Audio Engine**
[cite_start]The ESP32 is configured in Station Mode (`WIFI_STA`), connecting directly to a local router[cite: 549, 551]. [cite_start]It serves a custom HTML, CSS, and JavaScript webpage directly from its PROGMEM memory[cite: 495, 671]. 

[cite_start]Real-time head movements (U, D, L, R) and clicks (C) are broadcasted via WebSockets[cite: 695, 696, 698]. [cite_start]When a user double-blinks to select a word, the browser's built-in `SpeechSynthesis` API immediately reads the word aloud[cite: 679]. [cite_start]To bypass modern browser auto-play security policies, the UI features a "Start Screen" that caregivers tap to unlock the audio engine before placing the glasses on the user[cite: 748, 751].

### **4. Caregiver Emergency Alarms**
[cite_start]If the user selects critical needs like "Pain" or "Help," the webpage sends a WebSocket command back to the ESP32[cite: 726]. [cite_start]This triggers a physical active 3-pin buzzer module wired to ESP32 Pin 25, creating a loud audible alarm to alert caregivers who may not be looking at the computer screen[cite: 716, 722, 741]. [cite_start]The buzzer persists until the user double-blinks again to dismiss the fullscreen alert[cite: 715, 727].

## Usage Instructions
1. [cite_start]Power on the ESP32 via USB[cite: 597].
2. [cite_start]Open the Arduino Serial Monitor (9600 baud) to find the assigned IP Address (e.g., `192.168.1.15`)[cite: 552, 555].
3. [cite_start]Open a web browser on any device connected to the same Wi-Fi network and enter the IP address[cite: 556].
4. [cite_start]Tap the "Tap Here to Unlock Audio & Start HOIC" screen to authorize the browser's text-to-speech engine[cite: 753].
5. Put the glasses on. [cite_start]Turn your head Left/Right or Up/Down to navigate the grid[cite: 566].
6. [cite_start]Perform a rapid double-blink to select an option, which will enlarge the selection and speak it aloud[cite: 607, 706].

## Tech Stack
* [cite_start]**Microcontroller:** ESP32 [cite: 485]
* [cite_start]**Sensors:** MPU6050 (IMU) [cite: 341][cite_start], Digital IR Sensor (Obstacle Avoidance) [cite: 343]
* [cite_start]**Actuators:** Active 3-Pin Buzzer Module [cite: 735]
* [cite_start]**Software:** C/C++ (Arduino IDE), HTML, CSS, JavaScript [cite: 180, 671]
* [cite_start]**Protocols:** WebSockets, I2C, Web Speech API [cite: 180, 238, 679]

## Requirements / Installation
**Arduino Board Manager:**
* [cite_start]ESP32 by Espressif Systems (Must be downgraded to version **2.0.17** to ensure WebSocket compatibility)[cite: 509, 512].

**Arduino Libraries Required:**
```bash
Adafruit MPU6050
Adafruit Unified Sensor
WebSockets by Markus Sattler

publishDate: 2026-05-24T00:00:00Z
title: HOIC - Hybrid Occulo Inertial Communicator
excerpt: A comprehensive assistive wearable device engineered for immobilized users, utilizing zero-latency inertial head-tracking for UI navigation and infrared double-blink detection for completely hands-free communication.
image: hoic-system/myosa-hoic-cover.jpg
tags:
- IoT
- Assistive-Tech
- ESP32
- Wearable
- Embedded-Systems

> Empowering immobilized users with completely hands-free, intuitive communication through precision head-tracking and infrared blink detection.

## Acknowledgements

This project represents the culmination of intense research, hardware integration, and software development, none of which would have been possible without the profound support and guidance of our mentor, **Dr. Allwyn Gnanadas**. We also extend our deepest gratitude to our institution, **KPR Institute of Engineering and Technology**, for providing the state-of-the-art facilities and academic environment necessary to foster such innovation. 

A special and massive thank you to the **MYOSA Team** for sending the official MYOSA kit, which served as the critical hardware foundation for this entire prototype. Finally, this project is the shared achievement of our dedicated engineering team: **Lakshana S** and **Linghadharinee A S**, who spent countless hours writing algorithms, soldering components, and refining the user experience to bring this vision to life.

## Overview

The **Hybrid Occulo Inertial Communicator (HOIC)** is an advanced, non-invasive assistive technology wearable designed specifically to restore autonomy and communication capabilities to individuals suffering from severe motor impairments, locked-in syndrome, Amyotrophic Lateral Sclerosis (ALS), severe cerebral palsy, and post-stroke paralysis. 

Communication is a fundamental human right. However, standard human-computer interfaces (HCIs) such as keyboards, mice, and touchscreens rely heavily on fine motor skills and hand-eye coordination. For a paralyzed individual, these traditional interfaces are entirely inaccessible. While commercial solutions like specialized eye-tracking cameras or Brain-Computer Interfaces (BCIs) do exist, they are prohibitively expensive, require bulky hardware setups, rely on complex proprietary software, and often demand exhaustive daily calibration by trained medical professionals. 

The HOIC system democratizes assistive communication by leveraging low-cost, high-precision micro-electromechanical systems (MEMS) and infrared optics to create a wearable device that mounts directly onto the temple of a standard pair of eyeglasses. By utilizing the subtle, retained anatomical movements of the user—specifically, slight head rotations and deliberate eye blinks—the HOIC system translates physical biomechanics into digital commands. 

The architecture is built upon a two-tiered input philosophy:
1. **Navigational Input:** Using an MPU6050 Inertial Measurement Unit (IMU), the system continuously polls the angular velocity and acceleration of the user's head. By applying digital low-pass filtering and deadzone thresholds, the device maps intuitive head movements (pitching up/down, yawing left/right) directly to a cursor navigating a custom grid-based dashboard.
2. **Selection Input:** An infrared obstacle avoidance sensor is focused precisely on the outer region of the user's eye. It continuously monitors the localized reflection of infrared light. When the user performs a deliberate, timed double-blink, the change in IR absorption triggers a localized state machine in the microcontroller, generating a precise "Click" command without requiring any physical force.

Together, these inputs feed into an ESP32 microcontroller, which acts as a local WebSockets server, transmitting commands with zero perceivable latency to a locally hosted web application.

**Key features:**
* **Hands-Free, Intuitive Navigation:** Translates raw inertial data into smooth, predictable UI cursor movements.
* **Optical Blink-to-Click:** Highly responsive infrared selection mechanism immune to ambient lighting interference.
* **Zero-Latency Local Network:** Utilizes WebSockets and mDNS to operate entirely independent of active internet connections, ensuring absolute reliability.
* **Text-to-Speech Engine:** Dynamically generates audible speech based on user selections for seamless communication with caregivers.
* **Emergency Override Protocol:** Features instant visual red-alerts and localized hardware/software buzzers to notify caregivers of immediate physical distress.
* **Non-Invasive Wearability:** Featherweight component stack designed to attach to everyday spectacles, eliminating user fatigue.

## Demo / Examples

### **Images**

<p align="center">
<img src="/assets/images/hoic-system/myosa-hoic-prototype.jpeg" width="800"><br/>
<i>The HOIC Wearable Prototype: The ESP32, MPU6050 IMU, and localized IR sensor mounted efficiently onto the sidearm of a standard glasses frame.</i>
</p>

<p align="center">
<img src="/assets/images/hoic-system/myosa-hoic-dashboard.jpg" width="800"><br/>
<i>The Custom Web Dashboard: A high-contrast, mathematically spaced grid UI specifically designed to reduce cognitive load and visual fatigue for paralyzed users.</i>
</p>

<p align="center">
<img src="/assets/images/hoic-system/myosa-hoic-blockdiagram.png" width="800"><br/>
<i>HOIC System Architecture and Data Flow: Demonstrating the pipeline from raw physical biomechanics to WebSocket transmission and final Text-to-Speech output.</i>
</p>

### **Videos**

<video controls width="100%">
<source src="/myosa-hoic-demo.mp4" type="video/mp4">
</video>

## Features (Detailed)

The HOIC system operates as a cohesive union between embedded hardware engineering, mathematical signal processing, and responsive front-end web development. Below is an exhaustive technical breakdown of each core subsystem.

### **1. Inertial Head-Tracking Engine (MPU6050 Integration)**
The navigational core of the HOIC relies on the MPU6050, a 6-axis MotionTracking device that combines a 3-axis gyroscope and a 3-axis accelerometer on a single silicon die. The sensor is mounted on the user's temple, meaning the local coordinate frame of the sensor perfectly aligns with the anatomical axes of the user's neck.

To translate raw physics into digital navigation, the ESP32 communicates with the MPU6050 via the $I^2C$ protocol at a 400kHz clock speed, polling raw 16-bit analog-to-digital (ADC) values. Relying solely on the accelerometer would introduce severe jitter due to minor bodily tremors, while relying solely on the gyroscope would introduce "drift" over time. Therefore, the embedded C++ code implements a mathematical complementary filter to fuse both sensor readings:

$$\theta_{current} = \alpha \times (\theta_{prev} + \omega \times dt) + (1 - \alpha) \times \theta_{accel}$$

Where $\alpha$ is a tuning constant (typically $0.98$), $\omega$ is the angular velocity from the gyroscope, and $\theta_{accel}$ is the absolute angle derived from the accelerometer. 

Once the precise pitch (up/down) and yaw (left/right) angles are calculated, the firmware applies a configurable "deadzone." This deadzone is a software-defined angular buffer (e.g., $\pm 10^\circ$) that allows the user to rest their head naturally without accidentally triggering a cursor movement. Once the head rotates past this threshold, the ESP32 fires a discrete direction command (`L`, `R`, `U`, `D`) over the WebSocket connection. 

### **2. Optical Infrared Blink Detection State Machine**
To solve the problem of selecting an item without a physical button, the HOIC utilizes an active infrared (IR) proximity sensor. The sensor consists of an IR Light Emitting Diode (LED) and an IR photodiode receiver. It is mounted slightly offset from the eye, pointing toward the eyelashes and upper eyelid.

When the eye is open, the infrared beam travels outward, and only a minimal baseline amount of light reflects back to the photodiode. However, when the user blinks, the highly reflective skin of the eyelid and the physical sweep of the eyelashes intersect the beam. This dramatically increases the backscattered IR light, causing the photodiode's resistance to drop and pushing the analog voltage past a tuned potentiometer threshold, outputting a digital `HIGH` signal to the ESP32.

To differentiate between an involuntary biological blink (which happens 15-20 times a minute) and a deliberate command blink, the ESP32 runs a highly optimized non-blocking state machine utilizing the `millis()` function. The logic looks for a specific pattern: a "double blink" within a strict temporal window. 

The algorithm functions as follows:
1. Detect first blink transition (LOW to HIGH). Record Timestamp 1.
2. Wait for eye to open (HIGH to LOW).
3. If a second blink transition occurs, record Timestamp 2.
4. Calculate $T_{\Delta} = Timestamp 2 - Timestamp 1$.
5. If $200ms < T_{\Delta} < 700ms$, register a deliberate "Click" (`C`) command. If outside this window, reset the state machine.

This temporal filtering ensures absolute accuracy and eliminates false positives from natural resting behavior.

### **3. Zero-Latency Local Area Network via WebSockets**
For an assistive device, latency is unacceptable. Traditional HTTP REST APIs require the client to constantly poll the server, opening and closing TCP connections repeatedly, which causes delays and massive overhead. 

Instead, the HOIC system boots the ESP32 in Station Mode (STA), connecting to a local Wi-Fi router. It immediately spins up an asynchronous WebSocket server on port 81. WebSockets provide a full-duplex, persistent, bi-directional communication channel over a single TCP connection. When a head movement or blink occurs, the exact command string is pushed to the client in under 5 milliseconds.

Furthermore, to ensure the user does not need to memorize IP addresses, the firmware implements Multicast DNS (mDNS). By broadcasting `hoic.local` across the network, any device on the same Wi-Fi can resolve the ESP32's IP address dynamically, making the setup entirely plug-and-play for caregivers.

### **4. High-Contrast User Interface and Cognitive UX**
The frontend of the HOIC system is a bespoke web application written in pure HTML, CSS, and vanilla JavaScript, hosted on the caregiver's laptop, tablet, or smart TV. The UI is fundamentally designed around the psychological and visual needs of paralyzed patients. 

It features a strict grid layout, large typography, and high-contrast color palettes (stark whites, deep blacks, and vibrant focus outlines). When the user navigates using their head, a JavaScript event listener captures the WebSocket payload and dynamically shifts an active CSS class across the grid elements, providing immediate visual feedback.

When the user executes a double-blink, the UI captures the selected cell and reads the text payload. It then utilizes the browser's native **Web Speech API** (`window.speechSynthesis`). This API synthesizes human-sounding speech from the text, vocalizing the user's needs—such as "I am thirsty," "Please adjust my bed," or "I want to watch TV"—at an adjustable volume and rate.

### **5. Critical Emergency Override System**
Patient safety is the paramount directive of the HOIC system. Embedded directly into the UI are specific, high-priority emergency nodes (e.g., "PAIN", "CHOKING", "HELP"). 

If a user navigates to and selects an emergency node, the system initiates a dual-layered alarm protocol:
1. **Software Alarm:** The web interface immediately flashes the entire screen stark red to grab the visual attention of anyone in the room. Concurrently, it bypasses standard text-to-speech and utilizes the HTML5 `AudioContext` to generate a piercing, continuous high-frequency digital square wave through the device's main speakers.
2. **Hardware Alarm:** Simultaneously, the web app fires a reverse WebSocket command back to the ESP32. Upon receiving the `ALARM` payload, the ESP32 triggers a GPIO pin connected to a 5V active piezo buzzer physically located on the patient's bedside hardware unit, ensuring that an alert sounds even if the laptop volume was accidentally muted.

## Usage Instructions

The HOIC system is designed to be highly accessible for caregivers with minimal technical training. Follow these exact steps to initialize and operate the device:

**Phase 1: Hardware Mounting and Power-Up**
1. Take the HOIC glasses frame and gently place them onto the user's head. Ensure the glasses sit level on the bridge of the nose.
2. Adjust the semi-rigid wire holding the IR sensor. The IR diode should be positioned approximately 1.5 to 2 centimeters away from the outer corner of the user's dominant eye, angled slightly inward.
3. Connect the provided USB-C power cable from the ESP32 to a stable 5V, 2A USB power bank. 
4. Wait approximately 10 seconds. The onboard blue LED on the ESP32 will flash rapidly as it attempts to connect to the predefined local Wi-Fi network, and will turn solid blue upon successful connection.

**Phase 2: Dashboard Initialization**
1. Ensure the display device (Laptop, iPad, or Smart TV) is connected to the same Wi-Fi network as the ESP32.
2. Open a modern, WebSockets-compatible web browser (Google Chrome, Microsoft Edge, or Mozilla Firefox are highly recommended).
3. Navigate to the local dashboard file by opening `index.html` from the provided `/src` directory.
4. Upon loading, the browser will automatically establish a WebSocket handshake with the ESP32 via the `ws://` protocol. A green indicator in the top right corner of the UI will confirm "Connected".

**Phase 3: User Navigation Guide**
* **To Move Cursor:** The user must smoothly tilt their head in the desired direction. Tilting the chin down moves the cursor DOWN. Tilting the chin up moves the cursor UP. Turning the head to the left/right moves the cursor correspondingly. The user must return their head to a neutral, center position after each step to reset the sensor deadzone.
* **To Select an Option:** Once the visual highlight is hovering over the desired phrase, the user must execute two deliberate blinks in rapid succession (roughly the speed of a mouse double-click). 
* **Audio Feedback:** The system will announce the selection clearly.
* **Emergency Reset:** If the user triggers an emergency alarm, the caregiver must manually click the "Reset Alarm" button on the UI to silence the piezo buzzer and stop the screen flashing.

*Note for Caregivers: To assist with testing or in the event of sensor recalibration, the web interface supports direct keyboard fallbacks. You may use the standard `Arrow Keys` to navigate the grid and the `Enter` key to simulate a blink selection.*

## Tech Stack

The architecture of the HOIC bridges multiple engineering disciplines, utilizing a robust stack of modern hardware and software technologies.

**Hardware Layer**
* **Microcontroller:** Espressif ESP32-WROOM-32D (Dual-core 240MHz, embedded 802.11 b/g/n Wi-Fi, 520 KB SRAM). Chosen for its immense processing overhead, allowing for real-time mathematical filtering while simultaneously maintaining a continuous network stack.
* **Inertial Sensor:** InvenSense MPU6050 (3-Axis Gyroscope + 3-Axis Accelerometer). Interfaced via I2C.
* **Optical Sensor:** Standard LM393 based Infrared Obstacle Avoidance Module (tuned via onboard potentiometer for proximity thresholding).
* **Alert Mechanism:** 5V Active Piezoelectric Buzzer driven by a general-purpose NPN transistor.
* **Power Supply:** 5V Lithium-Ion USB Power Bank with stable 2A output to support Wi-Fi transmission spikes.

**Software & Connectivity Layer**
* **Embedded Firmware:** C++ written via the Arduino Core. Utilizes hardware timers, interrupts, and advanced struct-based state machines.
* **Networking Protocol:** WebSockets (RFC 6455) for persistent, low-latency, full-duplex binary and text transmission.
* **Frontend Markup & Styling:** Semantic HTML5 and CSS3, utilizing CSS Grid and Flexbox for highly responsive, scalable interfaces that adapt to any screen size.
* **Frontend Logic:** Vanilla JavaScript (ES6+).
* **Browser APIs:** Web Speech API for organic Text-to-Speech synthesis and AudioContext API for generated emergency waveforms.

## Requirements / Installation


**Hardware Requirements List:**
* 1x ESP32 Development Board (38-pin configuration preferred)
* 1x MPU6050 Breakout Board
* 1x IR Proximity Sensor (LM393 comparator type)
* 1x Active Buzzer module
* Connecting jumper wires (Dupont female-to-female)
* 1x Wearable chassis (Standard eyeglasses frame or 3D printed equivalent)

**Software Setup & Compilation:**
1. Download and install the latest version of the **Arduino IDE** (v2.0 or higher recommended).
2. Install the ESP32 Board Manager by adding the official Espressif JSON URL to your IDE preferences, and install the `esp32` package.
3. Open the Arduino Library Manager and install the following exact dependencies:
   * `Adafruit MPU6050` by Adafruit
   * `Adafruit Unified Sensor` by Adafruit
   * `WebSockets` by Markus Sattler (Critical for async server management)
4. Open the `hoic_master.ino` file provided in the repository.
5. **CRITICAL STEP:** Locate the configuration block at the top of the file and replace the `ssid` and `password` variables with the exact credentials of the local Wi-Fi router you intend to use.
6. Connect the ESP32 via USB, select the correct COM port and Board ("ESP32 Dev Module"), and click "Upload".

## Team
**The Alchemists, KPR Institute of Engineering and technology**

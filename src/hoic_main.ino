#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// --- WI-FI SETTINGS ---
const char* ssid = ""//enter your Wifi SSID; 
const char* password = ""//enter your Wifi Password ; 

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
Adafruit_MPU6050 mpu;

// --- PIN DEFINITIONS ---
#define IR_BLINK_PIN 27
#define BUZZER_PIN 25 // Buzzer Pin

// --- IR BLINK SENSOR SETTINGS ---
const unsigned long DOUBLE_BLINK_WINDOW = 600; 
const int DEBOUNCE_DELAY = 30; 
bool isEyeClosed = false;
unsigned long lastBlinkTime = 0;
int blinkCount = 0;

// --- MPU6050 SENSOR TUNING ---
const float ANGLE_THRESHOLD = 15.0; 
const float NEUTRAL_ZONE = 5.0;     
float yaw_angle = 0.0;
float pitch_angle = 0.0;
unsigned long lastTime = 0;

enum HeadState { NEUTRAL, LOOKING_SIDE, LOOKING_VERT };
HeadState h_state = NEUTRAL;
HeadState v_state = NEUTRAL;

// --- WEBSOCKET EVENT LISTENER ---
// Listens for messages sent FROM the webpage TO the ESP32
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    String msg = (char*)payload;
    if (msg == "BUZZ_ON") {
      digitalWrite(BUZZER_PIN, HIGH);
      Serial.println("Received: ALARM ON");
    } else if (msg == "BUZZ_OFF") {
      digitalWrite(BUZZER_PIN, LOW);
      Serial.println("Received: ALARM OFF");
    }
  }
}

// --- THE WEBPAGE (HTML + CSS + JS) ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>HOIC Wireless Board</title>
    <style>
        body { font-family: Arial, sans-serif; display: flex; flex-direction: column; align-items: center; background: #f4f4f9; padding: 20px; margin: 0; }
        #status { font-weight: bold; color: red; margin-bottom: 20px; font-size: 20px; }
        #board { display: grid; grid-template-columns: repeat(3, 1fr); gap: 15px; width: 90%; max-width: 800px; }
        .cell { background: #fff; border: 2px solid #ccc; border-radius: 8px; padding: 40px 10px; text-align: center; font-size: 24px; font-weight: bold; transition: 0.2s; }
        .active { background: #007bff; color: #fff; border-color: #0056b3; transform: scale(1.05); }
        
        #fullscreen-view {
            display: none; 
            position: fixed;
            top: 0; left: 0; width: 100%; height: 100%;
            background: #007bff; color: white;
            justify-content: center; align-items: center;
            font-size: 10vw; 
            font-weight: bold; z-index: 1000; text-align: center;
        }
        /* Make emergency screens red for visual urgency */
        .emergency-bg { background: #dc3545 !important; }
    </style>
</head>
<body>
    <div id="status">Connecting to HOIC...</div>
    <div id="board">
        <div class="cell active">Yes</div><div class="cell">No</div><div class="cell">Help</div>
        <div class="cell">Water</div><div class="cell">Food</div><div class="cell">Restroom</div>
        <div class="cell">Pain</div><div class="cell">Adjust Bed</div><div class="cell">Thanks</div>
    </div>

    <div id="fullscreen-view"></div>

    <script>
        const cells = document.querySelectorAll('.cell');
        let index = 0;
        let isFullscreen = false; 
        
        function updateCursor(newIndex) {
            cells[index].classList.remove('active');
            index = newIndex;
            if (index < 0) index = cells.length - 1;
            if (index >= cells.length) index = 0;
            cells[index].classList.add('active');
        }

        var gateway = `ws://${window.location.hostname}:81/`;
        var websocket;
        
        function initWebSocket() {
            websocket = new WebSocket(gateway);
            websocket.onopen = () => {
                document.getElementById('status').innerText = "HOIC Connected";
                document.getElementById('status').style.color = "green";
            };
            websocket.onclose = () => {
                document.getElementById('status').innerText = "Disconnected";
                document.getElementById('status').style.color = "red";
                setTimeout(initWebSocket, 2000);
            };
            websocket.onmessage = (event) => {
                let cmd = event.data;
                
                if (isFullscreen && cmd !== 'C') return;

                if (cmd === 'L') updateCursor(index - 1);
                if (cmd === 'R') updateCursor(index + 1);
                if (cmd === 'U') updateCursor(index - 3); 
                if (cmd === 'D') updateCursor(index + 3); 
                
                if (cmd === 'C') {
                    if (!isFullscreen) {
                        isFullscreen = true;
                        const selectedText = cells[index].innerText;
                        const fsView = document.getElementById('fullscreen-view');
                        
                        // 1. Show Fullscreen UI
                        fsView.innerText = selectedText;
                        fsView.style.display = 'flex';
                        
                        // 2. Trigger hardware buzzer and red UI for emergencies
                        if (selectedText === "Help" || selectedText === "Pain") {
                            websocket.send("BUZZ_ON");
                            fsView.classList.add("emergency-bg");
                        } else {
                            fsView.classList.remove("emergency-bg");
                        }
                        
                        // 3. Trigger Text-to-Speech (Audio)
                        window.speechSynthesis.cancel(); // Stop any currently playing audio
                        let msg = new SpeechSynthesisUtterance(selectedText);
                        msg.rate = 0.9; // Slightly slower for clearer pronunciation 
                        window.speechSynthesis.speak(msg);

                    } else {
                        // Exit Fullscreen and stop buzzer
                        isFullscreen = false;
                        document.getElementById('fullscreen-view').style.display = 'none';
                        websocket.send("BUZZ_OFF");
                        window.speechSynthesis.cancel(); // Silence audio if it's still playing
                    }
                }
            };
        }
        window.addEventListener('load', initWebSocket);
    </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(9600);
  delay(1000); 

  pinMode(IR_BLINK_PIN, INPUT_PULLUP);
  
  // Initialize Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Ensure it starts quiet

  Serial.println("\n--- BOOTING HOIC SYSTEM ---");
  
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi Connected Successfully!");
  Serial.print("Open your laptop/phone browser to: http://");
  Serial.println(WiFi.localIP()); 

  server.on("/", []() {
    server.send(200, "text/html", index_html);
  });
  server.begin();
  
  // Start WebSocket and attach the receiving event listener
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  Serial.println("Web Server and WebSockets running.");

  Serial.println("Connecting to MPU6050...");
  Wire.begin(21, 22);
  
  if (!mpu.begin()) { 
    Serial.println("CRITICAL ERROR: MPU6050 Failed at 0x69");
    while (1) { delay(10); } 
  }
  
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); 
  Serial.println("MPU6050 Connected. System Ready.");

  lastTime = millis();
}

void loop() {
  webSocket.loop(); 
  server.handleClient(); 

  unsigned long currentTime = millis();

  // 1. IR DOUBLE BLINK DETECTION
  int irState = digitalRead(IR_BLINK_PIN);

  if (irState == LOW && !isEyeClosed) {
    delay(DEBOUNCE_DELAY); 
    if (digitalRead(IR_BLINK_PIN) == LOW) { 
      isEyeClosed = true;

      if (currentTime - lastBlinkTime <= DOUBLE_BLINK_WINDOW) {
        blinkCount++;
      } else {
        blinkCount = 1;
      }
      lastBlinkTime = currentTime;

      if (blinkCount == 2) {
        webSocket.broadcastTXT("C"); 
        Serial.println("Sent: CLICK (Double Blink)");
        blinkCount = 0; 
      }
    }
  }

  if (irState == HIGH && isEyeClosed) {
    delay(DEBOUNCE_DELAY);
    if (digitalRead(IR_BLINK_PIN) == HIGH) {
      isEyeClosed = false;
    }
  }

  if (blinkCount == 1 && (currentTime - lastBlinkTime > DOUBLE_BLINK_WINDOW)) {
    blinkCount = 0;
  }

  // 2. MPU6050 HEAD TRACKING (INVERTED MOUNT)
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float dt = (currentTime - lastTime) / 1000.0; 
  lastTime = currentTime;

  float z_speed_deg = g.gyro.z * 57.2958; 
  float x_speed_deg = g.gyro.x * 57.2958; 

  pitch_angle += (z_speed_deg * dt);
  yaw_angle += (x_speed_deg * dt);
  
  pitch_angle *= 0.98;
  yaw_angle *= 0.98;

  if (h_state == NEUTRAL) {
    if (yaw_angle > ANGLE_THRESHOLD) {
      webSocket.broadcastTXT("L");
      Serial.println("Sent: LEFT");
      h_state = LOOKING_SIDE; 
    } else if (yaw_angle < -ANGLE_THRESHOLD) {
      webSocket.broadcastTXT("R");
      Serial.println("Sent: RIGHT");
      h_state = LOOKING_SIDE; 
    }
  } else if (abs(yaw_angle) < NEUTRAL_ZONE) {
    h_state = NEUTRAL; 
  }

  if (v_state == NEUTRAL) {
    if (pitch_angle > ANGLE_THRESHOLD) {
      webSocket.broadcastTXT("U");
      Serial.println("Sent: UP");
      v_state = LOOKING_VERT;
    } else if (pitch_angle < -ANGLE_THRESHOLD) {
      webSocket.broadcastTXT("D");
      Serial.println("Sent: DOWN");
      v_state = LOOKING_VERT;
    }
  } else if (abs(pitch_angle) < NEUTRAL_ZONE) {
    v_state = NEUTRAL;
  }

  delay(20);
}

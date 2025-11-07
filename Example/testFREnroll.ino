#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "SSID";
const char* password = "PSWD";

WebServer server(80);

bool enrollRequested = false;
unsigned long enrollTriggeredAt = 0;
const unsigned long ENROLL_COOLDOWN = 10000;

const char html_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Face Enroll</title>
  <style>
    body { font-family: sans-serif; text-align:center; margin-top:50px; }
    button {
      font-size: 1.5em;
      padding: 10px 25px;
      border-radius: 10px;
      background-color: #4CAF50;
      color: white;
      border: none;
      cursor: pointer;
    }
    button:hover { background-color: #45a049; }
  </style>
</head>
<body>
  <h2>Face Enroll</h2>
  <button onclick="fetch('/enroll').then(r=>r.text()).then(alert)">ENROLL</button>
  <p style='margin-top:40px;color:gray;'>© 2025 LUNGMEN ELECTRONICS</p>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", html_page);
}

void handleEnroll() {
  unsigned long now = millis();

  if (enrollTriggeredAt > 0 && now - enrollTriggeredAt < ENROLL_COOLDOWN) {
    server.send(200, "text/plain", "Please wait!");
    return;
  }

  enrollRequested = true;
  enrollTriggeredAt = now;
  server.send(200, "text/plain", "Command Sent!");
}

void handleCmd() {
  unsigned long now = millis();

  if (enrollTriggeredAt > 0 && now - enrollTriggeredAt < ENROLL_COOLDOWN) {
    if (server.hasArg("eat")) {
      server.send(200, "text/plain", "OK");
      return;
    }

    server.send(200, "text/plain", "OK");
    return;
  }

  if (enrollRequested) {
    enrollRequested = false;
    enrollTriggeredAt = now;
    Serial.println("[ESP32] Sent ENROLL Command to K210");
    server.send(200, "text/plain", "ENROLL");
    return;
  }

  if (server.hasArg("eat")) {
    enrollTriggeredAt = now;
    Serial.println("[ESP32] Receive From K210 eat=1");
    server.send(200, "text/plain", "OK");
    return;
  }

  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("[WiFi] Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("[WiFi] Connected!");
  Serial.print("[WiFi] IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/enroll", handleEnroll);
  server.on("/cmd", handleCmd);

  server.begin();
  Serial.println("[HTTP] Webserver startup!");
  Serial.println("[HTTP] http://" + WiFi.localIP().toString());
}

void loop() {
  server.handleClient();
  delay(1);
}

// LUNGMEN ELECTRONICS 2025
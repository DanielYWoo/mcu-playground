#include <ESP_EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define PIN_SHCP D0 // SHCP
#define PIN_STCP D1 // STCP
#define PIN_DATA D2 // DS
#define PIN_BUZZER D5
#define PIN_VOLUME A0
#define VOLUME_SAMPLES 30
#define VOLUME_NORMAL 585
#define ALERT_DURATION 1500

ESP8266WebServer server(80);
boolean inMeeting = false;
float volumeThreshold = 2.0;

void handleRoot() {  
  String str = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1, minimum-scale=1, maximum-scale=1'>"
    "<style>body, label, button, input {font-size: 1.2em; padding:0.5em;} .selected {background-color: green;}</style></head><body><h1>Meeting Noise Alerter</h1>"
    "<form action='meeting'><label>Meeting Status:";
  str += inMeeting ? "on" : "off";
  str += "</label><button type='submit' name='meeting' value='on' ";
  str += inMeeting ? "class='selected'" : "";
  str += ">ON</button><button type='submit' name='meeting' value='off' ";
  str += !inMeeting ? "class='selected'" : "";
  str += ">OFF</button></form><form action='settings'><label>Noise Threshold</label><input name='threshold' width='5em;' step='0.1' type='number' value='";
  str += volumeThreshold;
  str += "'/><button type='submit'>Set</button></form></body></html>";  
  server.send(200, "text/html", str);  
}


void setup() {
  Serial.begin(115200);
  pinMode(PIN_STCP, OUTPUT);
  pinMode(PIN_SHCP, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_VOLUME, INPUT);  
  delay(1000); // avoid noise of Serial port  
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.begin("DNH", "danielnorah");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  Serial.println("");
  Serial.println("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Load Settings...");
  EEPROM.begin(16); // simulate EEPROM with 16 bytes from the flash
  EEPROM.get(0, volumeThreshold);
  if (volumeThreshold <= 0.0001) volumeThreshold = 2.0; // default value  
  Serial.println("Start Web Server...");
  server.on("/", handleRoot);
  server.on("/meeting", []() {
    String meeting = server.arg("meeting");    
    inMeeting = (meeting == "on");    
    Serial.printf("set meeting status: %d\n", inMeeting);    
    handleRoot();
  });  
  server.on("/settings", []() {
    String threshold = server.arg("threshold");
    Serial.printf("set threshold to %f\n", threshold);    
    volumeThreshold = threshold.toFloat();    
    EEPROM.put(0, volumeThreshold);
    EEPROM.commit();
    handleRoot();  
  });
  server.begin();
  Serial.println("Web Server started");  
}

int lastSamples[VOLUME_SAMPLES] = {VOLUME_NORMAL};
long lastSampleTime = millis();
long lastAlert = 0;
void loop() {
  server.handleClient();
  
  if (inMeeting) {
    setCross(true);
  } else {
    setCross(false);
  }

  if (tooNoisy() && inMeeting) {
    lastAlert = millis();    
    tone(PIN_BUZZER, 400, ALERT_DURATION);
  }
  
}

boolean tooNoisy() {
  long now = millis();
  if (now - lastSampleTime < 20) return false; // check interval with 20ms
  
  int v = analogRead(PIN_VOLUME);
  lastSampleTime = now;
  if (v > 1000 || v < 100 || now - lastAlert < ALERT_DURATION) { // incorrect, or in alert
    v = VOLUME_NORMAL;
  }
  
  double error = 0;
  for (int i = 0; i < VOLUME_SAMPLES; i++) {
    if (i != VOLUME_SAMPLES - 1) {
      lastSamples[i] = lastSamples[i+1];
    } else {      
      lastSamples[i] = v;
    }
    error += pow(lastSamples[i] - VOLUME_NORMAL, 2);
  }
  error /= VOLUME_SAMPLES;
  Serial.printf("error=%f\n", error);
  if (error > volumeThreshold) {
    Serial.println("too noisy!");
    return true;
  } else {
    return false;
  }
}

#define PINS 0B0011010001101011
const int rowToPin[] = {0, 9, 14, 8, 12, 1,  7,  2, 5};
const int colToPin[] = {0, 13, 3, 4, 10, 6, 11, 15, 16};

void setLED(int row, int col) {
  int bits = PINS;
  if (row > 0 && col > 0) { // otherwise turn all LEDs off
    int rowPin = rowToPin[row];
    int colPin = colToPin[col];
    bits = (PINS | (0B1000000000000000 >> (rowPin-1))) & ~(0B1000000000000000 >> (colPin-1));  
  }
  digitalWrite(PIN_STCP, LOW);
  shiftOut(PIN_DATA, PIN_SHCP, LSBFIRST, highByte(bits)); // pins 1-8
  shiftOut(PIN_DATA, PIN_SHCP, MSBFIRST, lowByte(bits));  // pins 9-16
  digitalWrite(PIN_STCP, HIGH);
}

void setCross(boolean show) {
  if (show) {
    for (int i = 1; i <= 8; i++) {    
      setLED(i, i);    
    }
    for (int i = 1; i <= 8; i++) {    
      setLED(9-i, i);    
    }
  } else {
    setLED(0, 0);
  }
}

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define clockPin D0
#define latchPin D1
#define dataPin D2

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", "<form action=\"/LED_BUILTIN_on\" method=\"get\" id=\"form1\"></form><button type=\"submit\" form=\"form1\" value=\"On\">On</button><form action=\"/LED_BUILTIN_off\" method=\"get\" id=\"form2\"></form><button type=\"submit\" form=\"form2\" value=\"Off\">Off</button>");
}

void handleSave() {
  if (server.arg("pass") != "") {
    Serial.println(server.arg("pass"));
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  /*
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println(LED_BUILTIN); 
  delay(3000);
  
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.begin("DNH", "danielnorah");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.on ( "/", handleRoot );
  server.on ("/save", handleSave);
  server.begin();
  Serial.println ( "HTTP server started" );
  server.on("/LED_BUILTIN_on", []() {
    digitalWrite(LED_BUILTIN, 1);
    Serial.println("on");
    handleRoot();
  });
  server.on("/LED_BUILTIN_off", []() {
    digitalWrite(LED_BUILTIN, 0);
    Serial.println("off");
    handleRoot();
  });
  */
}

void loop() {
  
  
  
  digitalWrite(latchPin, LOW);
  //shiftOut(dataPin, clockPin, MSBFIRST, 0x01);
  //shiftOut(dataPin, clockPin, MSBFIRST, 0x00);
  shiftOut(dataPin, clockPin, LSBFIRST, 0x01);
  shiftOut(dataPin, clockPin, LSBFIRST, 0x00);
  digitalWrite(latchPin, HIGH);
  delay(3000);
  
  
  //server.handleClient();
} 

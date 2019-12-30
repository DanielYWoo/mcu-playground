/**
   remote control with a Nano or Pro Mini

*/

#include <LiquidCrystal.h>

#include <printf.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>

const int PIN_JOYSTICK_V = A6;
const int PIN_JOYSTICK_H = A7;

const int PIN_1602_RS = 7;
const int PIN_1602_EN = 6;
const int PIN_BTN_1 = 5;
const int PIN_BTN_2 = 4;
const int PIN_BTN_3 = 3;
const int PIN_BTN_4 = 2;

const int PIN_1602_D4 = A3;
const int PIN_1602_D5 = A2;
const int PIN_1602_D6 = A1;
const int PIN_1602_D7 = A0;

const int RF24_PIN_IRQ = 8;
const int RF24_PIN_CE = 9;
const int RF24_PIN_CSN = 10;
const int RF24_PIN_MOSI = 11;
const int RF24_PIN_MISO = 12;
const int RF24_PIN_SCK = 13;

LiquidCrystal lcd(PIN_1602_RS, PIN_1602_EN, PIN_1602_D4, PIN_1602_D5, PIN_1602_D6, PIN_1602_D7); // create an LCD object
RF24 radio(RF24_PIN_CE, RF24_PIN_CSN); // create a radio object
const byte RF24_ADDR[6] = "00001";
unsigned long lastLcdDisplayMs = 0;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BTN_1, INPUT_PULLUP);
  pinMode(PIN_BTN_2, INPUT_PULLUP);
  pinMode(PIN_BTN_3, INPUT_PULLUP);
  pinMode(PIN_BTN_4, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Mode: Manual");

  radio.begin();
  radio.openWritingPipe(RF24_ADDR);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);   //set datarate to 250kbps
  radio.setChannel(100);             //set frequency to channel 100
  radio.startListening();
}

void loop() {
  receiveTelemetry();
  checkJoystickButton();
  //delay(1000);
  //sendCommand();
}

void checkJoystickButton() {
  //delay(1000);
  //int h = (analogRead(PIN_JOYSTICK_H) - 512) / 32;
  //int v = (analogRead(PIN_JOYSTICK_V) - 512) / 32;
  int h = analogRead(PIN_JOYSTICK_H);
  int v = analogRead(PIN_JOYSTICK_V);
  //lcd.setCursor(0, 0);
  //lcd.print("                ");

  if (millis() - lastLcdDisplayMs > 200) { // don't refresh debug info too frequently
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("DebugJS=");
    lcd.print(h);
    lcd.print(",");
    lcd.print(v);
    lastLcdDisplayMs = millis();
  }
/*
  Serial.print(digitalRead(PIN_BTN_1));
  Serial.print(" ");
  Serial.print(digitalRead(PIN_BTN_2));
  Serial.print(" ");
  Serial.print(digitalRead(PIN_BTN_3));
  Serial.print(" ");
  Serial.print(digitalRead(PIN_BTN_4));
  Serial.println();
*/
}

void sendCommand() {
  radio.stopListening();
  //Serial.println("Sending command");
  const char text[] = "Hello World";
  radio.write(&text, sizeof(text));
  Serial.println("Sent");
  radio.startListening();
}

void receiveTelemetry() {
  if (radio.available()) {
    const char text[32];
    radio.read(&text, sizeof(text));
    Serial.print("Received telemetry data:");
    Serial.println(text);
  }
}

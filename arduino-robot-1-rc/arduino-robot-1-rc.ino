/**
   remote control with a Nano or Pro Mini

*/
// for 1602
#include <LiquidCrystal.h>
// for rf24
#include <printf.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>
// for kalman filter


const int PIN_JOYSTICK_V = A4;
const int PIN_JOYSTICK_H = A5;

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

const int MODE_MANUAL_PID = 0;
const int MODE_MANUAL_NOPID = 1;
const int MODE_AUTO_PID = 2;
const int MODE_DANCE_PID = 3;

byte runMode = 0;
unsigned long lastControlMs = 0;
int lastBtn1 = 0, lastBtn2 = 0, lastBtn3 = 0, lastBtn4 = 0;
int lastJoyStickH = 0, lastJoyStickV = 0;

unsigned long lastLCDMs = 0;

const int DEBUG_JOYSTICK = 0;
const int DEBUG_PID = 1;
const int DEBUG_4WAY_OBSTACLE_DETECTION = 2;
const int DEBUG_ULTRA_SONIC = 3;
const int DEBUG_RF24 = 4;
byte debugMode = 0;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BTN_1, INPUT_PULLUP);
  pinMode(PIN_BTN_2, INPUT_PULLUP);
  pinMode(PIN_BTN_3, INPUT_PULLUP);
  pinMode(PIN_BTN_4, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Booting...");

  radio.begin();
  radio.openWritingPipe(RF24_ADDR);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);   //set datarate to 250kbps
  radio.setChannel(100);             //set frequency to channel 100
  radio.startListening();
  setRunMode(MODE_MANUAL_PID);
  debugMode = DEBUG_JOYSTICK;
}

void loop() {
  debug();
  receiveTelemetry();
  checkJoystickButton();
  //sendCommand();
}

void debug() {
  if (millis() - lastLCDMs < 500) { //don't refresh debug info too frequently
    return;
  }
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  switch (debugMode) {
    case DEBUG_JOYSTICK:
      lcd.print("Stick=");
      lcd.print(lastJoyStickH);
      lcd.print(",");
      lcd.print(lastJoyStickV);
      break;
    case DEBUG_PID:
      lcd.print("PID=");
      break;
    case DEBUG_4WAY_OBSTACLE_DETECTION:
      lcd.print("4WAY=");
      break;
    case DEBUG_ULTRA_SONIC:
      lcd.print("Sonic=");
      break;
    case DEBUG_RF24:
      lcd.print("RF24=");
      break;
    default:
      lcd.print("ERROR");
      break;
  }
  lastLCDMs = millis();
}

void setRunMode(int m) {
  lcd.clear();
//  lcd.setCursor(0, 0);
//  lcd.print("                ");
  switch (m) {
    case MODE_MANUAL_PID:
      lcd.print("Mode: Manual PID");
      break;
    case MODE_MANUAL_NOPID:
      lcd.print("Mode: Manual");
      break;
    case MODE_AUTO_PID:
      lcd.print("Mode: AUTO PID");
      break;
    case MODE_DANCE_PID:
      lcd.print("Mode: Dance PID");
      break;
    default:
      lcd.print("ERROR");
      break;
  }
}

void checkJoystickButton() {
  if (millis() - lastControlMs > 150) { // debounce buttons
    int btn1 = digitalRead(PIN_BTN_1);
    if (lastBtn1 == HIGH && btn1 == LOW) { // avoid repeating while keeping the button pressed
      setRunMode(++runMode % 4);
    }
    lastBtn1 = btn1;

    int btn2 = digitalRead(PIN_BTN_2);
    if (lastBtn2 == HIGH && btn2 == LOW) { // avoid repeating while keeping the button pressed
      debugMode = ++debugMode % 5;
    }
    lastBtn2 = btn2;

    float h = analogRead(PIN_JOYSTICK_H); //165-835, per stick
    float v = analogRead(PIN_JOYSTICK_V); //215-730, per stick
    
    h = (h - 165) * 1.4925 - 498;
    v = (v - 215) * 1.9417 - 495;
    lastJoyStickH = (int) h;
    lastJoyStickV = (int) v;
    
    int hNoise = v * 0.375;
    int vNoise = h * 0.765;
    lastJoyStickH = (int) ((h - hNoise) / 100);
    lastJoyStickV = (int) ((v - vNoise) / 100);
    // now h v ranges from -3 to 3.
    lastControlMs = millis();
    
  }
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

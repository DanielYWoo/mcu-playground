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

// -------------- Pins -----------------
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

const int PIN_RF24_IRQ = 8;
const int PIN_RF24_CE = 9;
const int PIN_RF24_CSN = 10;
const int PIN_RF24_MOSI = 11;
const int PIN_RF24_MISO = 12;
const int PIN_RF24_SCK = 13;

// -------------- global objects -----------------
LiquidCrystal lcd(PIN_1602_RS, PIN_1602_EN, PIN_1602_D4, PIN_1602_D5, PIN_1602_D6, PIN_1602_D7); // create an LCD object
RF24 radio(PIN_RF24_CE, PIN_RF24_CSN); // create a radio object
const byte RF24_ADDR[6] = "00001";

// -------------- mode -----------------
const char CMD_MODE [] = "MODE";
const byte CMD_MODE_MANUAL_PID = 0;
const byte CMD_MODE_MANUAL_NOPID = 1;
const byte CMD_MODE_AUTO_PID = 2;
const byte CMD_MODE_DANCE_PID = 3;
byte cmdRunMode = 0;

// -------------- move -----------------
const char CMD_MOVE[] = "MOVE";
byte cmdMoveH = 0;
byte cmdMoveV = 0;

// -------------- debug -----------------
const byte CMD_DEBUG_JOYSTICK = 0;
const byte CMD_DEBUG_PID = 1;
const byte CMD_DEBUG_4WAY_OBSTACLE_DETECTION = 2;
const byte CMD_DEBUG_ULTRA_SONIC = 3;
const byte CMD_DEBUG_RF24 = 4;
byte cmdDebugMode = 0;

// -------------- controls vaiables -----------------
unsigned long lastControlMs = 0;
int lastBtn1 = 0, lastBtn2 = 0, lastBtn3 = 0, lastBtn4 = 0;
unsigned long lastLCDMs = 0;
bool enableSerial = false;

void setup() {
  if (enableSerial) Serial.begin(9600);
  pinMode(PIN_BTN_1, INPUT_PULLUP);
  pinMode(PIN_BTN_2, INPUT_PULLUP);
  pinMode(PIN_BTN_3, INPUT_PULLUP);
  pinMode(PIN_BTN_4, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Booting...");

  radio.begin();
  radio.openWritingPipe(RF24_ADDR);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);   //set datarate to 250kbps
  radio.setChannel(100);             //set frequency to channel 100
  radio.startListening();
  setRunMode(CMD_MODE_MANUAL_PID);
  cmdDebugMode = CMD_DEBUG_JOYSTICK;
}

void loop() {
  debug();
  receiveTelemetry();
  checkJoystickButton();
}

void debug() {
  if (millis() - lastLCDMs < 200) { //don't refresh debug info too frequently
    return;
  }
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  switch (cmdDebugMode) {
    case CMD_DEBUG_JOYSTICK:
      lcd.print("D: Stick=");
      lcd.print((int) cmdMoveH);
      lcd.print(",");
      lcd.print((int) cmdMoveV);
      break;
    case CMD_DEBUG_PID:
      lcd.print("D: PID=");
      break;
    case CMD_DEBUG_4WAY_OBSTACLE_DETECTION:
      lcd.print("D: 4WAY=");
      break;
    case CMD_DEBUG_ULTRA_SONIC:
      lcd.print("D: Sonic=");
      break;
    case CMD_DEBUG_RF24:
      lcd.print("D: RF24=");
      break;
    default:
      lcd.print("D: ERROR");
      break;
  }
  lastLCDMs = millis();
}

void setRunMode(int m) {
  lcd.clear();
  switch (m) {
    case CMD_MODE_MANUAL_PID:
      sendCommand(CMD_MODE,  CMD_MODE_MANUAL_PID, 0);
      lcd.print("M: Manual PID");
      break;
    case CMD_MODE_MANUAL_NOPID:
      sendCommand(CMD_MODE, CMD_MODE_MANUAL_NOPID, 0);
      lcd.print("M: Manual No PID");
      break;
    case CMD_MODE_AUTO_PID:
      sendCommand(CMD_MODE, CMD_MODE_AUTO_PID, 0);
      lcd.print("M: Auto PID");
      break;
    case CMD_MODE_DANCE_PID:
      sendCommand(CMD_MODE, CMD_MODE_DANCE_PID, 0);
      lcd.print("M: Dance PID");
      break;
    default:
      lcd.print("M: ERROR");
      break;
  }
}

void checkJoystickButton() {
  if (millis() - lastControlMs > 20) { // debounce buttons
    int btn1 = digitalRead(PIN_BTN_1);
    if (lastBtn1 == HIGH && btn1 == LOW) { // avoid repeating while keeping the button pressed
      setRunMode(++cmdRunMode % 4);
    }
    lastBtn1 = btn1;

    int btn2 = digitalRead(PIN_BTN_2);
    if (lastBtn2 == HIGH && btn2 == LOW) { // avoid repeating while keeping the button pressed
      cmdDebugMode = ++cmdDebugMode % 5;
    }
    lastBtn2 = btn2;

    float h = analogRead(PIN_JOYSTICK_H); //165-835, per stick
    float v = analogRead(PIN_JOYSTICK_V); //215-730, per stick

    h = (h - 165) * 1.4925 - 498;
    v = (v - 215) * 1.9417 - 495;

    int hNoise = v * 0.375;
    int vNoise = h * 0.765;
    h = (int) ((h - hNoise) / 100);
    v = (int) ((v - vNoise) / 100);
    // now h v ranges from -3 to 3.
    if (h < 0) {
      cmdMoveH = 1;
    } else if (h > 0) {
      cmdMoveH = 3;
    } else {
      cmdMoveH = 2;
    }
    if (v < 0) {
      cmdMoveV = 1;
    } else if (v > 0) {
      cmdMoveV = 3;
    } else {
      cmdMoveV = 2;
    }
    sendCommand(CMD_MOVE, cmdMoveH, cmdMoveV);
    lastControlMs = millis();
  }
}

void sendCommand(byte * cmd1, byte param1, byte param2) {
  radio.stopListening();
  byte cmd [7];
  for (int i = 0; i < 4; i++) {
    cmd[i] = cmd1[i];
  }
  cmd[4] = param1;
  cmd[5] = param2;
  cmd[6] = '\0';
  // better log
  if (enableSerial) {
    Serial.print("Send command: ");
    for (int i = 0; i < 4; i++) {
      Serial.print((char) cmd[i]);
    }
    Serial.print(':');
    Serial.print((byte) cmd[4]);
    Serial.print(':');
    Serial.print((byte) cmd[5]);
    Serial.println();
  }
  radio.write(cmd, 7); // fixed 4 bytes command, 2 bytes params, with a zero ending
  radio.startListening();
}

void receiveTelemetry() {
  if (radio.available()) {
    const char text[32];
    radio.read(&text, sizeof(text));
    if (enableSerial) {
      Serial.print("Received telemetry data:");
      Serial.println(text);
    }
  }
}

#include <Servo.h>

#include <printf.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>

/**
   3-wheel robot with a Uno
   three PWM pins, two for wheel with a timer, one for servo with another timer
   a 74HC595 is used for more outputs
*/

// 74HC595
const int PIN_OUT_DS = 0;
const int PIN_OUT_SHCP = 1;
const int PIN_OUT_STCP = 4;
byte hc595Bits = 0;

// mouth and heart
// 2 pins of 595 chip for the OLED screen
const int REG_MOUTH_SCL = 0;
const int REG_MOUTH_SDA = 1;

// wheels, including 4 pins of 595 chip
const int REG_WHEEL_LEFT_OUT1 = 2;
const int REG_WHEEL_LEFT_OUT2 = 3;
const int REG_WHEEL_RIGHT_OUT3 = 4;
const int REG_WHEEL_RIGHT_OUT4 = 5;
const int PIN_WHEEL_LEFT_PWM = 5;
const int PIN_WHEEL_RIGHT_PWM = 6;

// speed, counter
const int PIN_WHEEL_LEFT_COUNTER = 2; // must be 2/3 to use interrupt
const int PIN_WHEEL_RIGHT_COUNTER = 3; // must be 2/3 to use interrupt

// servo
const int PIN_SERVO_PWM = 9;

// RF2.4G wireless
const int PIN_RF24_CE = 7;
const int PIN_RF24_CSN = 10;
const int PIN_RF24_IRQ = 8; // fixed, cannot change
const int PIN_RF24_MOSI = 11; // fixed, cannot change
const int PIN_RF24_MISO = 12; // fixed, cannot change
const int PIN_RF24_SCK = 13; // fixed, cannot change

// ultra sonic
const int PIN_SRF05_TRIG = A0;
const int PIN_SRF05_ECHO = A1;

// 4-way LED obstacle detection
const int PIN_LED_1 = A0;
const int PIN_LED_2 = A1;
const int PIN_LED_3 = A2;
const int PIN_LED_4 = A3;

// horn
const int REG_HORN_OUT = 1;

// -------------- rf24 ---------------
RF24 radio(PIN_RF24_CE, PIN_RF24_CSN); // create a radio object
const byte RF24_ADDR_RC [6] = "00001";
const byte RF24_ADDR_ROBOT [6] = "00002";

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

// --------------- speed --------------
byte countWheelL = 0; // current
byte countWheelR = 0;
byte lastWheelSpeedL = 0; // last
byte lastWheelSpeedR = 0;

// -------------- horn -----------------
const char CMD_HORN[] = "HORN";
unsigned long lastHornMs = 0;
unsigned int hornDurationMs = 0;

// ------------ servo ----------------
Servo servo;
#define servoError 10
const char CMD_SERVO_TEST [] = "SERV";
byte servoDegree = 90;

// -------------- debug -----------------
const byte CMD_DEBUG [] = "DBUG";
const byte CMD_DEBUG_JOYSTICK = 0;
const byte CMD_DEBUG_WHEEL_COUNTER = 1;
const byte CMD_DEBUG_PID = 2;
const byte CMD_DEBUG_4WAY_OBSTACLE_DETECTION = 3;
const byte CMD_DEBUG_ULTRA_SONIC = 4;
const byte CMD_DEBUG_RF24 = 5;
byte cmdDebugMode = 0;

// ---- infra 4 way obstacle detection ----
bool infra4Way1 = false;
bool infra4Way2 = false;
bool infra4Way3 = false;
bool infra4Way4 = false;

// ------------- telemetry ----------------
const byte CMD_TELE_WHEEL_COUNTER [] = "TLWC";
const byte CMD_TELE_PID [] = "TLPI";
const byte CMD_TELE_4WAY_OBSTACLE_DETECTION [] = "TL4W";
const byte CMD_TELE_ULTRA_SONIC [] = "TLUS";
const byte CMD_TELE_RF24 [] = "TLRF";
const int telemetryIntervalMs = 200;
unsigned long lastTelemetryMs = 0;

bool enableSerial = false; // at runtime, must set to false to avoid Serial interfere

void setup() {
  if (enableSerial) Serial.begin(9600);
  pinMode(PIN_OUT_DS, OUTPUT);
  pinMode(PIN_OUT_SHCP, OUTPUT);
  pinMode(PIN_OUT_STCP, OUTPUT);
  pinMode(PIN_WHEEL_LEFT_PWM, OUTPUT);
  pinMode(PIN_WHEEL_RIGHT_PWM, OUTPUT);
  servo.attach(PIN_SERVO_PWM);
  servo.write(servoDegree - servoError);

  attachInterrupt(digitalPinToInterrupt(PIN_WHEEL_LEFT_COUNTER), countWheelLeft, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_WHEEL_RIGHT_COUNTER), countWheelRight, CHANGE);

  radio.begin();
  radio.openWritingPipe(RF24_ADDR_RC);
  radio.openReadingPipe(0, RF24_ADDR_ROBOT);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_1MBPS); // if signal is poor, switch to lower speed
  radio.setRetries(20, 2); // 20x250us=5ms
  radio.setChannel(117); // use higer channel to avoid interference with wifi
  radio.startListening();
  setRunMode(CMD_MODE_MANUAL_PID);
  setDebugMode(CMD_DEBUG_JOYSTICK);
}

void loop()
{
  receiveCommand();
  drive();
  sendTelemetry();
}

void countWheelLeft() {
  countWheelL++;
}

void countWheelRight() {
  countWheelR++;
}

void receiveCommand() {
  if (!radio.available()) return;
  const char cmd[32];
  radio.read(&cmd, sizeof(cmd));
  if (enableSerial && !(cmd[0] == 'M' && cmd[1] == 'O' && cmd[2] == 'V' && cmd[3] == 'E')) {
    Serial.print("Received command:");
    for (int i = 0; i < 32; i++)  {
      Serial.print((unsigned byte) cmd[i]);
      Serial.print(' ');
    }
    Serial.println("|");
  }
  //  setHorn(300);
  if (matchCmd(cmd, CMD_MODE)) { // change run mode
    setRunMode(cmd[4]);
    setHorn(300);
  } else if ((cmdRunMode == CMD_MODE_MANUAL_NOPID || cmdRunMode == CMD_MODE_MANUAL_PID) && matchCmd(cmd, CMD_MOVE)) { // move is special
    cmdMoveH = cmd[4];
    cmdMoveV = cmd[5];
  } else if (matchCmd(cmd, CMD_DEBUG)) {
    setDebugMode(cmd[4]);
    setHorn(300);
  } else if (matchCmd(cmd, CMD_HORN)) {
    setHorn(300);
  } else if (matchCmd(cmd, CMD_SERVO_TEST)) {
    servoDegree = (servoDegree + 45) % 225;
    servo.write(servoDegree - servoError); // write once, keep its PWM status
    setHorn(100);
  }

}

void sendTelemetry() {
  infra4Way1 = digitalRead(PIN_LED_1); //LOW if blocked
  infra4Way2 = digitalRead(PIN_LED_2);
  infra4Way3 = digitalRead(PIN_LED_3);
  infra4Way4 = digitalRead(PIN_LED_4);

  if (millis() - lastTelemetryMs < telemetryIntervalMs) return;

  lastWheelSpeedL = countWheelL;
  lastWheelSpeedR = countWheelR;

  switch (cmdDebugMode) {
    case CMD_DEBUG_PID:
      sendCommand(CMD_TELE_PID, 1, 1); // don't exceed 255, test
      break;
    case CMD_DEBUG_WHEEL_COUNTER:
      sendCommand(CMD_TELE_WHEEL_COUNTER, lastWheelSpeedL, lastWheelSpeedR); // don't exceed 255
      break;
    case CMD_DEBUG_4WAY_OBSTACLE_DETECTION:
      byte flags = 0;
      if (!infra4Way1) flags |= B00000001;
      if (!infra4Way2) flags |= B00000010;
      if (!infra4Way3) flags |= B00000100;
      if (!infra4Way4) flags |= B00001000;
      sendCommand(CMD_TELE_4WAY_OBSTACLE_DETECTION, flags, 0);
      break;
    case CMD_DEBUG_ULTRA_SONIC:
      sendCommand(CMD_TELE_4WAY_OBSTACLE_DETECTION, 3, 3); // don't exceed 255
      break;
    case CMD_DEBUG_RF24:
      sendCommand(CMD_TELE_RF24, 4, 4); // don't exceed 255
      break;
    default:
      break;
  }
  lastTelemetryMs = millis();
}

void sendCommand(byte * cmd1, byte param1, byte param2) {
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
  radio.stopListening();
  radio.write(cmd, 7); // fixed 4 bytes command, 2 bytes params, with a zero ending
  radio.startListening();
}

void setRunMode(int m) {
  switch (m) {
    case CMD_MODE_MANUAL_PID:
      if (enableSerial) Serial.println("Mode: Manual PID");
      break;
    case CMD_MODE_MANUAL_NOPID:
      if (enableSerial) Serial.println("Mode: Manual No PID");
      break;
    case CMD_MODE_AUTO_PID:
      if (enableSerial) Serial.println("Mode: Auto PID");
      break;
    case CMD_MODE_DANCE_PID:
      if (enableSerial) Serial.println("Mode: Dance PID");
      break;
    default:
      if (enableSerial) Serial.println("ERROR");
      return;
  }
  cmdRunMode = m;
}

void setHorn(unsigned int duration) {
  lastHornMs = millis();
  hornDurationMs = duration;
  if (enableSerial) Serial.println("Horn");
}

void setDebugMode(int m) {
  switch (m) {
    case CMD_DEBUG_JOYSTICK:
      if (enableSerial) Serial.println("Debug: Joystick");
      break;
    case CMD_DEBUG_WHEEL_COUNTER:
      if (enableSerial) Serial.println("Debug: Wheel Counter");
      break;
    case CMD_DEBUG_PID:
      if (enableSerial) Serial.println("Debug: PID");
      break;
    case CMD_DEBUG_4WAY_OBSTACLE_DETECTION:
      if (enableSerial) Serial.println("Debug: 4Way");
      break;
    case CMD_DEBUG_ULTRA_SONIC:
      if (enableSerial) Serial.println("Debug: Ultra Sonic");
      break;
    case CMD_DEBUG_RF24:
      if (enableSerial) Serial.println("Debug: RF24");
      break;
    default:
      if (enableSerial) Serial.println("ERROR");
      return;
  }
  cmdDebugMode = m;
}

void drive() {
  bool blocked = !infra4Way1 || !infra4Way2 || !infra4Way3 || !infra4Way4;
  if (blocked && cmdMoveV == 3) {
    setHorn(50);
    cmdMoveV = 2; // brake if going forward
  }

  if (cmdMoveH == 1) { // left
    analogWrite(PIN_WHEEL_LEFT_PWM, 100);
    analogWrite(PIN_WHEEL_RIGHT_PWM, 230);
  } else if (cmdMoveH == 3) { // right
    analogWrite(PIN_WHEEL_LEFT_PWM, 230);
    analogWrite(PIN_WHEEL_RIGHT_PWM, 100);
  } else { // straight
    analogWrite(PIN_WHEEL_LEFT_PWM, 230);
    analogWrite(PIN_WHEEL_RIGHT_PWM, 200);
  }

  if (cmdMoveV == 1) { // backward
    bitWrite(hc595Bits, REG_WHEEL_LEFT_OUT1, 0);
    bitWrite(hc595Bits, REG_WHEEL_LEFT_OUT2, 1);
    bitWrite(hc595Bits, REG_WHEEL_RIGHT_OUT3, 0);
    bitWrite(hc595Bits, REG_WHEEL_RIGHT_OUT4, 1);
  } else if (cmdMoveV == 3) { // forward
    bitWrite(hc595Bits, REG_WHEEL_LEFT_OUT1, 1);
    bitWrite(hc595Bits, REG_WHEEL_LEFT_OUT2, 0);
    bitWrite(hc595Bits, REG_WHEEL_RIGHT_OUT3, 1);
    bitWrite(hc595Bits, REG_WHEEL_RIGHT_OUT4, 0);
  } else { // brake
    bitWrite(hc595Bits, REG_WHEEL_LEFT_OUT1, 0);
    bitWrite(hc595Bits, REG_WHEEL_LEFT_OUT2, 0);
    bitWrite(hc595Bits, REG_WHEEL_RIGHT_OUT3, 0);
    bitWrite(hc595Bits, REG_WHEEL_RIGHT_OUT4, 0);
  }

  unsigned long now = millis();
  if (now - lastHornMs < hornDurationMs) {
    if (enableSerial) Serial.println("Horn ...");
    bitWrite(hc595Bits, REG_HORN_OUT, 1);
  } else {
    bitWrite(hc595Bits, REG_HORN_OUT, 0);
  }

  output595Bits();
}

bool matchCmd (const byte *p1, const byte *p2)
{
  for (int i = 0; i < 4; i++) {
    if (* p1++ != * p2++) {
      return false;
    }
  }
  return true;
}

void output595Bits() {
  digitalWrite(PIN_OUT_STCP, LOW);
  shiftOut(PIN_OUT_DS, PIN_OUT_SHCP, MSBFIRST, hc595Bits);
  digitalWrite(PIN_OUT_STCP, HIGH);
}

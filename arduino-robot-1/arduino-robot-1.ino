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
const int PIN_LED_1 = A2;
const int PIN_LED_2 = A3;
const int PIN_LED_3 = A4;
const int PIN_LED_4 = A5;


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


void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, RF24_ADDR);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);   //set datarate to 250kbps
  radio.setChannel(100);             //set frequency to channel 100
  radio.startListening();
  setRunMode(CMD_MODE_MANUAL_PID);
}

void loop()
{
  receiveCommand();

}

void receiveCommand() {
  if (!radio.available()) {
    return;
  }
  const char cmd[32];
  radio.read(&cmd, sizeof(cmd));
  Serial.print("Received command:");
  for (int i = 0; i < 32; i++)  {
    Serial.print((unsigned byte) cmd[i]);
    Serial.print(' ');
  }
  Serial.println("|");

  if (matchCmd(cmd, CMD_MODE)) { // run mode
    setRunMode(cmd[4]);
  } else if ((cmdRunMode == CMD_MODE_MANUAL_NOPID || cmdRunMode == CMD_MODE_MANUAL_PID) && matchCmd(cmd, CMD_MOVE)) { // move
    setMove(cmd[4], cmd[5]);
  } else {
    Serial.println("unknown command");
  }

}

void sendTelemetry() {
  //radio.stopListening();
  //Serial.print("Send telemetry: ");
  //Serial.println(cmd);
  //radio.write(&cmd, sizeof(cmd));
  //radio.startListening();
}


void setRunMode(int m) {
  switch (m) {
    case CMD_MODE_MANUAL_PID:
      Serial.println("Mode: Manual PID");
      break;
    case CMD_MODE_MANUAL_NOPID:
      Serial.println("Mode: Manual No PID");
      break;
    case CMD_MODE_AUTO_PID:
      Serial.println("Mode: Auto PID");
      break;
    case CMD_MODE_DANCE_PID:
      Serial.println("Mode: Dance PID");
      break;
    default:
      Serial.println("ERROR");
      break;
  }
}

void setMove(int h, int v) {


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

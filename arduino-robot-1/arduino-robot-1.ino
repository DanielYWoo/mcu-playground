#include <printf.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>

/**
 * 3-wheel robot with a Uno
 * three PWM pins, two for wheel with a timer, one for servo with another timer
 * a 74HC595 is used for more outputs
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
const int MODE_MANUAL_PID = 0;
const int MODE_MANUAL_NOPID = 1;
const int MODE_AUTO_PID = 2;
const int MODE_DANCE_PID = 3;
byte runMode = 0;


void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(RF24_ADDR);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);   //set datarate to 250kbps
  radio.setChannel(100);             //set frequency to channel 100
  radio.startListening();
  setRunMode(MODE_MANUAL_PID);
}

void loop()
{ 
  receiveCommand();

}

void receiveCommand() {
  if (radio.available()) {
    const char text[32];
    radio.read(&text, sizeof(text));
    Serial.print("Received command:");
    Serial.println(text);
  }
}

void sendTelemetry() {
  radio.stopListening();
  Serial.print("Send telemetry: ");
  //Serial.println(cmd);
  //radio.write(&cmd, sizeof(cmd));
  radio.startListening();
}


void setRunMode(int m) { 
  switch (m) {
    case MODE_MANUAL_PID:
      Serial.println("Mode: Manual PID");      
      break;
    case MODE_MANUAL_NOPID:      
      Serial.println("Mode: Manual No PID");
      break;
    case MODE_AUTO_PID:
      Serial.println("Mode: AUTO PID");
      break;
    case MODE_DANCE_PID:
      Serial.println("Mode: Dance PID");
      break;
    default:
      Serial.println("ERROR");
      break;
  }
}

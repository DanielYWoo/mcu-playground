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
const int PIN_OUT_DS = 4;
const int PIN_OUT_SHCP = 7;
const int PIN_OUT_STCP = 10;

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
const int REG_RF24_CE = 6;
const int REG_RF24_CSN = 7;
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



void setup() {
  Serial.begin(9600);
}

void loop()
{ 
  

}

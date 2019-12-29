/**
 * remote control with a Nano or Pro Mini
 * 
 */

#include <printf.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>

const int PIN_JOYSTICK_V = A0;
const int PIN_JOYSTICK_H = A1;

const int PIN_BTN_1 = 2;
const int PIN_BTN_2 = 3;
const int PIN_BTN_3 = 4;
const int PIN_BTN_4 = 5;

const int PIN_1602_RS = 6;
const int PIN_1602_EN = 7;
const int PIN_1602_D4 = A2;
const int PIN_1602_D5 = A3;
const int PIN_1602_D6 = A4;
const int PIN_1602_D7 = A5;

const int RF24_PIN_IRQ = 8;
const int RF24_PIN_CE = 9;
const int RF24_PIN_CSN = 10;
const int RF24_PIN_MOSI = 11;
const int RF24_PIN_MISO = 12;
const int RF24_PIN_SCK = 13;

void setup() {
  Serial.begin(9600);
}

void loop()
{ 
  

}

/**
 * 3-wheel robot with a Uno
 * three PWM pins, two for wheel with a timer, one for servo with another timer
 */

// 74HC595
const int PIN_OUT_DS = 0;
const int PIN_OUT_SHCP = 1;
const int PIN_OUT_STCP = 2;

// wheels
const int REG_WHEEL_LEFT_OUT1 = 0;
const int REG_WHEEL_LEFT_OUT2 = 1;
const int REG_WHEEL_RIGHT_OUT3 = 2;
const int REG_WHEEL_RIGHT_OUT4 = 3;
const int PIN_WHEEL_LEFT_PWM = 3
const int PIN_WHEEL_RIGHT_PWM = 5;

// speed, counter
const int PIN_WHEEL_LEFT_COUNTER = 4;
const int PIN_WHEEL_LEFT_COUNTER = 7;

// servo 
const int PIN_SERVO_PWM = 6;

// RF2.4G wireless
const int PIN_RF24_IRQ = 8;
const int PIN_RF24_CE = 9;
const int PIN_RF24_CSN = 10;
const int PIN_RF24_MOSI = 11;
const int PIN_RF24_MISO = 12;
const int PIN_RF24_SCK = 13;

// ultra sonic
const int PIN_SRF05_TRIG = A0;
const int PIN_SRF05_ECHO = A1;

// 4-way LED obstacle detection
const int REG_LED_1 = A2;
const int REG_LED_1 = A3;
const int REG_LED_1 = A4;
const int REG_LED_1 = A5;



void setup() {
  Serial.begin(9600);
}

void loop()
{ 
  

}

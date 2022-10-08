#include <Otto9.h>
#include <SoftwareSerial.h>
#include <U8g2lib.h>
#include <Wire.h> // software I2C for u8g

#define PIN_YL 2 //servo[0]  left leg
#define PIN_YR 3 //servo[1]  right leg
#define PIN_RL 4 //servo[2]  left foot
#define PIN_RR 5 //servo[3]  right foot
#define PIN_Buzzer   6
#define PIN_Trigger  8  //TRIGGER pin (8)
#define PIN_Echo     9  //ECHO pin (9)
#define PIN_SOFT_RX  10
#define PIN_SOFT_TX  11
#define PIN_BT_STATUS  12
#define PIN_MOUTH_DATA A0
#define PIN_MOUTH_CLOCK A1

// action
#define MOVE_SPEED 800
#define MOVE_ROAM 'e'
#define MOVE_STAND 'q'
#define MOVE_FORWARD 'w'
#define MOVE_BACKWARD 's'
#define MOVE_LEFT 'a'
#define MOVE_RIGHT 'd'

// mouth and emotion
#define MOUTH_HEIGHT 9
#define MOUTH_POSITION 14
#define MOUTH_WIDTH 128
#define EMOTION_NORMAL '1'
#define EMOTION_HAPPY '2'
#define EMOTION_UNHAPPY '3'
#define EMOTION_LICK '4'
#define EMOTION_SPEAK '5'


const bool beep = true;
SoftwareSerial BTSerial(PIN_SOFT_RX, PIN_SOFT_TX); // Arduino RX|TX, HC05 TX|RX
U8G2_SSD1306_128X32_UNIVISION_1_SW_I2C u8g2(U8G2_R0, PIN_MOUTH_CLOCK, PIN_MOUTH_DATA); // rotation, clock, data
Otto9 zhamao;
unsigned long lastHeartbeat = millis();
byte cmd = 0x00;
byte emotion = EMOTION_NORMAL;

// ------------------------- setup -----------------------
void setup()
{
  Serial.begin(9600);
  u8g2.begin();
  zhamao.init(PIN_YL, PIN_YR, PIN_RL, PIN_RR, true, A6, PIN_Buzzer, PIN_Trigger, PIN_Echo); //Set the servo pins and ultrasonic pins and Buzzer pin
  if (beep) zhamao.sing(S_connection); //Otto wake up!
  zhamao.home();
  Serial.println(F("Initialized, waiting for bluetooth connection"));
  ensureConnected(true);  
}

// ------------------------- main loop ------------------------

void loop()
{
  ensureConnected(false);
  
  //debugDistance();
  if (!BTSerial.available()) return;
  cmd = BTSerial.read();
  if (cmd == 255) return; // no data
  Serial.print(F("incoming byte:"));
  Serial.println(cmd);

  switch (cmd) {
    case '?':
      echo(F("Welcome, ? -> help, q -> stand, asdw -> move, e -> roam"));
      break;
    case MOVE_STAND:
      echo(F(">Stand"));
      zhamao.home();
      break;
    case MOVE_FORWARD:
      echo(F(">Move Forward"));
      zhamao.walk(4, MOVE_SPEED, 1); //4 steps, "TIME". IF HIGHER THE VALUE THEN SLOWER (from 600 to 1400), 1 FORWARD
      zhamao.home();
      break;
    case MOVE_BACKWARD:
      echo(F(">Move Backward"));
      zhamao.walk(4, MOVE_SPEED, -1); //4 steps, "TIME". IF HIGHER THE VALUE THEN SLOWER (from 600 to 1400), -1 BACKWARD
      zhamao.home();
      break;
    case MOVE_LEFT:
      echo(F(">Move Left"));
      zhamao.turn(3, MOVE_SPEED, 1); //3 steps turning LEFT
      zhamao.home();
      break;
    case MOVE_RIGHT:
      echo(F(">Move Right"));
      zhamao.turn(3, MOVE_SPEED, -1); //3 steps turning RIGHT
      zhamao.home();
      break;
    case MOVE_ROAM:
      echo(F(">Roam"));
      break;
    case EMOTION_NORMAL:
      echo(F(">Emotion: Normal"));
      showEmotion(EMOTION_NORMAL);
      break;
    case EMOTION_HAPPY:
      echo(F(">Emotion: Happy"));
      showEmotion(EMOTION_HAPPY);
      break;
    case EMOTION_UNHAPPY:
      echo(F(">Emotion: Unhappy"));
      showEmotion(EMOTION_UNHAPPY);
      break;
    case EMOTION_LICK:
      echo(F(">Emotion: Lick"));
      showEmotion(EMOTION_LICK);
      break;
    case EMOTION_SPEAK:
      echo(F(">Emotion: Speak"));
      showEmotion(EMOTION_SPEAK);
      break;       
    default:
      echo(F(">Unknown"));
      Serial.print(F("unexpect command "));
      Serial.println(cmd);
  }  
}

void echo(const String& str) {  
  BTSerial.println(str);
  Serial.println(str);
}

void ensureConnected(boolean forceConnect) {
  unsigned long now = millis();  
  if (now - lastHeartbeat > 10000) {
    echo(F("heatbeat"));
    lastHeartbeat = now;
  }
  if (!forceConnect && digitalRead(PIN_BT_STATUS) == HIGH) {
    return;
  }
  
  //disconnected
  if (beep) zhamao.sing(S_mode3);
  showEmotion(EMOTION_UNHAPPY);
  while (digitalRead(PIN_BT_STATUS) == LOW) {
    Serial.println(F("connecting bluetooth..."));
    delay(1000);
  }

  //connected
  BTSerial.begin(9600);
  if (beep) zhamao.sing(S_mode1);
  Serial.println(F("connected."));
  showEmotion(EMOTION_HAPPY);
}

void debugDistance() {
  float distance = zhamao.getDistance(); // get the distance from the object - value will be between 99cm and 4cm
  Serial.print(F("dist:"));
  Serial.println(distance); // send the distance measured to the serial port so you can see the distance as a number 
  delay(100);
}

// ----------------- emotion --------------------
void showEmotion(byte nextEmotion) {
  int i;
  // back to normal
  switch (emotion) {
    case EMOTION_NORMAL:      
      break;
    case EMOTION_HAPPY:
      for (i = MOUTH_HEIGHT; i >= 0; i--) {
        u8g2.firstPage();
        do {
          showLowerLip(i);
        } while (u8g2.nextPage());
      }
      break;
    case EMOTION_UNHAPPY:
      for (i = MOUTH_HEIGHT; i >= 0; i--) {
        u8g2.firstPage();
        do {
          showUpperLip(i);
        } while (u8g2.nextPage());
      }
      break;
    case EMOTION_LICK:
      for (i = MOUTH_HEIGHT; i >= 0; i--) {
        u8g2.firstPage();
        do {          
          showTongue(i);          
        } while (u8g2.nextPage());
      }
      break;
    case EMOTION_SPEAK:
      break;
  }
    
  // to the next emotion
  switch (nextEmotion) {
    case EMOTION_NORMAL:
      u8g2.firstPage();
      do {
        u8g2.drawLine(0, 16, MOUTH_WIDTH, 16);
      } while (u8g2.nextPage());
      break;
    case EMOTION_HAPPY:
      for (i = 0; i < MOUTH_HEIGHT; i++) {
        u8g2.firstPage();
        do {
          showLowerLip(i);
        } while (u8g2.nextPage());
      }
      break;
    case EMOTION_UNHAPPY:
      for (i = 0; i < MOUTH_HEIGHT; i++) {
        u8g2.firstPage();
        do {
          showUpperLip(i);
        } while (u8g2.nextPage());
      }
      break;
    case EMOTION_LICK:
      for (i = 0; i < MOUTH_HEIGHT; i++) {
        u8g2.firstPage();
        do {
          u8g2.drawLine(0, 16, MOUTH_WIDTH, 16);
          showTongue(i);          
        } while (u8g2.nextPage());
      }
      break;
    case EMOTION_SPEAK:
      for (i = 0; i < MOUTH_HEIGHT; i++) {
        u8g2.firstPage();
        do {
          showLowerLip(i);
          showUpperLip(i);
          showTongue(i);
        } while (u8g2.nextPage());
      }
      for (i = MOUTH_HEIGHT - 1; i >= 0; i--) {
        u8g2.firstPage();
        do {
          showLowerLip(i);
          showUpperLip(i);
          showTongue(i);
        } while (u8g2.nextPage());        
      }
      break;
  }
  emotion = nextEmotion;
}

void showLowerLip(int i) {
  u8g2.drawLine(MOUTH_WIDTH/4, MOUTH_POSITION+i, MOUTH_WIDTH*3/4, MOUTH_POSITION+i);
  u8g2.drawLine(0,             MOUTH_POSITION,   MOUTH_WIDTH/4,   MOUTH_POSITION+i);
  u8g2.drawLine(MOUTH_WIDTH,   MOUTH_POSITION,   MOUTH_WIDTH*3/4, MOUTH_POSITION+i);
}
void showUpperLip(int i) {
  u8g2.drawLine(MOUTH_WIDTH/4, MOUTH_POSITION-i, MOUTH_WIDTH*3/4, MOUTH_POSITION-i);
  u8g2.drawLine(0,             MOUTH_POSITION,   MOUTH_WIDTH/4,   MOUTH_POSITION-i);
  u8g2.drawLine(MOUTH_WIDTH,   MOUTH_POSITION,   MOUTH_WIDTH*3/4, MOUTH_POSITION-i);
}

void showTongue(int i) {
  u8g2.drawLine(0, MOUTH_POSITION, MOUTH_WIDTH, MOUTH_POSITION);
  u8g2.drawFilledEllipse(MOUTH_WIDTH/2, MOUTH_POSITION, MOUTH_WIDTH/6, i, U8G2_DRAW_LOWER_LEFT|U8G2_DRAW_LOWER_RIGHT);
}

#include <Otto9.h>
#include <SoftwareSerial.h>

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
#define MOVE_SPEED 800

const bool beep = false;
Otto9 zhamao;
SoftwareSerial BTSerial(PIN_SOFT_RX, PIN_SOFT_TX); // Arduino RX|TX, HC05 TX|RX

unsigned long lastHeartbeat = millis();

void ensureConnected(boolean forceConnect) {
  unsigned long now = millis();  
  if (now - lastHeartbeat > 10000) {
    echo("heatbeat");
    lastHeartbeat = now;
  }
  if (!forceConnect && digitalRead(PIN_BT_STATUS) == HIGH) {
    return;
  }
  if (beep) zhamao.sing(S_mode3); //disconnected  
  while (digitalRead(PIN_BT_STATUS) == LOW) {
    Serial.println("connecting bluetooth...");
    delay(1000);
  }
  
  BTSerial.begin(9600);
  if (beep) zhamao.sing(S_mode1); //connected
  Serial.println("connected.");
  BTSerial.println("Hello, I am here.");
}

void echo(const String& str) {  
  BTSerial.println(str);
  Serial.println(str);  
}

void debugDistance() {
  float distance = zhamao.getDistance(); // get the distance from the object - value will be between 99cm and 4cm
  Serial.print("dist:");
  Serial.println(distance); // send the distance measured to the serial port so you can see the distance as a number 
  delay(100);
}

void setup()
{
  Serial.begin(9600);
  delay(2000);
  Serial.println("v3");

  zhamao.init(PIN_YL, PIN_YR, PIN_RL, PIN_RR, true, A6, PIN_Buzzer, PIN_Trigger, PIN_Echo); //Set the servo pins and ultrasonic pins and Buzzer pin
  if (beep) zhamao.sing(S_connection); //Otto wake up!
  zhamao.home();
  delay(50);

  Serial.println("Initialized, waiting for bluetooth connection");  
  ensureConnected(true);
}

void loop()
{  
  ensureConnected(false);
  
  //debugDistance();
  byte cmd = BTSerial.read();
  if (cmd == 255) {
    return; // no data
  }
  Serial.print("incoming byte:");
  Serial.println(cmd);

  
  switch (cmd) {
    case '?':
      echo("Welcome, ? -> help, q -> stand, asdw -> move, e -> roam");
      break;
    case 'q':
      echo(">Stand");
      zhamao.home();
      break;
    case 'w':
      echo(">Move Forward");
      zhamao.walk(4, MOVE_SPEED, 1); //4 steps, "TIME". IF HIGHER THE VALUE THEN SLOWER (from 600 to 1400), 1 FORWARD
      zhamao.home();
      break;
    case 's':
      echo(">Move Backward");
      zhamao.walk(4, MOVE_SPEED, -1); //4 steps, "TIME". IF HIGHER THE VALUE THEN SLOWER (from 600 to 1400), -1 BACKWARD
      zhamao.home();
      break;
    case 'a':
      echo(">Move Left");
      zhamao.turn(3, MOVE_SPEED, 1); //3 steps turning LEFT
      zhamao.home();
      break;
    case 'd':
      echo(">Move Right");          
      zhamao.turn(3, MOVE_SPEED, -1); //3 steps turning RIGHT
      zhamao.home();
      break;
    case 'e':
       echo(">Roam");
       break;
    default:
      echo(">Unknown");
      Serial.print("unexpect command ");
      Serial.println(cmd);
  }  
}

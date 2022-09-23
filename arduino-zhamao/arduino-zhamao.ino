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


Otto9 zhamao;
SoftwareSerial BTSerial(PIN_SOFT_RX, PIN_SOFT_TX); // Arduino RX|TX, HC05 TX|RX

#define CMD_HELP '?'
#define CMD_STAND 's'
#define CMD_WALK_FORWARD 65
#define CMD_WALK_BACKWARD 66
#define CMD_WALK_LEFT 68
#define CMD_WALK_RIGHT 67
#define MOVE_SPEED 800


byte buf[32];
int bufIndex = 0;
unsigned long lastHeartbeat = millis();

void ensureConnected(boolean forceConnect) {
  if (millis() - lastHeartbeat > 10000) {
    echo("heatbeat");
    lastHeartbeat = millis();
  }
  if (!forceConnect && digitalRead(PIN_BT_STATUS) == HIGH) {
    return;
  }
  
  zhamao.sing(S_mode3); //disconnected  
  while (digitalRead(PIN_BT_STATUS) == LOW) {
    Serial.println("connecting bluetooth...");
    delay(1000);
  }
  
  BTSerial.begin(38400);
  zhamao.sing(S_mode1); //connected
  Serial.println("connected.");
  BTSerial.println("Hello, I am here.");
}

bool receiveCommand() {
  delay(500);
  BTSerial.print('.');
  byte b = BTSerial.read();
  if (b == 255) {
    return false; // no data
  }
  Serial.print("incoming byte:");
  Serial.println(b);
  if (bufIndex >= 31) {
    BTSerial.println("Error: Command too long");
    Serial.println("Error: Command too long");
    return true;
  }  
  buf[bufIndex++] = b;
  return true;
}

void resetCommand() {
  bufIndex = 0;
  for (int i = 0; i < 32; i++) {
    buf[i] = 0;
  }
}

void echo(const String& str) {  
  BTSerial.println(str);
  Serial.println(str);
  delay(10);
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
  zhamao.sing(S_connection); //Otto wake up!
  zhamao.home();
  delay(50);

  Serial.println("Initialized, waiting for bluetooth connection");  
  ensureConnected(true);
}

void loop()
{  
  ensureConnected(false);
  
  //debugDistance();
  
  if (!receiveCommand()) {
    return;
  }
  
  if (buf[0] == CMD_HELP) {
    echo("Welcome, ? -> help, s -> stand, arrow -> move");    
    resetCommand();
  } else if (buf[0] == CMD_STAND) {    
    echo("<STAND");
    zhamao.home();
    resetCommand();    
  } else if (buf[0] == 27) { // escape
    if (buf[1] == 0) { // buf[1] will come later, return without reset
      return; 
    } else if (buf[1] == 91) { // buf[1] arrives
      if (buf[2] == 0) { // buf[2] will come later, return without reset
        return;
      }
      switch (buf[2]) {
        case CMD_WALK_FORWARD:
          echo("<FWD");
          zhamao.walk(4, MOVE_SPEED, 1); //4 steps, "TIME". IF HIGHER THE VALUE THEN SLOWER (from 600 to 1400), 1 FORWARD          
          break;
        case CMD_WALK_BACKWARD:
          echo("<BACK");
          zhamao.walk(4, MOVE_SPEED, -1); //4 steps, "TIME". IF HIGHER THE VALUE THEN SLOWER (from 600 to 1400), -1 BACKWARD
          break;
        case CMD_WALK_LEFT:          
          echo("<LEFT");
          zhamao.turn(3, MOVE_SPEED, 1); //3 steps turning LEFT
          break;
        case CMD_WALK_RIGHT:
          echo("<RIGHT");          
          zhamao.turn(3, MOVE_SPEED, -1); //3 steps turning RIGHT
          break;
        default:
          echo("unknown");          
      }
      zhamao.home();
    } else { // wrong      
      Serial.print("expect buf[0/1]=27,91, but buf[1] is");
      Serial.println(buf[1]);
    }
  }
  resetCommand();
  
}

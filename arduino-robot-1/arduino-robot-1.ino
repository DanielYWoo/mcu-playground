#include <PID_v1.h>

// IR remote control
#include <IRremote.h>
#include <IRremoteInt.h>
#define MARK_EXCESS 20

// pins definition
#define PIN_WHEEL_L1    A0
#define PIN_WHEEL_L2    A1
#define PIN_WHEEL_R1    A2
#define PIN_WHEEL_R2    A3
#define PIN_SONIC_TRIG  A4 
#define PIN_SONIC_ECHO  A5
#define PIN_WHEEL_COUNTER_L 2
#define PIN_WHEEL_COUNTER_R 3
#define PIN_IR_RECV     4
#define PIN_SERVO        5
#define PIN_DETECTOR_1   6
#define PIN_DETECTOR_2   7
#define PIN_DETECTOR_3   8
#define PIN_DETECTOR_4   9
#define PIN_WHEEL_LPWM  10
#define PIN_WHEEL_RPWM  11


// speed feedback and PID control
// 3.1415 * D(6.5cm) / 20, not used yet
#define WHEEL_COUNTER_LEN 1.02101761
// the normal speed, signal falls
#define WHEEL_STD_SPEED 12
unsigned long wheelCounterL = 0;
unsigned long wheelCounterR = 0;
double wheelSpeedL = 0;
double wheelSpeedR = 0;
double wheelSetpointL = 0;
double wheelSetpointR = 0;
double wheelOutputL = 0;
double wheelOutputR = 0;
PID wheelLPID(&wheelSpeedL, &wheelOutputL, &wheelSetpointL, 1, 5, 0, DIRECT);
PID wheelRPID(&wheelSpeedR, &wheelOutputR, &wheelSetpointR, 5, 5, 0, DIRECT);
unsigned long wheelLastTime = millis();

// robot constants
#define ROBOT_TURN_LEFT    1
#define ROBOT_TURN_RIGHT   2
#define ROBOT_MOVE_FORWARD 3
#define ROBOT_STOP         4

// IR receiver init
IRrecv irrecv(PIN_IR_RECV);
decode_results results;

void setup() {
  Serial.begin(9600);  
  pinMode(PIN_DETECTOR_1, INPUT);
  pinMode(PIN_DETECTOR_2, INPUT);
  pinMode(PIN_DETECTOR_3, INPUT);
  pinMode(PIN_DETECTOR_4, INPUT);
  
  irrecv.enableIRIn(); // init the IR receiver pin
  attachInterrupt(digitalPinToInterrupt(PIN_WHEEL_COUNTER_L), countWheelL, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_WHEEL_COUNTER_R), countWheelR, FALLING);
  wheelLPID.SetMode(AUTOMATIC);
  wheelRPID.SetMode(AUTOMATIC);
}

// ----------- speed feedback for PID ------------
void countWheelL() { wheelCounterL++; }

void countWheelR() { wheelCounterR++; }

void updateCurrentSpeed() {  
  unsigned long now = millis();
  double elapsed = now - wheelLastTime;
  // don't be too frequent
  if (elapsed < 300) return;  
  
  wheelSpeedL = wheelCounterL / elapsed * 1000;    
  wheelSpeedR = wheelCounterR / elapsed * 1000;   
 
  wheelLPID.Compute();
  wheelRPID.Compute();
  
  analogWrite(PIN_WHEEL_LPWM, wheelOutputL);
  analogWrite(PIN_WHEEL_RPWM, wheelOutputR);
  //analogWrite(PIN_WHEEL_LPWM, 255);
  //analogWrite(PIN_WHEEL_RPWM, 255);
  
  Serial.print("LS=");
  Serial.print(wheelSpeedL);
  Serial.print("\tLO=");
  Serial.print(wheelOutputL);
  Serial.print("\tRS=");
  Serial.print(wheelSpeedR);
  Serial.print("\tRO=");
  Serial.print(wheelOutputR);
  Serial.print("\te=");
  Serial.println(elapsed);

  noInterrupts(); // critical, time-sensitive code here  
  wheelCounterL = 0;
  wheelCounterR = 0;
  wheelLastTime = now;
  interrupts(); 
}

void moveRobot(int dir) {
  Serial.println(String("move:") + dir);
  switch (dir) {
    case ROBOT_TURN_LEFT:    
      digitalWrite(PIN_WHEEL_L1, LOW);
      digitalWrite(PIN_WHEEL_L2, LOW);
      digitalWrite(PIN_WHEEL_R1, HIGH);
      digitalWrite(PIN_WHEEL_R2, LOW);
      wheelSetpointL = 0; wheelSetpointR = WHEEL_STD_SPEED;
      break;
    case ROBOT_TURN_RIGHT:
      digitalWrite(PIN_WHEEL_L1, HIGH);
      digitalWrite(PIN_WHEEL_L2, LOW);
      digitalWrite(PIN_WHEEL_R1, LOW);
      digitalWrite(PIN_WHEEL_R2, LOW);
      wheelSetpointL = WHEEL_STD_SPEED; wheelSetpointR = 0;
      break;
    case ROBOT_MOVE_FORWARD:
      digitalWrite(PIN_WHEEL_L1, HIGH);
      digitalWrite(PIN_WHEEL_L2, LOW);
      digitalWrite(PIN_WHEEL_R1, HIGH);
      digitalWrite(PIN_WHEEL_R2, LOW);      
      wheelSetpointL = WHEEL_STD_SPEED; wheelSetpointR = WHEEL_STD_SPEED;
      break;
    case ROBOT_STOP:
      digitalWrite(PIN_WHEEL_L1, LOW);
      digitalWrite(PIN_WHEEL_L2, LOW);
      digitalWrite(PIN_WHEEL_R1, LOW);
      digitalWrite(PIN_WHEEL_R2, LOW);
      wheelSetpointL = 0; wheelSetpointR = 0;
      break;
    default:
      break;      
  }
}

void processIRCode() {    
  unsigned long currentValue = results.value;  
  Serial.println(currentValue, HEX);    
  
    
    

  switch(currentValue) { 
    case 0xFF30CF: //1 
    case 0x9716BE3F:
      break;
    case 0xFF18E7: //2
    case 0x3D9AE3F7:
      moveRobot(ROBOT_MOVE_FORWARD);
      break;
    case 0xFF7A85: //3  
    case 0x6182021B:
      break;
    case 0xFF10EF: //4
    case 0x8C22657B:
      moveRobot(ROBOT_TURN_LEFT);
      break;
    case 0xFF38C7: //5
    case 0x488F3CBB:
      moveRobot(ROBOT_STOP);
      break;
    case 0xFF5AA5: //6
    case 0x0449E79F:
      moveRobot(ROBOT_TURN_RIGHT);
      break;
    case 0xFFA25D: Serial.println("CH-"); break;
    case 0xFF629D: Serial.println("CH");  break;
    case 0xFFE21D: Serial.println("CH+");  break;
    case 0xFF22DD: Serial.println("PREV"); break;
    case 0xFF02FD: Serial.println("NEXT"); break;
    case 0xFFC23D: Serial.println("PLAY"); break;
    case 0xFFE01F: Serial.println("VOL-"); break;
    case 0xFFA857: Serial.println("VOL+"); break;
    case 0xFF906F: Serial.println("EQ");  break;
    case 0xFF6897: Serial.println("0"); break;
    case 0xFF9867: Serial.println("100+"); break;
    case 0xFFB04F: Serial.println("200+"); break;    
    case 0xFF42BD: Serial.println("7"); break;
    case 0xFF4AB5: Serial.println("8"); break;
    case 0xFF52AD: Serial.println("9"); break;
    case 0xFFFFFF: Serial.println("repeat"); break;
    default: 
      Serial.println("Noisy signal");
  } 
}

void loop()
{ 
  if (irrecv.decode(&results)) {    
    processIRCode();
    irrecv.resume();
  }
    
  updateCurrentSpeed();
  
  //Serial.println(String("detector:") + digitalRead(PIN_DETECTOR_1) +":" + digitalRead(PIN_DETECTOR_2) + ":" +digitalRead(PIN_DETECTOR_3) + ":" + digitalRead(PIN_DETECTOR_4));
}



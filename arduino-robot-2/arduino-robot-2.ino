/* For mega 2560 only */

#include <PID_v1.h>

// pins definition
#define PIN_WHEEL_COUNTER_FL 18
#define PIN_WHEEL_COUNTER_FR 19
#define PIN_WHEEL_COUNTER_RL 20
#define PIN_WHEEL_COUNTER_RR 21
#define PIN_WHEEL_FL1    22
#define PIN_WHEEL_FL2    23
#define PIN_WHEEL_FR1    24
#define PIN_WHEEL_FR2    25
#define PIN_WHEEL_RL1    26
#define PIN_WHEEL_RL2    27
#define PIN_WHEEL_RR1    28
#define PIN_WHEEL_RR2    29

#define PIN_WHEEL_FL_PWM  4
#define PIN_WHEEL_FR_PWM  5
#define PIN_WHEEL_RL_PWM  6
#define PIN_WHEEL_RR_PWM  7

// speed feedback and PID control
// the normal speed, signal falls
unsigned long wheelCounterFL = 0;
unsigned long wheelCounterFR = 0;
unsigned long wheelCounterRL = 0;
unsigned long wheelCounterRR = 0;
double wheelSpeedFL = 0;
double wheelSetpointFL = 0;
double wheelOutputFL = 0;
PID wheelFLPID(&wheelSpeedFL, &wheelOutputFL, &wheelSetpointFL, 1, 8, 0, DIRECT);
unsigned long wheelLastTime = millis();

void setup() {
  Serial.begin(9600);  
  attachInterrupt(digitalPinToInterrupt(PIN_WHEEL_COUNTER_FL), countWheelFL, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_WHEEL_COUNTER_FR), countWheelFR, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_WHEEL_COUNTER_RL), countWheelRL, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_WHEEL_COUNTER_RR), countWheelRR, RISING);
  wheelFLPID.SetMode(AUTOMATIC);
}

// ----------- speed feedback for PID ------------
void countWheelFL() { wheelCounterFL++; }
void countWheelFR() { wheelCounterFR++; }
void countWheelRL() { wheelCounterRL++; }
void countWheelRR() { wheelCounterRR++; }

void updateCurrentSpeed() {  
  
  unsigned long now = millis();
  double elapsed = now - wheelLastTime;
  // don't be too frequent
  if (elapsed < 3000) return;  
  wheelSpeedFL = wheelCounterFL / elapsed * 1000;      
  wheelSetpointFL = 40;
  wheelFLPID.Compute();
  Serial.print(wheelCounterFL);
  Serial.print(" ");
  Serial.println(wheelSpeedFL);
  digitalWrite(PIN_WHEEL_FL1, HIGH);
  digitalWrite(PIN_WHEEL_FL2, LOW);
  analogWrite(PIN_WHEEL_FL_PWM, wheelOutputFL);
  /*
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
*/
 /* 
    noInterrupts(); // critical, time-sensitive code here    
    wheelCounterL = 0;
    wheelLastTime = now;  
    interrupts(); 
  */
}


void loop()
{    
  updateCurrentSpeed(); 
  
}



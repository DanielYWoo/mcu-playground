#include <PID_v1.h>

// pins definition
#define PIN_WHEEL_L1    4
#define PIN_WHEEL_L2    5
#define PIN_WHEEL_COUNTER_L 2
#define PIN_WHEEL_LPWM  6

// speed feedback and PID control
// the normal speed, signal falls
unsigned long wheelCounterL = 0;
double wheelSpeedL = 0;
double wheelSetpointL = 0;
double wheelOutputL = 0;
PID wheelLPID(&wheelSpeedL, &wheelOutputL, &wheelSetpointL, 0.8,9, 0, DIRECT);
unsigned long wheelLastTime = millis();

void setup() {
  Serial.begin(9600);  
  attachInterrupt(digitalPinToInterrupt(PIN_WHEEL_COUNTER_L), countWheelL, FALLING);
  wheelLPID.SetMode(AUTOMATIC);
}

// ----------- speed feedback for PID ------------
void countWheelL() { wheelCounterL++; }

void updateCurrentSpeed() {  
  
  unsigned long now = millis();
  double elapsed = now - wheelLastTime;
  // don't be too frequent
  if (elapsed < 300) return;  
  //Serial.println(wheelCounterL);
  wheelSpeedL = wheelCounterL / elapsed * 1000;      
  wheelSetpointL = 40;
  wheelLPID.Compute();
  Serial.print(wheelCounterL);
  Serial.print(" ");
  Serial.println(wheelSpeedL);
  digitalWrite(PIN_WHEEL_L1, HIGH);
  digitalWrite(PIN_WHEEL_L2, LOW);
  analogWrite(PIN_WHEEL_LPWM, wheelOutputL);
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
  //if (elapsed > 500) {
    noInterrupts(); // critical, time-sensitive code here    
    wheelCounterL = 0;
    wheelLastTime = now;  
    interrupts(); 
  //}
}


void loop()
{    
  updateCurrentSpeed(); 
  
}



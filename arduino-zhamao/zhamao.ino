#include <Servo.h>

Servo servoLeftLeg;
Servo servoRightLeg;
Servo servoLeftFoot;
Servo servoRightFoot;

int counter;

void setup()
{
  Serial.begin(9600);  
  servoLeftLeg.attach(2, 500, 2500);
  servoRightLeg.attach(3, 500, 2500);
  servoLeftFoot.attach(4, 500, 2500);
  servoRightFoot.attach(5, 500, 2500);  
}

void loop()
{
  for (counter = 0; counter < 10; ++counter) {
    servoLeftFoot.write(0);    
    delay(1000);
    servoLeftFoot.write(180);    
    delay(1000);
    
    /*
    servoRightLeg.write(90);
    servoLeftFoot.write(30);
    servoRightFoot.write(30);
    delay(1000); // Wait for 1000 millisecond(s)
    servoLeftLeg.write(0);
    delay(1000); // Wait for 1000 millisecond(s)                                                        
    Serial.println(0.01723 * readUltrasonicDistance(8, 9));
    */
  }
}


long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}

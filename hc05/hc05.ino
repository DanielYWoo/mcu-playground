#include <SoftwareSerial.h>

SoftwareSerial HC05(5, 6);
 
void setup() 
{  
  Serial.begin(9600);
  HC05.begin(9600);
  Serial.println("ready");  
} 

void loop() 
{ 
  HC05.write('=');
  Serial.write('.');
  delay(1000);
}

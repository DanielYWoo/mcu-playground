#include <SoftwareSerial.h> 

const int RxD = 10;
const int TxD = 11;
const int StD = 12;

SoftwareSerial HC05(RxD,TxD); // RX, TX
 
void setup() 
{ 
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);

  Serial.begin(9600);
  HC05.begin(38400);
  Serial.println("ready");
} 

char c = ' ';
void loop() 
{ 
  if(HC05.available()) {
    c = HC05.read();        
    Serial.write(c);
  }
  if (Serial.available()) {
    c = Serial.read();
    HC05.write(c);
  }
}

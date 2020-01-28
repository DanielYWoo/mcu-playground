// for 1602
#include <LiquidCrystal.h>

const int PIN_1602_RS = 5;
const int PIN_1602_EN = 4;
const int PIN_1602_D4 = 12;
const int PIN_1602_D5 = 11;
const int PIN_1602_D6 = 10;
const int PIN_1602_D7 = 9;

const int PIN_WATER_LEVEL = 3;

// -------------- global objects -----------------
LiquidCrystal lcd(PIN_1602_RS, PIN_1602_EN, PIN_1602_D4, PIN_1602_D5, PIN_1602_D6, PIN_1602_D7); // create an LCD object

// -------------- controls vaiables -----------------
int m01 = 10;
bool refill = false;
long t1 = 0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  pinMode(7, OUTPUT); // F01
  pinMode(6, OUTPUT); // F02
  digitalWrite(7, HIGH);
  digitalWrite(6, HIGH);
}

void loop() {
  m01 = analogRead(A0);
  refill = digitalRead(PIN_WATER_LEVEL);
  display();
  
  if (m01 > 500 && millis() - t1 > 60000) {
    Serial.println("watering");
    digitalWrite(7, LOW);
    delay(2000);
    digitalWrite(7, HIGH);
    t1 = millis();
    Serial.println("done");
  }
  display();
  delay(10000);
  
}

void display() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("F01 M=");
  lcd.print(m01);
  lcd.setCursor(0, 1);
  lcd.print("refill=");
  lcd.print(refill);
}

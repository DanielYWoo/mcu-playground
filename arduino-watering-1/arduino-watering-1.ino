// for 1602
#include <LiquidCrystal.h>

const int PIN_1602_RS = 5;
const int PIN_1602_EN = 4;
const int PIN_1602_D4 = 12;
const int PIN_1602_D5 = 11;
const int PIN_1602_D6 = 10;
const int PIN_1602_D7 = 9;

//const int PIN_BTN_1 = 3;

// -------------- global objects -----------------
LiquidCrystal lcd(PIN_1602_RS, PIN_1602_EN, PIN_1602_D4, PIN_1602_D5, PIN_1602_D6, PIN_1602_D7); // create an LCD object

// -------------- controls vaiables -----------------
int m01 = 10;
bool refill = false;
long t1=0;
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
}

void loop() {
  m01 = analogRead(A0);
  lcd.print("F01 M=");
  lcd.print(m01);
  lcd.setCursor(0, 1);
  lcd.print("refill=");
  lcd.print(refill);
  if (m01 > 500 && millis() - t1 > 18000) {
    Serial.println("watering");
    delay(5000);
    t1 = millis();
    Serial.println("done");
  }

  delay(1000);
  lcd.clear();


}

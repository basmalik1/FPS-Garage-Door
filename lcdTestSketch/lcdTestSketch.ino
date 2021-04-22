#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

void setup()
{
  lcd.begin(16,2);  // start serial monitor serial
  lcd.print("Hello World");  // start lcd serial
}

void loop()
{
  delay(1000);
  lcd.print("a");
  delay(300);
  lcd.print("Bye World");
  delay(300);
  lcd.print("Hello World");
}

#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup()
{
  lcd.begin(20, 4);
  lcd.clear();
}

void loop()
{

  auto time = millis();
  lcd.setCursor(0, 0);
  PrintTime(time, 0);

  lcd.setCursor(10, 0);
  PrintTime(time, 1);

  lcd.setCursor(0, 1);
  PrintTime(time, 2);

  lcd.setCursor(10, 1);
  PrintTime(time, 3);

  lcd.setCursor(0, 2);
  PrintTime(time * 7, 1);

  lcd.setCursor(10, 2);
  PrintTime(time * 7, 3);

  lcd.setCursor(0, 3);
  PrintTime(time * 21, 1);

  lcd.setCursor(10, 3);
  PrintTime(time * 21, 2);
}

void PrintTime(unsigned long ms)
{
  PrintTime(ms, 3);
}

void PrintTime(unsigned long ms, byte fractionDigits)
{
  auto minutes = ms / 60000;
  auto seconds = ms % 60000 / 1000;
  auto fraction = ms % 1000;

  if (minutes > 0)
  {
    PrintDigits(minutes, 2, false);
    lcd.print(':');
  }
  else
    lcd.print("   ");

  PrintDigits(seconds, 2, minutes > 0);

  if (fractionDigits > 0)
    lcd.print(".");

  switch (fractionDigits)
  {
  case 0:
    return;
  case 1:
    PrintDigits(fraction / 100, 1, true);
    return;
  case 2:
    PrintDigits(fraction / 10, 2, true);
    return;
  case 3:
    PrintDigits(fraction, 3, true);
    return;
  }
}

void PrintDigits(int value, size_t digits, bool printZero)
{
  if (value < 100 && digits > 2)
    lcd.print(printZero ? '0' : ' ');
  if (value < 10 && digits > 1)
    lcd.print(printZero ? '0' : ' ');

  switch (digits)
  {
  case 1:
    value = value % 10;
    break;
  case 2:
    value = value % 100;
    break;
  default:
    value = value % 1000;
    break;
  }
  lcd.print(value);
}

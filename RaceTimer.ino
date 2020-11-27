#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

uint8_t PinLapA = A1;
uint8_t PinLapB = A2;
uint8_t PinReset = A3;

struct RaceInfo
{
  bool Started;
  unsigned long LapStart;
  int CurrentLap;
  unsigned long LastLapTime;
  int BestLap;
  unsigned long BestLapTime;
};

struct RaceInfo A = {0};
struct RaceInfo B = {0};

void setup()
{
  Reset(&A);
  Reset(&B);

  pinMode(PinLapA, INPUT_PULLUP);
  pinMode(PinLapB, INPUT_PULLUP);
  pinMode(PinReset, INPUT_PULLUP);

  lcd.begin(20, 4);
  lcd.clear();

  Serial.begin(112500);
}

void loop()
{
  if (digitalRead(PinReset) == LOW)
  {
    Reset(&A);
    Reset(&B);
  }

  if (digitalRead(PinLapA) == LOW)
   SetLap(&A);

  if (digitalRead(PinLapB) == LOW)
    SetLap(&B);

  auto now = millis();

  lcd.setCursor(0, 0);
  lcd.print("24 123.45  25  17,23");
  lcd.setCursor(0, 1);
  lcd.print("12 111.10   9  17,23");
}

void Reset(RaceInfo *info)
{
  info->Started = false;
  info->LapStart = 0;
  info->CurrentLap = 1;
  info->LastLapTime = 0;
  info->BestLap = 0;
  info->BestLapTime = 0;
}

void SetLap(RaceInfo *info)
{
  auto now = millis();
  
  if (!info->Started)
  {
    info->Started = true;
    info->LapStart = now;
    return;
  }

  // Debounce
  if (now - info->LapStart < 100) 
    return; 

  auto lapTime = now - info->LapStart;

  info->LastLapTime = lapTime;
  if (lapTime < info->BestLapTime)
  {
    info->BestLap = info->CurrentLap;
    info->BestLapTime = lapTime;
  }
  info->LapStart = now;
  info->CurrentLap++;
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

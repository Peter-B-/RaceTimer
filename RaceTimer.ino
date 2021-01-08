#include <LiquidCrystal.h>

const int rs = 12, en = 10, d4 = 5, d5 = 4, d6 = 7, d7 = 6;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

uint8_t PullupPinLapA = A0;
uint8_t PullupPinLapB = A1;
uint8_t PinLapA = 2;
uint8_t PinLapB = 3;

uint8_t PinReset = A3;
uint8_t LcdRW = 11;

byte SymbolAverage[8] = {
  0b00000,
  0b00001,
  0b01110,
  0b10011,
  0b10101,
  0b11001,
  0b01110,
  0b10000,
};

struct RaceInfo
{
  bool Started;
  unsigned long LapStart;
  int CurrentLap;
  unsigned long LastTime;
  int BestLap;
  unsigned long BestTime;
  bool LapUpdate;
  unsigned long AverageTime;
  unsigned long SumTime;
};

struct RaceInfo A = {0};
struct RaceInfo B = {0};
bool isReset = true;

bool lapA = false;
bool lapB = false;

void setup()
{
  Reset(&A);
  Reset(&B);

  pinMode(PullupPinLapA, INPUT_PULLUP);
  pinMode(PullupPinLapB, INPUT_PULLUP);
  pinMode(PinReset, INPUT_PULLUP);
  pinMode(LcdRW, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(PinLapA), IsrLapA, RISING); 
  attachInterrupt(digitalPinToInterrupt(PinLapB), IsrLapB, RISING); 

  digitalWrite(LcdRW, LOW);

  lcd.begin(20, 4);
  lcd.createChar(1, SymbolAverage);

  ResetLcd();
 
  Serial.begin(112500);
  Serial.println("Race Timer");
}

void loop()
{
  if (digitalRead(PinReset) == LOW)
  {
    Reset(&A);
    Reset(&B);
    ResetLcd();
    isReset = true;
  }

  if (lapA)
  {
    SetLap(&A);
    lapA = false;
  }

  if (lapB)
  {
    SetLap(&B);
    lapB = false;
  }

  auto now = millis();

  if (!A.Started && !B.Started)
    return;
  
  if (isReset)
  {
    lcd.clear();
    isReset = false;
  }

  PrintRace(&A, now, 0);
  PrintRace(&B, now, 11);
} 

void PrintRace(RaceInfo *info, unsigned long now, uint8_t col)
{

  if (info->Started)
  {
    lcd.setCursor(3 + col, 0);
    auto elapsed = now - info->LapStart;
    if (info->CurrentLap > 1 && elapsed < 2500)
      PrintTime(info->LastTime, 2);
    else
    {
      PrintTime(elapsed, 1);
      lcd.print(' ');
    }
  }

  if (!info->LapUpdate)
    return;

  lcd.setCursor(0 + col, 0);
  PrintDigits(info->CurrentLap, 2);

  lcd.setCursor(3 + col, 1);
  PrintTime(info->LastTime, 2);

  if (info->CurrentLap > 2)
  {
    lcd.setCursor(0 + col, 2);
    PrintDigits(info->BestLap, 2);
    
    lcd.setCursor(3 + col, 2);
    PrintTime(info->BestTime, 2);

    lcd.setCursor(3 + col, 3);
    PrintTime(info->AverageTime, 2);

    lcd.setCursor(10, 2);
    lcd.print('*');
    lcd.setCursor(10, 3);
    lcd.write(byte(1));
  }

  info->LapUpdate = false;
}

void Reset(RaceInfo *info)
{
  info->Started = false;
  info->LapStart = 0;
  info->CurrentLap = 1;
  info->LastTime = 0;
  info->BestLap = 0;
  info->BestTime = 0;
  info->LapUpdate = false;
  info->SumTime = 0;
  info->AverageTime = 0;
}

void ResetLcd()
{
  lcd.clear();
  lcd.setCursor(2,1);
  lcd.write((byte)0x7E);
  lcd.write((byte)0x3E);
  lcd.print("    Race     ");
  lcd.write((byte)0x3C);
  lcd.write((byte)0x7F);

  lcd.setCursor(2,2);
  lcd.write((byte)0x7E);
  lcd.write((byte)0x3E);
  lcd.print("    Timer    ");
  lcd.write((byte)0x3C);
  lcd.write((byte)0x7F);
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
  if (now - info->LapStart < 500) 
    return; 

  auto lapTime = now - info->LapStart;

  info->LastTime = lapTime;
  if (info->BestLap == 0 || lapTime < info->BestTime)
  {
    info->BestLap = info->CurrentLap;
    info->BestTime = lapTime;
  }
  info->SumTime+=lapTime;
  info->AverageTime = info->SumTime / info->CurrentLap;
  info->LapStart = now;
  info->CurrentLap++;
  info->LapUpdate = true;
}

void PrintTime(unsigned long ms)
{
  PrintTime(ms, 2);
}

void PrintTime(unsigned long ms, byte fractionDigits)
{
  auto seconds = ms / 1000;
  auto fraction = ms % 1000;

  if (seconds >= 1000)
    lcd.print("???");
  else
    PrintDigits(seconds, 3, false);

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

void PrintDigits(int value, byte digits)
{
  PrintDigits(value, digits, false);
}

void PrintDigits(int value, byte digits, bool printZero)
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

void IsrLapA()
{
  lapA = true;
}

void IsrLapB()
{
  lapB = true;
}

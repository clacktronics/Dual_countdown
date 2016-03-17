#include <RCSwitch.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <stdio.h>


RCSwitch mySwitch = RCSwitch();

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

int i;
bool timer_one_set = false;
bool timer_two_set = false;
int cs1 = 0;
int cs2 = 0;
int cm1 = 0;
int cm2 = 0;
unsigned long elapsed1 = millis();
unsigned long elapsed2 = millis();
byte sel_time = 0;

int m1 = EEPROM.read(0);
int m2 = EEPROM.read(2);
byte last_m1 = m1;
byte last_m2 = m2;

int s1 = EEPROM.read(1);
int s2 = EEPROM.read(3);
byte last_s1 = s1;
byte last_s2 = s2;

bool setting_time = false;

unsigned long bounce1 = millis();
unsigned long bounce2 = millis();
unsigned long savecycle = millis();

char toDisp1[14 * 6];
char toDisp2[14 * 6];



#define DATA_PIN 3 // external int 1 on Uno

void setup() {

  pinMode(DATA_PIN, INPUT);
  // just leave D4 tristated

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);

  mySwitch.enableReceive(1);  // Receiver on interrupt 1 => that is pin D3

  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  digitalWrite(A0, HIGH);
  digitalWrite(A1, HIGH);

  pinMode(4, OUTPUT);
}


void loop() {

  if (mySwitch.available()) {

    if (mySwitch.getReceivedValue() == 1115484 || mySwitch.getReceivedValue() == 15578529)
    {
      timer_one_set = true;
    }
    if (mySwitch.getReceivedValue() == 348500 || mySwitch.getReceivedValue() == 6540706)
    {
      timer_two_set = true;
    }

  }

  mySwitch.resetAvailable();

  if (timer_one_set)
  {
    cs1 = s1;
    cm1 = m1;
    timer_one_set = false;
    elapsed1 = millis();
  }

  if (timer_two_set)
  {
    cs2 = s2;
    cm2 = m2;
    timer_two_set = false;
    elapsed2 = millis();
  }

  if ((millis() - elapsed1) > 1000)
  {

    cs1 -= 1;
    if (cs1 <= -1)
    {
      if (cm1 > 0)
      {
        cm1 -= 1;
        cs1 = 59;
      }
      else
      {
        cs1 = 0;
      }
    }

    elapsed1 += 1000;
  }

  if ((millis() - elapsed2) > 1000)
  {
    cs2 -= 1;
    if (cs2 <= -1)
    {
      if (cm2 > 0)
      {
        cm2 -= 1;
        cs2 = 59;
      }
      else
      {
        cs2 = 0;
      }
    }

    elapsed2 += 1000;
  }


  if (!digitalRead(A0))
  {
    lcd.setCursor(0, 0);
    lcd.write(0xFF);
    lcd.setCursor(0, 1);
    lcd.print(" ");
    sel_time = 1;
  }
  if (!digitalRead(A1))
  {
    lcd.setCursor(0, 1);
    lcd.write(0xFF);
    lcd.setCursor(0, 0);
    lcd.print(" ");
    sel_time = 2;
  }

  if (!digitalRead(A3) && (millis() - bounce1) > 10)
  {
    if (sel_time == 1)       {
      s1++;
      if (s1 > 59) {
        s1 = 0;
        m1++;
      }
    }
    else if (sel_time == 2)  {
      s2++;
      if (s2 > 59) {
        s2 = 0;
        m2++;
      }
    }
    if (m1 > 99) {
      m1 = 99;
    }
    if (m2 > 99) {
      m2 = 99;
    }

    bounce1 = millis();
  }

  if (!digitalRead(A2) && (millis() - bounce2) > 10)
  {
    if (sel_time == 1)       {
      s1--;
      if (s1 < 0) {
        s1 = 59;
        m1--;
      }
    }
    else if (sel_time == 2)  {
      s2--;
      if (s2 < 0) {
        s2 = 59;
        m2--;
      }
    }
    if (m1 < 0) {
      m1 = 0;
    }
    if (m2 < 0) {
      m2 = 0;
    }

    bounce2 = millis();
  }

  if ((millis() - savecycle) > 20000)
  {
    if (s1 != last_s1 && m1 != last_m1)
    {
      EEPROM.write(0, m1);
      EEPROM.write(1, s1);
      last_s1 = s1;
      last_m1 = m1;
    }


    if (s2 != last_s2  && m2 != last_m2 )
    {
      EEPROM.write(2, m2);
      EEPROM.write(3, s2);
      last_s2 = s2;
      last_m2 = m2;
    }

    savecycle = millis();
  }

  if (cs1 == 0 && cm1 == 0 && cs2 == 0 && cm2 == 0)
  {
    digitalWrite(4, HIGH);
    lcd.setCursor(15, 1);
    lcd.write(0xFF);
  }
  else
  {
    digitalWrite(4, LOW);
    lcd.setCursor(15, 1);
    lcd.print(" ");
  }

  sprintf(toDisp1, "%02d:%02d - %02d:%02d", cm1, cs1, m1, s1);
  lcd.setCursor(1, 0);
  lcd.print(toDisp1);

  sprintf(toDisp2, "%02d:%02d - %02d:%02d", cm2, cs2, m2, s2);
  lcd.setCursor(1, 1);
  lcd.print(toDisp2);



}




//Home_Automation_with_fan_dimmer by Swayamshree Mohanty

//If you don't have the library like <IRremote.h>, <TimerOne.h> and others also in your system, then download the library from the (Sketch-->Include Library-->Manage Library) or press (Ctrl+Shift+I)

#include<IRremote.h>
#include<LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>

//Initialization of Bluetooth
SoftwareSerial BTserial(8, 9);

//Initialization of LCD Display 
LiquidCrystal lcd(6, 7, 13, 12, 11, 10);

//For IR Receiver PIN
int recv_pin = A0;
IRrecv irrecv(recv_pin);
decode_results results;

volatile int z = 0;
volatile boolean zero_cross = 0;

//For relay of FAN
int AC_pin = 3;
int dim2 = 0;
int dim = 110;
int freqStep = 75;
char incomingByte;
String ctr;

//For LCD Display on/off
int lcdled = A4;
int lcd1 = HIGH;

char x;

//Pins for Light1,Light2,FAN
int rp1 = A1;
int rp2 = A2;
int rp3 = A3;

int initial1 = LOW;
int initial2 = LOW;
int initial3 = LOW;

//Startup show of the LCD Display
byte hea[8] = {0b00000, 0b01010, 0b11111, 0b11111, 0b11111, 0b01110, 0b00100, 0b00000};
byte smi[8] = {0b00000, 0b00000, 0b01010, 0b00000, 0b10001, 0b01110, 0b00000, 0b00000};
byte sp1[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b00000};
byte sp2[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b00000};
byte sp3[8] = {0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b00000};
byte sp4[8] = {0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b00000};
byte sp5[8] = {0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b00000};

int i = 0, j = 0, k = 0;
void sfl(int line, char str1[])
{
  i = 40 - strlen(str1);
  line -= 1;
  for (j = i; j <= i + 16; j++)
  {
    for (k = 0; k <= 15; k++)
      lcd.print(" ");
    lcd.setCursor(j, line);
    lcd.print(str1);
    delay(100);
  }
}

int e = 1;

void setup()
{
  Serial.begin(9600);
  BTserial.begin(9600);
  irrecv.enableIRIn();

  pinMode(rp1, OUTPUT);
  pinMode(rp2, OUTPUT);
  pinMode(rp3, OUTPUT);

//For relay of FAN
  pinMode(AC_pin, OUTPUT);
  attachInterrupt(0, zero_cross_detect, RISING);
  Timer1.initialize(freqStep);
  Timer1.attachInterrupt(dim_check, freqStep);

//For LCD Display
  pinMode(lcdled, OUTPUT);
  digitalWrite(lcdled, HIGH);
  lcd.begin(16, 2);
  lcd.createChar(1, hea);
  lcd.createChar(2, smi);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("B1- OFF");
  lcd.setCursor(9, 0);
  lcd.print("B2- OFF");
  lcd.setCursor(0, 1);
  lcd.print("FAN-OFF ");
  lcd.setCursor(7, 1);
  lcd.print("**SPEED**");
  lcd.createChar(3, sp1);
  lcd.createChar(4, sp2);
  lcd.createChar(5, sp3);
  lcd.createChar(6, sp4);
  lcd.createChar(7, sp5);
}

void zero_cross_detect()
{
  zero_cross = true;
  z = 0;
  digitalWrite(AC_pin, LOW);
}

void dim_check()
{
  if (zero_cross == true)
  {
    if (z >= dim)
    {
      digitalWrite(AC_pin, HIGH);
      z = 0;
      zero_cross = false;
    }
    else
    {
      z++;
    }
  }
}

//Dimmer Program
void switchcond()
{
  switch (e)
  {
    case 1:
      dim = 110;
      break;

    case 2:
      dim = 90;
      break;

    case 3:
      dim = 65;
      break;

    case 4:
      dim = 40;
      break;

    case 5:
      dim = 0;
      break;
  }
}

//void recvir(), For IR reciever input
void translateIR()
{
  if (results.value == 0x81222B21 ||results.value == 0x80BF49B6 ||results.value == 0x81222B21 )
  {
    initial1 = (initial1 == LOW) ? HIGH : LOW;
  }
  else if (results.value == 0x80BFC936 ||results.value == 0xFD100BD7)
  {
    initial2 = (initial2 == LOW) ? HIGH : LOW;
  }
  else if (results.value == 0xB3BCB395 || results.value == 0x80BF33CC )
  {
    initial3 = (initial3 == LOW) ? HIGH : LOW;

  }
  else if (results.value == 0x2443519 ||results.value == 0x80BF3BC4)
  {

    initial1 = (initial1 == LOW) ? HIGH : LOW;
    initial2 = (initial2 == LOW) ? HIGH : LOW;
    initial3 = (initial3 == LOW) ? HIGH : LOW;
  }
  else if (results.value == 0x40352E21 ||results.value == 0x80BF39C6 ||results.value == 0x3AB76AD5)
  {
    initial1 = LOW;
    initial2 = LOW;
    initial3 = LOW;
  }
  else if (results.value == 0x80BFBB44 ||results.value == 0xE08BBA1C ||results.value == 0x39A744DF)
  {
    e--;
    if (e > 0)
      switchcond();
    else
      e = 1;
  }
  else if (results.value == 0x80BF31CE ||results.value == 0xEC2FE951)
  {
    e++;
    if (e < 6)
      switchcond();
    else
      e = 5;
  }

  else if (results.value == 0x1FE30CF)
  {
    lcd1 = (lcd1 == HIGH) ? LOW : HIGH;
  }
}

//For Bluetooth communication
void BRswitch()
{
  if (x == 'a')
  {
    initial1 = LOW;
  }

  else if (x == 'b')
  {
    initial1 = HIGH;
  }

  else if (x == 'c')
    initial2 = LOW;

  else if (x == 'd')
    initial2 = HIGH;

  else if (x == 'e')
    initial3 = LOW;

  else if (x == 'f')
    initial3 = HIGH;

  else if (x == 'g')
  {
    e--;
    if (e > 0)
      switchcond();
    else
      e = 1;
  }
  else if (x == 'h')
  {
    e++;
    if (e < 6)
      switchcond();
    else
      e = 5;
  }
  else if (x == 'i')
  {
    initial1 = HIGH;
    initial2 = HIGH;
    initial3 = HIGH;
    x = 0;
  }

  else if (x == 'j')
  {
    initial1 = LOW;
    initial2 = LOW;
    initial3 = LOW;
    x = 0;
  }

  else if (x == 'k')
    lcdled = LOW;

  else if (x == 'l')
    lcdled =  HIGH;
}

//For Output pin combination
void rswitch()
{
  digitalWrite(rp1, initial1);
  digitalWrite(rp2, initial2);
  digitalWrite(rp3, initial3);
  digitalWrite(lcdled, lcd1);
}


//LCD Display Operation
void lcddisp()
{
  if (initial1 == HIGH)
  {
    lcd.setCursor(0, 0);
    lcd.print("B1- ON ");
  }

  if (initial1 == LOW)
  {
    lcd.setCursor(0, 0);
    lcd.print("B1- OFF");
  }

  if (initial2 == HIGH)
  {
    lcd.setCursor(9, 0);
    lcd.print("B2- ON ");
  }
  else if (initial2 == LOW)
  {
    lcd.setCursor(9, 0);
    lcd.print("B2- OFF");
  }

  if (initial3 == HIGH)
  {
    lcd.setCursor(0, 1);
    lcd.print("FAN-ON->");
  }
  else if (initial3 == LOW)
  {
    lcd.setCursor(0, 1);
    lcd.print("FAN-OFF ");
  }
  if (e == 1)
  {
    lcd.setCursor(8, 1);
    lcd.print("S-1");
    lcd.setCursor(11, 1);
    lcd.write(3);
    lcd.setCursor(12, 1);
    lcd.print("           ");

  }
  if (e == 2)
  {
    lcd.setCursor(8, 1);
    lcd.print("S-2");
    lcd.setCursor(11, 1);
    lcd.write(3); lcd.write(4);
    lcd.setCursor(13, 1);
    lcd.print("          ");
  }
  if (e == 3)
  {
    lcd.setCursor(8, 1);
    lcd.print("S-3");
    lcd.setCursor(11, 1);
    lcd.write(3); lcd.write(4); lcd.write(5);
    lcd.setCursor(14, 1);
    lcd.print("         ");
  }
  if (e == 4)
  {
    lcd.setCursor(8, 1);
    lcd.print("S-4");
    lcd.setCursor(11, 1);
    lcd.write(3); lcd.write(4); lcd.write(5); lcd.write(6);
    lcd.setCursor(15, 1);
    lcd.print("        ");
  }
  if (e == 5)
  {
    lcd.setCursor(8, 1);
    lcd.print("S-5");
    lcd.setCursor(11, 1);
    lcd.write(3); lcd.write(4); lcd.write(5); lcd.write(6); lcd.write(7);
  }
}


void loop()
{
  if (irrecv.decode(&results))
  {
    Serial.println(results.value, HEX);
    delay(500);
    translateIR();
    irrecv.resume();
  }

  else if (BTserial.available())
  {
    delay(100);
    x = BTserial.read();
    Serial.println(x);
    BRswitch();
  }
  rswitch();
  lcddisp();
}

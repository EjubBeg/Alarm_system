#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 
#include "IRremote.h"
#include <Servo.h>
#include <TonePlayer.h>


#define GasPin A1
#define DirPin 2
#define ServPin 10
#define Soundpin 9
#define receiver 11


void set_defaults(); 
void open_door();
void close_door();
void readIR();
void do_order();
void turn_on_lcd();
void warning(); 
void check_result(); 
void print_screen();  
void add_to_num();  
long long ConvertCharToLong(); 
void sound_on();
void connection_setup();
void set_lcd();
void set_pins();
void listen_pi();
void listen_sensors();
bool door_state();  


unsigned int try_times;
unsigned long time_now ; 
int pos ;   
bool authorization ;
char order[10];
char strnumber[17]= "" ;
int num_position;
bool lcd_ligth = false;
String  incoming;
bool connection;
bool alarm;
int analog_read;
int digital_read;
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2); 
IRrecv irrecv(receiver);    
decode_results results; 
Servo myservo; 
TonePlayer tone1 (TCCR1A, TCCR1B, OCR1AH, OCR1AL, TCNT1H, TCNT1L); 


void setup() {
  Serial.begin(9600);
  myservo.attach(ServPin);
  irrecv.enableIRIn(); 
  set_defaults();
  set_pins();
  set_lcd();
  lcd.print("PROGRAM STARTS..");
  connection_setup();
  time_now= millis();
  while(time_now+1000>millis())
  {}
  lcd.clear();
  lcd.noBacklight();
  myservo.write(pos);
}


void set_defaults(){
  pos = 0;
  connection=false;
  alarm= false;
  num_position=0;
  try_times = 0;
  authorization=false;
}


void set_lcd()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
}


void set_pins()
{
    pinMode (Soundpin, OUTPUT); 
    pinMode(GasPin, INPUT);
    pinMode(DirPin,INPUT);
}


void connection_setup(){

Serial.print("Start");

while(!connection)
{
  if (Serial.available() > 0) {
incoming = Serial.readString();
lcd.clear();
lcd.setCursor(0,0);
lcd.print(incoming);
if(incoming.equals("Connected"))
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Its connected");
  connection = true;
}
}
}
}


void loop() {

listen_pi();
listen_sensors();

  if (try_times== 3)
  {
      Serial.print("Times");
      sound_on();
  }
}


void listen_pi()
{
  if (Serial.available() > 0)
  {
    incoming= Serial.readString();
    if(incoming.equals("Alarm"))
    {
      alarm = true;
    }
    else if(incoming.equals("AlarmOff"))
    {
      alarm= false;
    }
  }
}


void listen_sensors()
{
  analog_read = analogRead(GasPin);

  analog_read = map(analog_read,0,1023,0,100);

  if(analog_read > 60)
  {
    Serial.print("Vatra"); 
    sound_on();
  }

  digital_read= digitalRead(DirPin);
  if((digital_read==HIGH)&&(alarm))
  {
    Serial.print("Lopov"); 
    sound_on();
  }
    if (irrecv.decode(&results))
  {
    readIR(); 
    irrecv.resume(); 
    print_screen();
  }  
}


void readIR()
{
  switch(results.value)
  {
  case 0xFFA25D: strcpy(order, "Power"); break; 
  case 0xFF02FD: strcpy(order, "Check"); break;
  case 0xFF6897: strcpy(order, "0");    break;
  case 0xFF30CF: strcpy(order, "1");    break;
  case 0xFF18E7: strcpy(order, "2");    break;
  case 0xFF7A85: strcpy(order, "3");    break;
  case 0xFF10EF: strcpy(order, "4");    break;
  case 0xFF38C7: strcpy(order, "5");    break;
  case 0xFF5AA5: strcpy(order, "6");    break;
  case 0xFF42BD: strcpy(order, "7");    break;
  case 0xFF4AB5: strcpy(order, "8");    break;
  case 0xFF52AD: strcpy(order, "9");    break;

  default: 
    strcpy(order , "False");
  }

if(strcmp(order,"False") == 0)
  return;
  
do_order();
} 


void do_order()
{
  if(strcmp(order,"Power") == 0)
  {    
  turn_on_lcd();
  }
  else if((strcmp(order,"Check") == 0)&&(lcd_ligth == true))
  {
  check_result();
  }
  else if (lcd_ligth == true)
  {
   add_to_num(); 
  }
}


void turn_on_lcd(){
if(lcd_ligth == false)
{
  lcd.backlight();
  lcd_ligth = true; 
}
else
{
lcd.clear();
lcd.noBacklight();
strcpy(strnumber, "");
lcd_ligth = false; 
num_position= 0;
}
}


void print_screen(){
  if(lcd_ligth == false)
  return;

  if((authorization)&&(millis()>time_now+2000))
  {
    close_door();
    authorization= false;
    return;
  }

  if(authorization)
  {
  lcd.setCursor(0, 0); 
  lcd.print("Correct Password");
  }
  else
  {
  lcd.setCursor(0, 0); 
  lcd.print("Password:");
  lcd.setCursor(0, 1); 
  lcd.print(strnumber);
  }
}


void add_to_num(){
if(num_position == 16)
return;

strcat(strnumber, order);
num_position++;
}


void check_result()
{

if(strlen(strnumber)!=10)
{
  strcpy(strnumber,"");
  strcpy(strnumber, "Wrong Password");
  try_times++;
  return;
}

Serial.print("Check");
delay(10);
Serial.print(strnumber);
while (1) {
if (Serial.available() > 0) {
incoming = Serial.readString();
if(incoming.equals("Right"))
{
  authorization = true;
  open_door();
  lcd.clear();
  time_now = millis();
  try_times= 0;
  return;
}
else if(incoming.equals("False"))
{
  strcpy(strnumber,"");
  strcpy(strnumber, "Wrong Password");
  try_times++;
  return;
}
}
}
}


void open_door(){
    pos= 90;
    myservo.write(pos);   
    time_now= millis();   
  } 


void close_door(){
  pos= 0;
  myservo.write(pos);
  }


void sound_on(){
  tone1.tone (220); 
  while(1) {  
    if(Serial.available()>0)
    {
      incoming= Serial.readString();
      if(incoming.equals("SoundOff"))
      return;
    }

  }
tone1.noTone ();
}

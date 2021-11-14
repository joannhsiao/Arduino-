#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//#include <NewPing.h>
#include <Keypad.h>
#include <Buzzer.h>
#include <String.h>

#define BLYNK_PRINT Serial
#include <ESP8266_Lib.h>
#include <Blynk.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include<SoftwareSerial.h>

char auth[] = "";
char ssid[] = "";
char pass[] = "";

#define EspSerial Serial1
SoftwareSerial EspSerial(2, 3); // RX, TX

#define ESP8266_BAUD 9600
ESP8266 wifi(&EspSerial);

LiquidCrystal_I2C lcd(0x27,20,4);

#define TRIGGER_PIN  A4
#define ECHO_PIN  A5
#define MAX_DISTANCE 200

#define redpin 3
#define yellowpin 4
#define greenpin 5

//NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

int trigPin = A0;
int echoPin = A1;
unsigned long duration, cm, pingcm;

const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1','4','7','*'},
  {'2','5','8','0'},
  {'3','6','9','#'},
  {'A','B','C','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {13, 12, 11, 10}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
Buzzer buzzer(2);

int Time=500,limit=5000;
String password="1111";

void lcdInit(){
  lcd.begin(16,2);
  for(int i=0; i<2; i++){
    lcd.backlight(); 
    delay(250);
    lcd.noBacklight();
    delay(250);
  }
}

void setup(){
  Serial.begin(9600);
  delay(10);
  EspSerial.begin(ESP8266_BAUD);
  delay(10);
  Blynk.begin(auth, wifi, ssid, pass);
  pinMode(redpin, OUTPUT);
  pinMode(yellowpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  
  lcdInit();
  delay(500);
  lcd.init();   //initialize the lcd
  lcd.backlight();    // Print a message to the LCD
  lcd.setCursor(3,0);
  lcd.print("CSIE's House.");
}

void loop(){
  Blynk.run();
  digitalWrite(yellowpin,0);
  digitalWrite(redpin,0);
  digitalWrite(greenpin,0);
  delay(500);
  distance();
  Blynk.virtualWrite(V1,(int)cm);
  delay(500);
  //pingSR04();
  if(cm < 5){
    digitalWrite(yellowpin,1);
    Blynk.virtualWrite(V2,"someone closed");
    lcd.print("Enter the password: ");
    readkey();
  }
 delay(500); 
}

void distance(){  
   pinMode(trigPin, OUTPUT);
   digitalWrite(trigPin, LOW);
   delayMicroseconds(2);
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin, LOW);
   pinMode(echoPin, INPUT);
   duration = pulseIn(echoPin, HIGH);
   cm = duration /29/2;  
}

/*void pingSR04(){
  Serial.print("Ping: ");
  pingcm = sonar.ping_cm();
  Serial.print(pingcm);
  Serial.println("cm");
}*/

void readkey(){
  String guess="";
  char key=0;
  lcd.setCursor(0,3);
  while(key!='#'){
    key = keypad.getKey();
    lcd.print(key);
    guess += key;
  }
  if(guess==password){
    digitalWrite(yellowpin,0);
    digitalWrite(greenpin,1);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Welcome home~~");
    Blynk.virtualWrite(V2,"safe,welcome!");
    delay(500);
  }
  else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Wrong password.");
    digitalWrite(yellowpin,0);
    digitalWrite(redpin,1);
    delay(200);
    Blynk.virtualWrite(V2,"warning!!there is a thief");
    lcd.clear();
    lcd.print("!!Warning!!");
    for(int i=1;i<4;i++){
      buzzer.sound(998,Time);
      delay(200);
    }
  }
}

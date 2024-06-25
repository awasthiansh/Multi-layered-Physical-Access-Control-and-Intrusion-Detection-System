#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include<SoftwareSerial.h>

SoftwareSerial gsmSerial(4,5);

Servo servo_1; // servo controller (multiple can exist)

LiquidCrystal_I2C lcd(0x27, 16, 2); // Format -> (Address,Width,Height )

int servo_pin = 3; // PWM pin for servo control
int pos = 0;    // servo starting position

const int buttonPin = 2;     // the number of the pushbutton pin
const int PIR_SENSOR_OUTPUT_PIN = 4;  /* PIR sensor O/P pin */
int warm_up;
 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int sensor_output;
bool valid_scan = 0;
bool  system_active = 1;
int buttonState = 0;    

void setup() 
{
  gsmSerial.begin(9600);
  pinMode(PIR_SENSOR_OUTPUT_PIN, INPUT);
  pinMode(buttonPin, INPUT);
  servo_1.attach(servo_pin); // start servo control
    // initialize the lcd
  lcd.init();
  // Turn on the Backlight
  lcd.backlight();
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

    // Clear the display buffer
  lcd.clear(); 
  // Set cursor (Column, Row)
  lcd.setCursor(0, 0);  
  lcd.print("Home Security"); 
  lcd.setCursor(0,1);
  lcd.print("System");

  delay(100);
  servo_1.write(20);              // tell servo to go to position in variable 'pos'

  delay(1000); // wait 5 seconds after arriving back at 0 degrees
  
}
void loop() 
{
    buttonState = digitalRead(buttonPin);
    delay(50);
    if (buttonState == HIGH) 
    {
      
      if(system_active == 1)
      {
        system_active = 0;
              lcd.clear(); 
      // Set cursor (Column, Row)
      lcd.setCursor(0, 0);  
      lcd.print("security system "); 
      lcd.setCursor(0,1);
      lcd.print("deactivate");
         delay(2000);
      }
       else
      {
         system_active = 1;
        lcd.clear(); 
      // Set cursor (Column, Row)
      lcd.setCursor(0, 0);  
      lcd.print("security system "); 
      lcd.setCursor(0,1);
      lcd.print("activate");
         delay(2000);
      }
    }

  sensor_output = digitalRead(PIR_SENSOR_OUTPUT_PIN);
  delay(50);
  if( sensor_output == LOW )
  {
    if( warm_up == 1 )
     {
      warm_up = 0;
      delay(200);
    }
  }
  else
  {    
    warm_up = 1;
    delay(1000);
    if(system_active == 1)
    {
      lcd.clear(); 
      // Set cursor (Column, Row)
      lcd.setCursor(0, 0);  
      lcd.print("unAuthorized "); 
      lcd.setCursor(0,1);
      lcd.print("access");
      delay(1000);
      SendMessage();
    }
  }  
      lcd.clear(); 
    // Set cursor (Column, Row)
    lcd.setCursor(0, 0);  
    lcd.print("Scan Your Card"); 
    delay(100);
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  //Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "1D F3 59 38") //change here the UID of the card/cards that you want to give access
  {
    system_active = 0;
    lcd.clear(); 
    // Set cursor (Column, Row)
    lcd.setCursor(0, 0);  
    lcd.print("Valid Access "); 
    lcd.setCursor(0,1);
    lcd.print("Card Detected");
    delay(1000);

    for (pos = 20; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo_1.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15); // delay to allow the servo to reach the desired position
  }
  delay(1000); // wait 5 seconds after reaching 180 degrees
 
  for (pos = 180; pos >= 20; pos -= 1) { // goes from 180 degrees to 0 degrees
    servo_1.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);
  }
  delay(1000); // wait 5 seconds after arriving back at 0 degrees
  
  }
 
 else  
 {
    lcd.clear(); 
    // Set cursor (Column, Row)
    lcd.setCursor(0, 0);  
    lcd.print("Invalid Access ");
    lcd.setCursor(0,1);
    lcd.print("Card Detected"); 
    delay(1000);
    SendMessage();
  }
} 

void SendMessage()
{
  gsmSerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  gsmSerial.println("AT+CMGS=\"+91249564442\"\r"); // Replace x with mobile number
  delay(1000);
  gsmSerial.println("unauthorized person enter in your house");// The SMS text you want to send
  delay(1000);
      gsmSerial.println((char)26);// ASCII code of CTRL+Z
  delay(100);

}
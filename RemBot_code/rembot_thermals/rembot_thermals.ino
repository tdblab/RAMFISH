//Temperature controller
//Designed and programmed by: Dr. Tirtha Das Banerjee,  Department of Biological Sciences and Yong Loo Lin School of Medicine, National University of Singapore
//Date first written: 6th Oct 2024
//Date last modified: 23rd March 2026

//Attach DHT library
//#include <DHT.h>
//Attach LiquidCrystal_I2C
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C display

//clock module
//#include <wire.h>
//#include <DS1307.h>
//DS1307 rtc(A4, A5);


#define RELAY_PIN 2        // pin connected to relay heater
#define RELAY_PIN2 3




int tempPin1 = A0;  //heater1          
int tempPin2 = A2;  //heater2

int tempPin3 = A1;  //chamber_temperature


void setup()
{                                            
  Serial.begin(9600); // initialize serial
  
  pinMode(RELAY_PIN, OUTPUT); // initialize relay pin as an output
  pinMode(RELAY_PIN2, OUTPUT);
 
  lcd.init();      // initialize the lcd
  lcd.backlight(); // open the backlight 

  // Initialize the rtc object
  //rtc.begin();
  
  // Set the clock to run-mode
  //rtc.halt(false);


   //rtc.setDOW(SUNDAY);        // Set Day-of-Week to Friday
  //rtc.setTime(22, 25, 0);    // Set the time to 11:30:00 (24hr format)
    //rtc.setDate(9, 02, 2025);
   
    // Set SQW/Out rate to 1Hz, and enable SQW
  //rtc.setSQWRate(SQW_RATE_1);
  //rtc.enableSQW(true);

 
}

void loop()
{
  //wait a second between measurements.
  delay(100);

  int tempReading = analogRead(tempPin1);
 
  double tempK = log(10000.0 * ((1024.0 / tempReading - 1)));
  tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK )) * tempK );       //  Temp Kelvin
  int tempC = tempK - 273.15;            // Convert Kelvin to Celcius
  float tempF = (tempC * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit

  int tempReading2 = analogRead(tempPin2);

  double tempK2 = log(10000.0 * ((1024.0 / tempReading2 - 1)));
  tempK2 = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK2 * tempK2 )) * tempK2 );       //  Temp Kelvin
  int tempC2 = tempK2 - 273.15;            // Convert Kelvin to Celcius
  float tempF2 = (tempC2 * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit


  int tempReading3 = analogRead(tempPin3);

  double tempK3 = log(10000.0 * ((1024.0 / tempReading3 - 1)));
  tempK3 = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK3 * tempK3 )) * tempK3 );       //  Temp Kelvin
  int tempC3 = tempK3 - 273.15;            // Convert Kelvin to Celcius
  float tempF3 = (tempC3 * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit

  
    lcd.clear();
  // check if any reads failed
  if (isnan(tempC)) {
    lcd.setCursor(0, 0);
    lcd.print("Failed");
  } 
  else {
    lcd.setCursor(0, 0);  
    lcd.print("T1:");
    lcd.print(tempC);     
    lcd.print((char)223); 
    lcd.print("C");

    lcd.setCursor(8, 0);  
    lcd.print("T2:");
    lcd.print(tempC2);     
    lcd.print((char)223); 
    lcd.print("C");

    lcd.setCursor(0, 1);  
    lcd.print("Plate Temp:");
    lcd.print(tempC3);     
    lcd.print((char)223); 
    lcd.print("C"); 

  }
  
   
    if(tempC2 < 40){
      digitalWrite(RELAY_PIN, HIGH); // turn on relay
      digitalWrite(RELAY_PIN2, HIGH);
      
    } 

    else {
      digitalWrite(RELAY_PIN, LOW); // turn off relay
      digitalWrite(RELAY_PIN2, LOW);
      delay(1000);
    }





}
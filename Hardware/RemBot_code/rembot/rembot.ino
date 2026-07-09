//L298N based 8 motor fluidics+thermal system for DNAseI based signal removal
//Programmed by: Dr. Tirtha Das Banerjee, Department of Biological Sciences and Yong Loo Lin School of Medicine, National Uuniversity of Singapore
//Date first written: 18th Aug 2024
//Date modified: 24th March 2026

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <math.h>

#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeMono12pt7b.h>                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

unsigned long startMillis = 0;

#define TFT_CS         47
#define TFT_DC         46
#define TFT_RST        45
//#define TFT_MOSI 51  // SDA
//#define TFT_SCLK 52 // SCL

// OR for the ST7789-based displays, we will use this call
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#define SKY_BLUE  tft.color565(135, 206, 235)

void drawCloud(int x, int y) {
  tft.fillCircle(x, y, 10, ST77XX_WHITE);
  tft.fillCircle(x + 10, y + 5, 12, ST77XX_WHITE);
  tft.fillCircle(x - 10, y + 5, 12, ST77XX_WHITE);
  tft.fillCircle(x, y + 10, 10, ST77XX_WHITE);
}

void drawBead(int x, int y, uint16_t color, int beadSize) {
  tft.fillCircle(x, y, beadSize, color);
}

void startupAnimation() {
  tft.fillScreen(SKY_BLUE);
  tft.setRotation(3);  // Landscape mode

  // Clouds positioned for 240x320 screen
  drawCloud(40, 40);
  drawCloud(180, 60);
  drawCloud(200, 30);
  drawCloud(90, 50);
  drawCloud(280, 40);

  // Text: Spatial BioSystems
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(120, 120);
  tft.println("Spatial");
  tft.setCursor(100, 150);
  tft.println("BioSystems");

  tft.setTextSize(2);
tft.setCursor(40, 220);
  tft.println("RemBot_V1");

  delay(3000); // Pause before spinner

  // Spinner (buffering beads)
  int centerX = 120;
  int centerY = 280;
  int radius = 30;
  int beadSize = 8;
  uint16_t colors[6] = {
    ST77XX_RED,
    ST77XX_YELLOW,
    ST77XX_GREEN,
    ST77XX_CYAN,
    ST77XX_BLUE,
    ST77XX_MAGENTA
  };

  for (int cycle = 0; cycle < 3; cycle++) {
    for (int i = 0; i < 10; i++) {
      int angle = i * 36 + (cycle * 10);  // Smooth rotation
      float rad = angle * 3.14159 / 180.0;
      int x = centerX + cos(rad) * radius;
      int y = centerY + sin(rad) * radius;

      drawBead(x, y, colors[i % 6], beadSize);
      delay(100);
      // Erase previous bead
      tft.fillCircle(x, y, beadSize, SKY_BLUE);
    }
  }
}

// Motor 1: SSCT 1
 
//int enA = 0;
int in11 = 14;
int in12 = 15;
 
// Motor 2: SSCT 1

//int enB = 1;
int in21 = 16;
int in22 = 17;
 

// Motor 3: SWB 1
 
//int enC = 2;
int in31 = 18;
int in32 = 19;
 
// Motor 4: SWB 1

//int enD = 3; 
int in41 = 20;
int in42 = 21;

// Motor 5: SRS 1
 
//int enE = 4;
int in51 = 22;
int in52 = 23;

// Motor 6: SRS 2
 
//int enF = 5;
int in61 = 24; 
int in62 = 25; 

// Motor 7: Waste 1
 
//int enG = 6;
int in71 = 26;
int in72 = 27;

// Motor 8: Waste 2
//int enG = 7;
int in81 = 28;
int in82 = 29;


//1 sample
const char rem_Sample1 = 10;
bool pressed1 = false;

//2 samples
const char rem_Sample2 = 9;
bool pressed2 = false;

//Calibrate
const char Calibrate = 8;
bool pressed3 = false;

//Clean
const char Clean = 7;
bool pressed4 = false;


#define relay1 1 //For heating main power

#define RESET_PIN 2


void setup(){

 // OR use this initializer (uncomment) if using a 2.0" 320x240 TFT:
  tft.init(240, 320);           // Init ST7789 240x320

 tft.setRotation(3);
 startupAnimation();
 /*
  tft.fillScreen(ST77XX_BLACK);
  tft.setFont(&FreeSans12pt7b);
  
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(30, 80);
  tft.println("RemBot_V1");
  tft.setTextSize(1);
tft.setCursor(50, 180);
  tft.println("Spatial Biosystems");
 delay(5000);
*/
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);
  tft.setFont(&FreeSans12pt7b);
  
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 80);
  tft.println("Developed by");
  tft.setTextSize(1);
tft.setCursor(40, 140);
  tft.println("Tirtha Das Banerjee");
 delay(5000);

  tft.fillScreen(ST77XX_BLACK);
  tft.setFont(&FreeSans12pt7b);
  
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(30, 80);
  tft.println("Welcome");
  tft.setTextSize(1);
tft.setCursor(30, 180);
  tft.println("System initializing...");
 delay(5000);

 pinMode (rem_Sample1, INPUT_PULLUP);
  pinMode (rem_Sample2, INPUT_PULLUP);
  pinMode (Calibrate, INPUT_PULLUP);
  pinMode (Clean, INPUT_PULLUP);

pinMode(in11, OUTPUT);
  pinMode(in12, OUTPUT);
  pinMode(in21, OUTPUT);
  pinMode(in22, OUTPUT);
  pinMode(in31, OUTPUT);
  pinMode(in32, OUTPUT);
  pinMode(in41, OUTPUT);
  pinMode(in42, OUTPUT);
  pinMode(in51, OUTPUT);
  pinMode(in52, OUTPUT);
  pinMode(in61, OUTPUT);
  pinMode(in62, OUTPUT);
  pinMode(in71, OUTPUT);
  pinMode(in72, OUTPUT);
  pinMode(in81, OUTPUT);
  pinMode(in82, OUTPUT);

pinMode(relay1, OUTPUT); 

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
}



void loop() {

bool currentState1 = digitalRead(rem_Sample1);
bool currentState2 = digitalRead(rem_Sample2);

bool currentState3 = digitalRead(Calibrate);
bool currentState4 = digitalRead(Clean);

startMillis = millis();

  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Press a button:");
tft.setCursor(30, 90);
  tft.println("# 1_sample");
tft.setCursor(30, 120);
  tft.println("# 2_samples");
tft.setCursor(30, 150);
  tft.println("# Calibrate");
  tft.setCursor(30, 180);
  tft.println("# Deep_Clean");
  delay(100);

if(currentState2 == pressed2)

{

//Adjust the values to increase or decrease the volume dispensed.
//These values are calibrated for GROTHEN 12V 7W peristaltic pumps

int a=20;//test ON
int b=20;//test OFF

int c=8;//inlet ON
int d=20;//inlet OFF

int e=8;//outlet1 ON
int f=20;//outlet1 OFF

int r=8;//outlet2 ON
int s=20;//outlet2 OFF

int g = 5; //For test

int h =1100; //for first dispensing

int p = 390; //for subsiquent dispensing

int q = 1300; //for removing buffer

int i;
int j;
int k;
int l;
int m;
int n;
int o;



tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Program: 2_samples");
tft.setCursor(30, 60);
  tft.println("Initiated");


showElapsedTime();

delay(2000);


tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pumps");


//Testing pump 1
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 1");

for (i=0; i<=g; i++){
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (a);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in11, HIGH);
  digitalWrite(in12, LOW);
  delay (a);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (b);
  }

showElapsedTime();



delay(2000);



//Testing pump 2
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 2");
  
for (i=0; i<=g; i++){
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (a);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in21, HIGH);
  digitalWrite(in22, LOW);
  delay (a);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);



//Testing pump 3
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 3");
  
for (i=0; i<=g; i++){
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (a);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in31, HIGH);
  digitalWrite(in32, LOW);
  delay (a);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);



//Testing pump 4
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 4");
  
for (i=0; i<=g; i++){
  digitalWrite(in41, LOW);
  digitalWrite(in42, HIGH);
  delay (a);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in41, HIGH);
  digitalWrite(in42, LOW);
  delay (a);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (b);
  }


showElapsedTime();

delay(2000);


//Testing pump 5
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 5");
  
for (i=0; i<=g; i++){
  digitalWrite(in51, LOW);
  digitalWrite(in52, HIGH);
  delay (a);
  digitalWrite(in51, LOW);
  digitalWrite(in52, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in51, HIGH);
  digitalWrite(in52, LOW);
  delay (a);
  digitalWrite(in51, LOW);
  digitalWrite(in52, LOW);
  delay (b);
  }


showElapsedTime();

delay(2000);



//Testing pump 6
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 6");
  
for (i=0; i<=g; i++){
  digitalWrite(in61, LOW);
  digitalWrite(in62, HIGH);
  delay (a);
  digitalWrite(in61, LOW);
  digitalWrite(in62, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in61, HIGH);
  digitalWrite(in62, LOW);
  delay (a);
  digitalWrite(in61, LOW);
  digitalWrite(in62, LOW);
  delay (b);
  }


showElapsedTime();

delay(2000);



//Testing pump 7
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 7");
  
for (i=0; i<=g; i++){
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (a);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in71, HIGH);
  digitalWrite(in72, LOW);
  delay (a);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (b);
  }


showElapsedTime();

delay(2000);

//Testing pump 8
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 8");
  
for (i=0; i<=g; i++){
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (a);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in81, HIGH);
  digitalWrite(in82, LOW);
  delay (a);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (b);
  }


showElapsedTime();

delay(2000);



tft.fillScreen(ST77XX_RED);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Heating");

  digitalWrite(relay1, HIGH);

showElapsedTime();

delay(10000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

tft.fillScreen(ST77XX_RED);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Preheating Plate");


showElapsedTime();

delay(30000);


// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);



tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 30);
  tft.println("Reaction starting");

showElapsedTime();

  delay(3000);

  //Add 5x SSCT
  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");


showElapsedTime();

  delay(2000);


  for(int j=0; j<3; j++){

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 1");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 2");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  }

    tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(40, 60);
  tft.println("5x SSCT");


showElapsedTime();

  delay(120000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }

  // Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

delay(500);



  for(int j=0; j<2; j++){

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 1");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 2");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

 tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(40, 60);
  tft.println("5x SSCT");


  // Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

showElapsedTime();

  delay(120000);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  }

 for(int j=0; j<3; j++){

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SWB");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 3");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (c);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SWB");
  tft.setCursor(30, 60);
  tft.println("In plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 4");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in41, LOW);
  digitalWrite(in42, HIGH);
  delay (c);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (d);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);


 }


 tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(60, 60);
  tft.println("SWB1");

showElapsedTime();

  delay(120000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);


for(int j=0; j<2; j++){

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SWB1");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 3");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (c);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (d);
  }
  
  // Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SWB1");
  tft.setCursor(30, 60);
  tft.println("In plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 4");

showElapsedTime();

for (i=0; i<=p; i++){ //p=400
  digitalWrite(in41, LOW);
  digitalWrite(in42, HIGH);
  delay (c);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (d);
  }
delay(500);

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(60, 60);
  tft.println("SWB1");

showElapsedTime();

  delay(120000);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }

  // Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

 }

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SRS");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 5");

showElapsedTime();

for (i=0; i<=h; i++){ 
  digitalWrite(in51, LOW);
  digitalWrite(in52, HIGH);
  delay (c);
  digitalWrite(in51, LOW);
  digitalWrite(in52, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SRS");
  tft.setCursor(30, 60);
  tft.println("In plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 6");

showElapsedTime();

for (i=0; i<=h; i++){ 
  digitalWrite(in61, LOW);
  digitalWrite(in62, HIGH);
  delay (c);
  digitalWrite(in61, LOW);
  digitalWrite(in62, LOW);
  delay (d);
  }
delay(500);

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing signal using");
  tft.setCursor(90, 60);
  tft.println("SRS"); //for 1 hrs, increase time if working on samples thicker than 500 um

for(int x=0; x<120; x++){

  

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing signal using");
  tft.setCursor(90, 60);
  tft.println("SRS");

showElapsedTime();

  delay(30000);

  // Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

}



  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }
delay(500);



for(int j=0; j<6; j++){

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SWB");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 3");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (c);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SWB");
  tft.setCursor(30, 60);
  tft.println("In plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 4");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in41, LOW);
  digitalWrite(in42, HIGH);
  delay (c);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (d);
  }
delay(500);

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(60, 60);
  tft.println("SWB1");

showElapsedTime();

  delay(60000);

  // Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

 }


for(int j=0; j<3; j++){

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 1");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 2");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

 tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(40, 60);
  tft.println("5x SSCT");


  // Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

showElapsedTime();

  delay(60000);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  }

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 1");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 2");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

digitalWrite(relay1, LOW);

 tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(50, 30);
  tft.println("Congratulations");
  tft.setCursor(30, 60);
  tft.println("Signal removal successful");
  tft.setCursor(30, 120);
  tft.println("Remove samples");

//Time elapsed
tft.setCursor(10, 210);
 tft.println("Total time:");
  tft.setCursor(160, 210);
  tft.println((millis() - startMillis)/1000);
  tft.setCursor(230, 210);
  tft.println("secs");



for (int j=0; j<=99999999999; j++){

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

delay(30000);
}


}




if(currentState1 == pressed1)
{

//Adjust the values to increase or decrease the volume dispensed.
//These values are calibrated for GROTHEN 12V 7W peristaltic pumps

  int a=20;//test ON
int b=20;//test OFF

int c=8;//inlet ON
int d=20;//inlet OFF

int e=8;//outlet1 ON
int f=20;//outlet1 OFF

int r=8;//outlet2 ON
int s=20;//outlet2 OFF

int g = 5; //For test

int h =1100; //for first dispensing

int p = 400; //for subsiquent dispensing

int q = 1300; //for removing buffer

int i;
int j;
int k;
int l;
int m;
int n;
int o;



tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Program: 1_sample");
tft.setCursor(30, 60);
  tft.println("Initiated");


showElapsedTime();

delay(2000);


tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pumps");


//Testing pump 1
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 1");

for (i=0; i<=g; i++){
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (a);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in11, HIGH);
  digitalWrite(in12, LOW);
  delay (a);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);


//Testing pump 2
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 2");
  
for (i=0; i<=g; i++){
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (a);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in21, HIGH);
  digitalWrite(in22, LOW);
  delay (a);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);



//Testing pump 3
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 3");
  
for (i=0; i<=g; i++){
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (a);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in31, HIGH);
  digitalWrite(in32, LOW);
  delay (a);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);



//Testing pump 4
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 4");
  
for (i=0; i<=g; i++){
  digitalWrite(in41, LOW);
  digitalWrite(in42, HIGH);
  delay (a);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in41, HIGH);
  digitalWrite(in42, LOW);
  delay (a);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (b);
  }


showElapsedTime();

delay(2000);


//Testing pump 5
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 5");
  
for (i=0; i<=g; i++){
  digitalWrite(in51, LOW);
  digitalWrite(in52, HIGH);
  delay (a);
  digitalWrite(in51, LOW);
  digitalWrite(in52, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in51, HIGH);
  digitalWrite(in52, LOW);
  delay (a);
  digitalWrite(in51, LOW);
  digitalWrite(in52, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);



//Testing pump 6
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 6");
  
for (i=0; i<=g; i++){
  digitalWrite(in61, LOW);
  digitalWrite(in62, HIGH);
  delay (a);
  digitalWrite(in61, LOW);
  digitalWrite(in62, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in61, HIGH);
  digitalWrite(in62, LOW);
  delay (a);
  digitalWrite(in61, LOW);
  digitalWrite(in62, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);



//Testing pump 7
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 7");
  
for (i=0; i<=g; i++){
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (a);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in71, HIGH);
  digitalWrite(in72, LOW);
  delay (a);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);

//Testing pump 8
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 8");
  
for (i=0; i<=g; i++){
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (a);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in81, HIGH);
  digitalWrite(in82, LOW);
  delay (a);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);



tft.fillScreen(ST77XX_RED);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Heating");

  digitalWrite(relay1, HIGH);

showElapsedTime();

delay(10000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

tft.fillScreen(ST77XX_RED);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Preheating Plate");

showElapsedTime();

delay(30000);


// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);



tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 30);
  tft.println("Reaction starting");

showElapsedTime();

  delay(3000);

  //Add 5x SSCT
  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");

showElapsedTime();

  delay(2000);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 1");

showElapsedTime();

for (i=0; i<=h; i++){ 
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
  
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

    tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(40, 60);
  tft.println("5x SSCT");


showElapsedTime();

  delay(120000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
delay(500);

  for(int j=0; j<2; j++){

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 1");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
  
delay(500);


 tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(40, 60);
  tft.println("5x SSCT");


  // Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

showElapsedTime();

  delay(120000);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);



// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  }


tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SWB");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 3");

showElapsedTime();

for (i=0; i<=h; i++){ 
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (c);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (d);
  }
  
  pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

delay(500);

 tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(60, 60);
  tft.println("SWB");

showElapsedTime();

  delay(120000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  
delay(500);



// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);


for(int j=0; j<2; j++){

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SWB");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 3");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (c);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (d);
  }
  
  
delay(500);


tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(60, 60);
  tft.println("SWB");

showElapsedTime();

  delay(120000);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
delay(500);


// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

 }

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SRS");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 5");

showElapsedTime();

for (i=0; i<=h; i++){ 
  digitalWrite(in51, LOW);
  digitalWrite(in52, HIGH);
  delay (c);
  digitalWrite(in51, LOW);
  digitalWrite(in52, LOW);
  delay (d);
  }
  
delay(500);

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing signal using");
  tft.setCursor(90, 60);
  tft.println("SRS"); //for 1 hrs

for(int x=0; x<120; x++){
  
  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing signal using");
  tft.setCursor(90, 60);
  tft.println("SRS");

showElapsedTime();

  delay(30000);

  // Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);



}


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
delay(500);


for(int j=0; j<6; j++){

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SWB");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 3");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (c);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (d);
  }
  
delay(500);


tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(60, 60);
  tft.println("SWB");

showElapsedTime();

  delay(60000);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
delay(500);


// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

 }


for(int j=0; j<3; j++){

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 1");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
  
  // Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

delay(500);


 tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(40, 60);
  tft.println("5x SSCT");

showElapsedTime();

  delay(60000);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  // Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);
  
delay(500);





  }

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 1");

showElapsedTime();

for (i=0; i<=p; i++){ //p=400
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
  
  
delay(500);



digitalWrite(relay1, LOW);

 tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(50, 30);
  tft.println("Congratulations");
  tft.setCursor(30, 60);
  tft.println("Signal removal successful");
  tft.setCursor(30, 120);
  tft.println("Remove samples");

//Time elapsed
tft.setCursor(10, 210);
 tft.println("Total time:");
  tft.setCursor(160, 210);
  tft.println((millis() - startMillis)/1000);
  tft.setCursor(230, 210);
  tft.println("secs");



for (int j=0; j<=9999999999; j++){
// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);
delay(30000);

}

}

if(currentState3 == pressed3)

{

//Adjust the values to increase or decrease the volume dispensed.
//These values are calibrated for GROTHEN 12V 7W peristaltic pumps
int a=20;//test ON
int b=20;//test OFF

int c=8;//inlet ON
int d=20;//inlet OFF

int e=8;//outlet1 ON
int f=20;//outlet1 OFF

int r=8;//outlet2 ON
int s=20;//outlet2 OFF

int g = 5; //For test

int h =1100; //for first dispensing

int p = 390; //for subsiquent dispensing

int q = 1300; //for removing buffer

int i;
int j;
int k;
int l;
int m;
int n;
int o;


tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Program: Calibration");
tft.setCursor(30, 60);
  tft.println("Initiated");

showElapsedTime();

delay(2000);


tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pumps");


//Testing pump 1
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 1");

for (i=0; i<=g; i++){
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (a);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in11, HIGH);
  digitalWrite(in12, LOW);
  delay (a);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);


//Testing pump 2
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 2");
  
for (i=0; i<=g; i++){
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (a);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in21, HIGH);
  digitalWrite(in22, LOW);
  delay (a);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);


//Testing pump 3
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 3");
  
for (i=0; i<=g; i++){
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (a);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in31, HIGH);
  digitalWrite(in32, LOW);
  delay (a);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);



//Testing pump 4
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 4");
  
for (i=0; i<=g; i++){
  digitalWrite(in41, LOW);
  digitalWrite(in42, HIGH);
  delay (a);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in41, HIGH);
  digitalWrite(in42, LOW);
  delay (a);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);


//Testing pump 5
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 5");
  
for (i=0; i<=g; i++){
  digitalWrite(in51, LOW);
  digitalWrite(in52, HIGH);
  delay (a);
  digitalWrite(in51, LOW);
  digitalWrite(in52, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in51, HIGH);
  digitalWrite(in52, LOW);
  delay (a);
  digitalWrite(in51, LOW);
  digitalWrite(in52, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);



//Testing pump 6
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 6");
  
for (i=0; i<=g; i++){
  digitalWrite(in61, LOW);
  digitalWrite(in62, HIGH);
  delay (a);
  digitalWrite(in61, LOW);
  digitalWrite(in62, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in61, HIGH);
  digitalWrite(in62, LOW);
  delay (a);
  digitalWrite(in61, LOW);
  digitalWrite(in62, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);



//Testing pump 7
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 7");
  
for (i=0; i<=g; i++){
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (a);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in71, HIGH);
  digitalWrite(in72, LOW);
  delay (a);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);

//Testing pump 8
tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Pump: 8");
  
for (i=0; i<=g; i++){
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (a);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (b);
  }
  for (i=0; i<=g; i++){
  digitalWrite(in81, HIGH);
  digitalWrite(in82, LOW);
  delay (a);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (b);
  }

showElapsedTime();

delay(2000);



tft.fillScreen(ST77XX_RED);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Testing Heating");

  digitalWrite(relay1, HIGH);

showElapsedTime();

delay(10000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

tft.fillScreen(ST77XX_RED);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Preheating Plate");

showElapsedTime();

delay(30000);


// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);



tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 30);
  tft.println("Reaction starting");

showElapsedTime();

  delay(3000);

  //Add 5x SSCT
  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");

showElapsedTime();

  delay(2000);

  for(int j=0; j<3; j++){

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 1");

showElapsedTime();

for (i=0; i<=p; i++){ //p=400
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 2");

showElapsedTime();

for (i=0; i<=p; i++){ //p=400
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  }

    tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(40, 60);
  tft.println("5x SSCT");


showElapsedTime();

  delay(12000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }
delay(500);


  for(int j=0; j<2; j++){

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 1");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 2");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

 tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(40, 60);
  tft.println("5x SSCT");


  // Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

showElapsedTime();

  delay(12000);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  }

 for(int j=0; j<3; j++){

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SWB");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 3");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (c);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SWB");
  tft.setCursor(30, 60);
  tft.println("In plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 4");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in41, LOW);
  digitalWrite(in42, HIGH);
  delay (c);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (d);
  }
delay(500);

 }


 tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(60, 60);
  tft.println("SWB1");

showElapsedTime();

  delay(12000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);


for(int j=0; j<2; j++){

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SWB");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 3");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (c);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SWB1");
  tft.setCursor(30, 60);
  tft.println("In plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 4");

showElapsedTime();

for (i=0; i<=p; i++){ //p=400
  digitalWrite(in41, LOW);
  digitalWrite(in42, HIGH);
  delay (c);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (d);
  }
delay(500);

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(60, 60);
  tft.println("SWB1");

showElapsedTime();

  delay(12000);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

 }

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SRS");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 5");

showElapsedTime();

for (i=0; i<=h; i++){ 
  digitalWrite(in51, LOW);
  digitalWrite(in52, HIGH);
  delay (c);
  digitalWrite(in51, LOW);
  digitalWrite(in52, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SRS");
  tft.setCursor(30, 60);
  tft.println("In plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 6");

showElapsedTime();

for (i=0; i<=h; i++){ 
  digitalWrite(in61, LOW);
  digitalWrite(in62, HIGH);
  delay (c);
  digitalWrite(in61, LOW);
  digitalWrite(in62, LOW);
  delay (d);
  }
delay(500);

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing signal using");
  tft.setCursor(90, 60);
  tft.println("SRS"); //for 30 seconds

for(int x=0; x<1; x++){
  

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing signal using");
  tft.setCursor(90, 60);
  tft.println("SRS");

showElapsedTime();

  delay(3000);

  // Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

}



  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }
delay(500);



for(int j=0; j<6; j++){

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SWB");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 3");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (c);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding SWB");
  tft.setCursor(30, 60);
  tft.println("In plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 4");

showElapsedTime();

for (i=0; i<=p; i++){ //p=400
  digitalWrite(in41, LOW);
  digitalWrite(in42, HIGH);
  delay (c);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (d);
  }
delay(500);

tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(60, 60);
  tft.println("SWB1");

showElapsedTime();

  delay(10000);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

 }


for(int j=0; j<3; j++){

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 1");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 2");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

 tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Washing samples in");
  tft.setCursor(40, 60);
  tft.println("5x SSCT");


  // Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

showElapsedTime();

  delay(10000);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  }

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 1");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Adding 5x SSCT");
  tft.setCursor(30, 60);
  tft.println("In plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 2");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

digitalWrite(relay1, LOW);

 tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(50, 30);
  tft.println("Congratulations");
  tft.setCursor(30, 60);
  tft.println("Calibration successful");
 

//Time elapsed
tft.setCursor(10, 210);
 tft.println("Total time:");
  tft.setCursor(160, 210);
  tft.println((millis() - startMillis)/1000);
  tft.setCursor(230, 210);
  tft.println("secs");

for (int j=0; j<=9999999999; j++){
// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);
delay(30000);

}

}

if(currentState4 == pressed4)

{

//Adjust the values to increase or decrease the volume dispensed.
//These values are calibrated for GROTHEN 12V 7W peristaltic pumps

int a=20;//test ON
int b=20;//test OFF

int c=10;//inlet ON
int d=20;//inlet OFF

int e=10;//outlet1 ON
int f=20;//outlet1 OFF

int r=10;//outlet2 ON
int s=20;//outlet2 OFF

int g = 5; //For test

int h =500; //for first dispensing

int p = 200; //for subsiquent dispensing

int q = 700; //for removing buffer

int i;
int j;
int k;
int l;
int m;
int n;
int o;



tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Program: Deep Cleaning");
tft.setCursor(30, 60);
  tft.println("Initiated");


showElapsedTime();

delay(2000);

for(int k=0; k<2; k++){

  for(int j=0; j<4; j++){

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Flushing");
  tft.setCursor(30, 60);
  tft.println("Pump 1");
  

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Flushing");
  tft.setCursor(30, 60);
  tft.println("Pump 2");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  }

  delay(1000);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }
delay(500);




 

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Flushing");
  tft.setCursor(30, 60);
  tft.println("Pump 5");
  

showElapsedTime();

for (i=0; i<=h; i++){ 
  digitalWrite(in51, LOW);
  digitalWrite(in52, HIGH);
  delay (c);
  digitalWrite(in51, LOW);
  digitalWrite(in52, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Flushing");
  tft.setCursor(30, 60);
  tft.println("Pump 6");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in61, LOW);
  digitalWrite(in62, HIGH);
  delay (c);
  digitalWrite(in61, LOW);
  digitalWrite(in62, LOW);
  delay (d);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  }

  delay(1000);



  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }
delay(500);

 for(int j=0; j<4; j++){

  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Flushing");
  tft.setCursor(30, 60);
  tft.println("Pump 3");
  

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (c);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (d);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Flushing");
  tft.setCursor(30, 60);
  tft.println("Pump 4");

showElapsedTime();

for (i=0; i<=p; i++){ 
  digitalWrite(in41, LOW);
  digitalWrite(in42, HIGH);
  delay (c);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (d);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);

  }

  delay(1000);



  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 1");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 7");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }
  
  
delay(500);


  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 30);
  tft.println("Removing buffer");
  tft.setCursor(30, 60);
  tft.println("from plate 2");
  tft.setCursor(30, 90);
  tft.println("Using: Pump 8");

showElapsedTime();

for (i=0; i<=q; i++){ 
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (r);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (s);
  }
delay(500);



tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(50, 30);
  tft.println("Congratulations");
  tft.setCursor(30, 60);
  tft.println("Cleaning successful");
 

//Time elapsed
tft.setCursor(10, 210);
 tft.println("Total time:");
  tft.setCursor(160, 210);
  tft.println((millis() - startMillis)/1000);
  tft.setCursor(230, 210);
  tft.println("secs");

for (int j=0; j<=9999999999; j++){
// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT);
delay(30000);

}

}


} //end of main loop

void showElapsedTime() {
//Time elapsed
tft.setCursor(10, 210);
 tft.println("Time elapsed:");
  tft.setCursor(160, 210);
  tft.println((millis() - startMillis)/1000);
  tft.setCursor(230, 210);
  tft.println("secs");
  }
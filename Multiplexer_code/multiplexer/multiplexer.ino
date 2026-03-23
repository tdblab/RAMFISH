//L298N motor driver based 18 Pump in-situ hybridization robot_glass_chamber
//Designed and programmed by: Dr. Tirtha Das Banerjee,  Department of Biological Sciences and Yong Loo Lin School of Medicine, National University of Singapore
//Date first written: 15th Jan 2024
//Date last modified: 24th March 2026

//Include library for liquid crystal display
//#include <LiquidCrystal_I2C.h>
//LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C first display
//LiquidCrystal_I2C lcd2(0x26, 16, 2); // I2C second display

//For the LCD-TFT display
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_SPI.h> //Hardware-specific library

//paramters define
#define MODEL ST7796S //Controller of display
//#define MODEL ILI9341
#define CS   53    
#define CD   48 //RS
#define RST  49
#define LED  61  
//#define TFT_MOSI 51 //SDA
//#define TFT_MISO 50 
//#define TFT_SCLK 52  //SCK


//Touchscreen control pins

//#define CTP_SCL 60
//#define CTP_RST 59
//#define CTP_SDA 58
//#define CTP_INT 57
//#define SD_CS 56

LCDWIKI_SPI mylcd(MODEL,CS,CD,RST,LED); 

//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);



// Motor 1: SSCT 1
//int enA = 0;
int in11 = 14;
int in12 = 15;
 
// Motor 2: SSCT 2
//int enB = 1;
int in21 = 16;
int in22 = 17;
 
// Motor 3: PWB 1
//int enC = 2;
int in31 = 18;
int in32 = 19;
 
// Motor 4: PWB 2
//int enD = 3; 
int in41 = 20;
int in42 = 21;

// Motor 5: AMP BUF 1
 
//int enE = 4;
int in51 = 22;
int in52 = 23;

// Motor 6: AMP BUF 2
 
//int enF = 5;
int in61 = 24; 
int in62 = 25; 

// Motor 7: SEC COLLECT 1
 
//int enG = 6;
int in71 = 27;
int in72 = 26;

// Motor 8: SEC COLLECT 2
 
//int enH = 7;
int in81 = 28; 
int in82 = 29; 

// Motor 9: WASTE 1
 
//int enI = 8;
int in91 = 30; 
int in92 = 31;

// Motor 10: PRIMARY 1
 
//int enJ = 9;
int in101 = 33; 
int in102 = 32; 

// Motor 11: SECONDARY 1
 
//int enK = 10;
int in111 = 34;
int in112 = 35;

// Motor 12: PRIMARY 2
 
//int enL = 11;
int in121 = 36;
int in122 = 37;

// Motor 13: SECONDARY 2
 
//int enM = 12;
int in131 = 38;
int in132 = 39;

// Motor 14: DAPI 1
 
//int enM = 14;
int in141 = 40;
int in142 = 41;

// Motor 15: DAPI 2
 
//int enM = 15;
int in151 = 42;
int in152 = 43;

// Motor 16: IMG BUF 1
 
//int enM = 16;
int in161 = 44;
int in162 = 45;

// Motor 17: IMG BUF 2
 
//int enM = 17;
int in171 = 6;
int in172 = 5;

// Motor 18: WASTE 2
 
//int enM = 18;
int in181 = 4;
int in182 = 3;

//HCR
//1 sample
const char Sample1 = 10;
bool pressed1 = false;

//2 samples
const char Sample2 = 9;
bool pressed2 = false;

//Quick clean: Cleans only the primary probe tube
const char Calibrate = 8;
bool pressed3 = false;

//Through clean: Cleans every tube
const char Clean = 7;
bool pressed4 = false;



//relay to control the heating from second controller
//#define relay0 A0 //For heating relay fluctuation
#define relay1 A1 //For heating main power
//#define relay2 A14 //For cooling

#define RESET_PIN 2

//define some colour values
#define SKY_BLUE    0x667F
#define WHITE       0xFFFF
#define LIGHT_GREY  0xC618
#define DARK_GREY   0x7BEF
#define CLOUD_COLOR 0xF79E
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xFD20

#define DARKGREEN    0x0320  // Dark green
#define LIGHTBLUE    0xAEDC  // Light blue
#define GRAY         0x8410  // Gray
#define DARKGRAY     0x4208  // Dark gray
#define LIGHTGRAY    0xC618  // Light gray
#define GOLD         0xFEA0  // Gold
#define LIME         0x07E0  // Pure green
#define TEAL         0x0410  // Teal
#define VIOLET       0x915C  // Violet
#define BEIGE        0xF7BB  // Beige
#define SKYBLUE      0x867D  // Sky blue
#define NAVY         0x000F  // Navy blue
#define OLIVE        0x7BE0  // Olive green
#define MAROON       0x7800  // Maroon
#define AQUA         0x5D1C  // Aqua
#define TURQUOISE    0x471A  // Turquoise


void drawCloud(int x, int y) {
  mylcd.Set_Draw_color(CLOUD_COLOR);
  mylcd.Fill_Circle(x, y, 10);
  mylcd.Fill_Circle(x + 10, y - 5, 12);
  mylcd.Fill_Circle(x + 20, y, 10);
  mylcd.Fill_Circle(x + 30, y, 10);
  mylcd.Fill_Rectangle(x - 2, y, x + 22, y + 10);
}

void drawBead(int x, int y, uint16_t color, int size) {
  mylcd.Set_Draw_color(color);
  mylcd.Fill_Circle(x, y, size);
}

//void fill_screen_test()
//{
 // mylcd.Fill_Screen(BLACK);  
// mylcd.Fill_Screen(RED);
//  mylcd.Fill_Screen(GREEN);
//  mylcd.Fill_Screen(BLUE);
//  mylcd.Fill_Screen(BLACK);
//}

void setup()
 
{
       mylcd.Init_LCD();
  mylcd.Fill_Screen(SKY_BLUE);
mylcd.Set_Text_Back_colour(SKY_BLUE);
mylcd.Set_Rotation(3);

mylcd.Set_Rotation(3);

 // Clouds
  drawCloud(40, 40);
  drawCloud(200, 60);
  drawCloud(120, 30);
   drawCloud(140, 90);
  drawCloud(420, 30);
  drawCloud(350, 40);
drawCloud(250, 40);
  drawCloud(380, 90);
  drawCloud(300, 60);

  // Boot Text - "Spatial BioSystems"
  mylcd.Set_Text_colour(BLUE);
  
  mylcd.Set_Text_Size(3); // Large text
  mylcd.Print_String("Spatial", 240, 190);  
  mylcd.Print_String("BioSystems", 220, 240);


  delay(1000); // Pause for a moment before starting animation

  // Animate buffering beads with larger size and smooth rotation
  int centerX = 100;
  int centerY = 220;
  int radius = 40; // Increase the radius for larger beads
  int beadSize = 12; // Larger beads
  uint16_t colors[6] = {
    0xF800, // Red
    0xFFE0, // Yellow
    0x07E0, // Green
    0x07FF, // Cyan
    0x001F, // Blue
    0xF81F  // Magenta
  };

for (int cycle = 0; cycle < 3; cycle++) {
    for (int i = 0; i < 10; i++) {
      int angle = i * 30 + (cycle * 5); // Smooth rotation effect with offset
      float rad = angle * 3.14 / 180;
      int x = centerX + cos(rad) * radius;
      int y = centerY + sin(rad) * radius;

      drawBead(x, y, colors[i], beadSize); // Draw the bead

      delay(100); // Smooth transition between beads

      // Erase bead for next rotation cycle
      mylcd.Set_Draw_color(SKY_BLUE);
      mylcd.Fill_Circle(x, y, beadSize); // Clear the previous bead
    }
  }

  // Final static beads once the animation ends
  for (int i = 0; i < 6; i++) {
    int angle = i * 60; 
    float rad = angle * 3.14 / 180;
    int x = centerX + cos(rad) * radius;
    int y = centerY + sin(rad) * radius;
    drawBead(x, y, colors[i], beadSize);
  }
  delay(2000);
   mylcd.Set_Text_colour(BLACK);
  
  mylcd.Set_Text_Size(3); // Large text
  mylcd.Print_String("Multiplexer_V1", 200, 190);  
  mylcd.Print_String("Software: MP_1.0", 180, 240);
delay(3000);

  

mylcd.Set_Text_Size(3);
  mylcd.Fill_Screen(CYAN); 
mylcd.Set_Text_Back_colour(CYAN);
  mylcd.Print_String("Developed By", 120, 100);
  mylcd.Print_String("Tirtha Das Banerjee", 80, 150);
delay(2000);

  pinMode (Sample1, INPUT_PULLUP);
  pinMode (Sample2, INPUT_PULLUP);
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
  pinMode(in91, OUTPUT);
  pinMode(in92, OUTPUT);
  pinMode(in101, OUTPUT);
  pinMode(in102, OUTPUT);
  pinMode(in111, OUTPUT);
  pinMode(in112, OUTPUT);
  pinMode(in121, OUTPUT);
  pinMode(in122, OUTPUT);
  pinMode(in131, OUTPUT);
  pinMode(in132, OUTPUT);
  pinMode(in141, OUTPUT);
  pinMode(in142, OUTPUT);
  pinMode(in151, OUTPUT);
  pinMode(in152, OUTPUT);
  pinMode(in161, OUTPUT);
  pinMode(in162, OUTPUT);
  pinMode(in171, OUTPUT);
  pinMode(in172, OUTPUT);
  pinMode(in181, OUTPUT);
  pinMode(in182, OUTPUT);



pinMode(relay1, OUTPUT); 
//pinMode(relay2, OUTPUT);

//fill_screen_test();
 // delay(100);

    mylcd.Fill_Screen(GREEN); 

}


void loop() {

  bool currentState1 = digitalRead(Sample1);
bool currentState2 = digitalRead(Sample2);

bool currentState3 = digitalRead(Calibrate);
bool currentState4 = digitalRead(Clean);


mylcd.Set_Text_colour(BLACK);
  mylcd.Set_Text_Size(3);
   mylcd.Set_Text_Back_colour(GREEN);

  mylcd.Print_String("Press a button", 100, 50);

  mylcd.Print_String("# 1_Sample", 20, 100);
  mylcd.Print_String("# 2_Samples", 20, 150);


  mylcd.Set_Text_colour(BLACK);
  mylcd.Print_String("# Calibrate", 20, 200);
  mylcd.Print_String("# Deep_Clean", 20, 250);
delay(50);



//Single sample 
if(currentState1 == pressed1)
  {

//Adjust the values to increase or decrease the volume dispensed.
//These values are calibrated for GROTHEN 12V 7W peristaltic pumps
int a=10;//test ON
int b=10;//test OFF

int c=7;//inlet ON
int d=20;//inlet OFF
int e=8;//outlet ON
int f=20;//outlet OFF

int g = 5; //For test

int h =1500; //for first dispensing of specific buffers
int u = 1200; //for first dispensing of common buffers (SSCT and PWB)
int p = 320; //for subsiquent dispensing

int q = 1500; //for removing buffer

int i;
int j;
int k;
int l;
int m;
int n;
int o;

  mylcd.Fill_Screen(WHITE); 
mylcd.Set_Text_Back_colour(WHITE);
  mylcd.Set_Text_colour(BLACK);  
  mylcd.Set_Text_Size(4);

  
mylcd.Print_String("Program: 1_FISH", 40, 90);// put your main code here, to run repeatedly:
mylcd.Set_Text_Size(4);
mylcd.Print_String("Initiated", 120, 150);
mylcd.Print_String("1 Sample HCR", 80, 250);
delay(5000);

mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pumps", 30, 120);


//Testing pump 1
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 1", 60, 120);

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
delay(200);


//Testing pump 2
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 2", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (a);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (b);
  }
  for (i=0; i<=5; i++){
  digitalWrite(in21, HIGH);
  digitalWrite(in22, LOW);
  delay (a);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (b);
  }
delay(200);


//Testing pump 3
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 3", 60, 120);

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
delay(200);

//Testing pump 4
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 4", 60, 120);

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
delay(200);

//Testing pump 5
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 5", 60, 120);

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
delay(200);

//Testing pump 6
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 6", 60, 120);

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
delay(200);

//Testing pump 7
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 7", 60, 120);

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
delay(200);

//Testing pump 8
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 8", 60, 120);

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
delay(200);

//Testing pump 9
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 9", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (a);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in91, HIGH);
  digitalWrite(in92, LOW);
  delay (a);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (b);
  }
delay(200);

//Testing pump 10
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 10", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in101, LOW);
  digitalWrite(in102, HIGH);
  delay (a);
  digitalWrite(in101, LOW);
  digitalWrite(in102, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in101, HIGH);
  digitalWrite(in102, LOW);
  delay (a);
  digitalWrite(in101, LOW);
  digitalWrite(in102, LOW);
  delay (b);
  }
delay(200);

//Testing pump 11
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 11", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in111, LOW);
  digitalWrite(in112, HIGH);
  delay (a);
  digitalWrite(in111, LOW);
  digitalWrite(in112, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in111, HIGH);
  digitalWrite(in112, LOW);
  delay (a);
  digitalWrite(in111, LOW);
  digitalWrite(in112, LOW);
  delay (b);
  }
delay(200);

//Testing pump 12
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 12", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in121, LOW);
  digitalWrite(in122, HIGH);
  delay (a);
  digitalWrite(in121, LOW);
  digitalWrite(in122, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in121, HIGH);
  digitalWrite(in122, LOW);
  delay (a);
  digitalWrite(in121, LOW);
  digitalWrite(in122, LOW);
  delay (b);
  }
delay(200);

//Testing pump 13
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 13", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in131, LOW);
  digitalWrite(in132, HIGH);
  delay (a);
  digitalWrite(in131, LOW);
  digitalWrite(in132, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in131, HIGH);
  digitalWrite(in132, LOW);
  delay (a);
  digitalWrite(in131, LOW);
  digitalWrite(in132, LOW);
  delay (b);
  }
delay(200);

//Testing pump 14
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 14", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in141, LOW);
  digitalWrite(in142, HIGH);
  delay (a);
  digitalWrite(in141, LOW);
  digitalWrite(in142, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in141, HIGH);
  digitalWrite(in142, LOW);
  delay (a);
  digitalWrite(in141, LOW);
  digitalWrite(in142, LOW);
  delay (b);
  }
delay(200);

//Testing pump 15
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 15", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in151, LOW);
  digitalWrite(in152, HIGH);
  delay (a);
  digitalWrite(in151, LOW);
  digitalWrite(in152, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in151, HIGH);
  digitalWrite(in152, LOW);
  delay (a);
  digitalWrite(in151, LOW);
  digitalWrite(in152, LOW);
  delay (b);
  }
delay(200);

//Testing pump 16
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 16", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in161, LOW);
  digitalWrite(in162, HIGH);
  delay (a);
  digitalWrite(in161, LOW);
  digitalWrite(in162, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in161, HIGH);
  digitalWrite(in162, LOW);
  delay (a);
  digitalWrite(in161, LOW);
  digitalWrite(in162, LOW);
  delay (b);
  }
delay(200);

//Testing pump 17
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 17", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in171, LOW);
  digitalWrite(in172, HIGH);
  delay (a);
  digitalWrite(in171, LOW);
  digitalWrite(in172, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in171, HIGH);
  digitalWrite(in172, LOW);
  delay (a);
  digitalWrite(in171, LOW);
  digitalWrite(in172, LOW);
  delay (b);
  }
delay(200);

//Testing pump 18
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 18", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (a);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in181, HIGH);
  digitalWrite(in182, LOW);
  delay (a);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (b);
  }
delay(200);


mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Pump Testing Done", 30, 120);
delay(500);



// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)




mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Thermals", 60, 120);
delay(500);

/*
//Testing Cooling
mylcd.Fill_Screen(BLUE);
mylcd.Set_Text_Back_colour(BLUE);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Cooling", 60, 120);


digitalWrite(relay2, HIGH);

delay(10000);

digitalWrite(relay2, LOW);
*/


//Testing heating
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Heating", 60, 120);

//digitalWrite(relay0, HIGH);
delay(1000);
digitalWrite(relay1, HIGH);

delay(10000);

//digitalWrite(relay0, LOW);
delay(1000);
digitalWrite(relay1, LOW);


//Testing done
mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Done", 70, 120);
delay(1000);


//Preheating
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Preheating plate", 60, 120);

mylcd.Print_String("Please Wait", 110, 180);

//digitalWrite(relay0, HIGH);
delay(1000);
digitalWrite(relay1, HIGH);

delay(30000);

mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Reaction Starting", 40, 120);
delay(2000);

//Primary 1
mylcd.Fill_Screen(GREEN);
mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Primary 1", 60, 90);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 10", 100, 200);



for (i=0; i<=h; i++){ //h=1500
  digitalWrite(in101, LOW);
  digitalWrite(in102, HIGH);
  delay (c);
  digitalWrite(in101, LOW);
  digitalWrite(in102, LOW);
  delay (d);
  }
delay(500);


//Inclubating primary
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Incubating Primaries", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 4 hours", 80, 180);

for (int j=0; j<=480; j++){ //for 4 hrs

delay(30000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}



//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


  //Probe wash
  mylcd.Fill_Screen(GREEN);
  mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Wash Buffer", 30, 90);
mylcd.Print_String("In plate 1", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 3", 100, 200);

for (i=0; i<=u; i++){ //u=1200
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (c);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (d);
  }
delay(500);


mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing Primaries", 30, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 80, 180);

delay(120000);

//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)



//Perform wash 8 times
for(j=0; j<8; j++){

   mylcd.Fill_Screen(GREEN);
   mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Wash Buffer", 10, 90);
mylcd.Print_String("In plate 1", 120, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 3", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (c);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (d);
  }
delay(500);


mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing Primaries", 30, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 60, 180);

delay(120000);

//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1000
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


}


  //5x SSCT wash
  //5x SSCT 1st time
  mylcd.Fill_Screen(GREEN);
  mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 60, 90);
mylcd.Print_String("In plate 1", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 1", 100, 200);

for (i=0; i<=u; i++){ //u=1200
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
delay(500);



mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing with 5x SSCT", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 60, 180);

delay(120000);

//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


//Perform 5x SSCT wash 2 times

for(j=0; j<2; j++){

   mylcd.Fill_Screen(GREEN);
   mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 10, 90);
mylcd.Print_String("In plate 1", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 1", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
delay(500);



mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing with 5x SSCT", 60, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 60, 180);

delay(120000);

//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}

  //Amp buffer
mylcd.Fill_Screen(GREEN);
mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Amp Buffer", 30, 90);

mylcd.Print_String("In plate 1", 30, 150);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 5", 100, 200);

for (i=0; i<=h; i++){ //h=1500
  digitalWrite(in51, LOW);
  digitalWrite(in52, HIGH);
  delay (c);
  digitalWrite(in51, LOW);
  digitalWrite(in52, LOW);
  delay (d);
  }
delay(500);



mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Inclubating in Amp Buff", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 10 mins", 80, 200);

for (int j=0; j<=20; j++){ //for 10 mins

delay(30000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}

//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


  //Secondary

  //Secondary 1
mylcd.Fill_Screen(GREEN);
mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Secondary 1", 30, 90);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 11", 100, 200);



for (i=0; i<=h; i++){ //h=1500
  digitalWrite(in111, LOW);
  digitalWrite(in112, HIGH);
  delay (c);
  digitalWrite(in111, LOW);
  digitalWrite(in112, LOW);
  delay (d);
  }
delay(500);



//Inclubating secondaries
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Incubating", 60, 90);
mylcd.Print_String("Secondaries", 60, 140);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 3 hours", 60, 200);

for (int j=0; j<=360; j++){ //for 3 hrs

delay(30000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}

//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Collecting", 60, 90);
mylcd.Print_String("Secondaries", 60, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 7", 100, 200);

for (i=0; i<=h; i++){ //h=1500
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }


  //5x SSCT wash: 8 times
for(j=0; j<8; j++){

   mylcd.Fill_Screen(GREEN);
   mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 60, 90);
mylcd.Print_String("In plate 1", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 1", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
delay(500);

mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing with 5xSSCT", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 60, 180);

delay(120000);

//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}

  //DAPI
mylcd.Fill_Screen(GREEN);
mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding DAPI Buffer", 10, 90);

mylcd.Print_String("In plate 1", 60, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 14", 100, 200);

for (i=0; i<=h; i++){ //h=1500
  digitalWrite(in141, LOW);
  digitalWrite(in142, HIGH);
  delay (c);
  digitalWrite(in141, LOW);
  digitalWrite(in142, LOW);
  delay (d);
  }
delay(500);


mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Inclubating in DAPI", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 10 mins", 80, 200);

for (int j=0; j<=10; j++){ //for 10 mins

delay(60000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}

//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


  //5x SSCT wash
for(j=0; j<4; j++){

   mylcd.Fill_Screen(GREEN);
   mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 60, 90);
mylcd.Print_String("In plate 1", 80, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 1", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
delay(500);

mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing with 5x SSCT", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 60, 180);

delay(120000);

//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


}

   mylcd.Fill_Screen(GREEN);
   mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Img Buffer", 40, 90);
mylcd.Print_String("In plate 1", 60, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 16", 100, 200);

for (i=0; i<=h; i++){ //h=1500
  digitalWrite(in161, LOW);
  digitalWrite(in162, HIGH);
  delay (c);
  digitalWrite(in161, LOW);
  digitalWrite(in162, LOW);
  delay (d);
  }
delay(500);


delay(1000);
digitalWrite(relay1, LOW);
//heating off


//cooling on
//digitalWrite(relay2, HIGH);

//Run finished
mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Congratulations", 60, 120);
mylcd.Print_String("Run successful", 80, 180);
delay(99999999999999999999999);


}












//Two samples 
if(currentState2 == pressed2)
  {


//Adjust the values to increase or decrease the volume dispensed.
//These values are calibrated for GROTHEN 12V 7W peristaltic pumps
int a=10;//test ON
int b=10;//test OFF


int c=7;//inlet ON
int d=20;//inlet OFF
int e=8;//outlet ON
int f=20;//outlet OFF

int g = 5; //For test

int h =1500; //for first dispensing specific buffer
int p = 320; //for subsiquent dispensing

int q = 1500; //for removing buffer

int i;
int j;
int k;
int l;
int m;
int n;
int o;

  mylcd.Fill_Screen(WHITE); 
mylcd.Set_Text_Back_colour(WHITE);
  mylcd.Set_Text_colour(BLACK);  
  mylcd.Set_Text_Size(4);

  
mylcd.Print_String("Program: 2_FISH", 40, 90);// put your main code here, to run repeatedly:
mylcd.Set_Text_Size(4);
mylcd.Print_String("Initiated", 100, 150);
mylcd.Print_String("2 Samples HCR", 80, 250);
delay(5000);

mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pumps", 30, 120);


//Testing pump 1
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 1", 60, 120);

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
delay(200);


//Testing pump 2
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 2", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (a);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (b);
  }
  for (i=0; i<=5; i++){
  digitalWrite(in21, HIGH);
  digitalWrite(in22, LOW);
  delay (a);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (b);
  }
delay(200);


//Testing pump 3
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 3", 60, 120);

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
delay(200);

//Testing pump 4
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 4", 60, 120);

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
delay(200);

//Testing pump 5
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 5", 60, 120);

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
delay(200);

//Testing pump 6
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 6", 60, 120);

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
delay(200);

//Testing pump 7
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 7", 60, 120);

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
delay(200);

//Testing pump 8
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 8", 60, 120);

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
delay(200);

//Testing pump 9
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 9", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (a);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in91, HIGH);
  digitalWrite(in92, LOW);
  delay (a);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (b);
  }
delay(200);

//Testing pump 10
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 10", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in101, LOW);
  digitalWrite(in102, HIGH);
  delay (a);
  digitalWrite(in101, LOW);
  digitalWrite(in102, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in101, HIGH);
  digitalWrite(in102, LOW);
  delay (a);
  digitalWrite(in101, LOW);
  digitalWrite(in102, LOW);
  delay (b);
  }
delay(200);

//Testing pump 11
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 11", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in111, LOW);
  digitalWrite(in112, HIGH);
  delay (a);
  digitalWrite(in111, LOW);
  digitalWrite(in112, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in111, HIGH);
  digitalWrite(in112, LOW);
  delay (a);
  digitalWrite(in111, LOW);
  digitalWrite(in112, LOW);
  delay (b);
  }
delay(200);

//Testing pump 12
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 12", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in121, LOW);
  digitalWrite(in122, HIGH);
  delay (a);
  digitalWrite(in121, LOW);
  digitalWrite(in122, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in121, HIGH);
  digitalWrite(in122, LOW);
  delay (a);
  digitalWrite(in121, LOW);
  digitalWrite(in122, LOW);
  delay (b);
  }
delay(200);

//Testing pump 13
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 13", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in131, LOW);
  digitalWrite(in132, HIGH);
  delay (a);
  digitalWrite(in131, LOW);
  digitalWrite(in132, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in131, HIGH);
  digitalWrite(in132, LOW);
  delay (a);
  digitalWrite(in131, LOW);
  digitalWrite(in132, LOW);
  delay (b);
  }
delay(200);

//Testing pump 14
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 14", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in141, LOW);
  digitalWrite(in142, HIGH);
  delay (a);
  digitalWrite(in141, LOW);
  digitalWrite(in142, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in141, HIGH);
  digitalWrite(in142, LOW);
  delay (a);
  digitalWrite(in141, LOW);
  digitalWrite(in142, LOW);
  delay (b);
  }
delay(200);

//Testing pump 15
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 15", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in151, LOW);
  digitalWrite(in152, HIGH);
  delay (a);
  digitalWrite(in151, LOW);
  digitalWrite(in152, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in151, HIGH);
  digitalWrite(in152, LOW);
  delay (a);
  digitalWrite(in151, LOW);
  digitalWrite(in152, LOW);
  delay (b);
  }
delay(200);

//Testing pump 16
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 16", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in161, LOW);
  digitalWrite(in162, HIGH);
  delay (a);
  digitalWrite(in161, LOW);
  digitalWrite(in162, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in161, HIGH);
  digitalWrite(in162, LOW);
  delay (a);
  digitalWrite(in161, LOW);
  digitalWrite(in162, LOW);
  delay (b);
  }
delay(200);

//Testing pump 17
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 17", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in171, LOW);
  digitalWrite(in172, HIGH);
  delay (a);
  digitalWrite(in171, LOW);
  digitalWrite(in172, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in171, HIGH);
  digitalWrite(in172, LOW);
  delay (a);
  digitalWrite(in171, LOW);
  digitalWrite(in172, LOW);
  delay (b);
  }
delay(200);

//Testing pump 18
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 18", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (a);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in181, HIGH);
  digitalWrite(in182, LOW);
  delay (a);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (b);
  }
delay(200);



mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Pump Testing Done", 30, 120);
delay(500);


mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Thermals", 60, 120);
delay(500);

/*
//Testing Cooling
mylcd.Fill_Screen(BLUE);
mylcd.Set_Text_Back_colour(BLUE);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Cooling", 60, 120);


digitalWrite(relay2, HIGH);

delay(10000);

digitalWrite(relay2, LOW);
*/

//Testing heating
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Heating", 60, 120);


delay(1000);
digitalWrite(relay1, HIGH);

delay(10000);

digitalWrite(relay1, LOW);


//Testing done
mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Done", 70, 120);
delay(1000);


//Preheating
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Preheating plate", 60, 120);

mylcd.Print_String("Please Wait", 110, 180);


delay(1000);
digitalWrite(relay1, HIGH);

delay(30000);

mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Reaction Starting", 40, 120);
delay(2000);

//Primary 1
mylcd.Fill_Screen(GREEN);
mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Primary 1", 60, 90);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 10", 100, 200);



for (i=0; i<=2000; i++){ //h=1500
  digitalWrite(in101, LOW);
  digitalWrite(in102, HIGH);
  delay (c);
  digitalWrite(in101, LOW);
  digitalWrite(in102, LOW);
  delay (d);
  }
delay(500);

//Primary 2
mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Primary 2", 60, 90);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 12", 100, 200);



for (i=0; i<=h; i++){ //h=1500
  digitalWrite(in121, LOW);
  digitalWrite(in122, HIGH);
  delay (c);
  digitalWrite(in121, LOW);
  digitalWrite(in122, LOW);
  delay (d);
  }



//Inclubating primary
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Incubating Primaries", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 4 hours", 80, 180);



for (int j=0; j<=480; j++){ //for 4 hrs

delay(30000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}



//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

//Probe wash

  for(int j=0; j<5; j++){

  mylcd.Fill_Screen(GREEN);
  mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Wash Buffer", 30, 90);
mylcd.Print_String("In plate 1", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 3", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (c);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (d);
  }
delay(500);


mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Wash Buffer", 30, 90);
mylcd.Print_String("In plate 2", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 4", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in41, LOW);
  digitalWrite(in42, HIGH);
  delay (c);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (d);
  }
delay(500);

  }


mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing Primaries", 30, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 80, 180);

delay(120000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)



//Perform wash 8 times
for(j=0; j<8; j++){

   mylcd.Fill_Screen(GREEN);
   mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Wash Buffer", 10, 90);
mylcd.Print_String("In plate 1", 120, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 3", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (c);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (d);
  }
delay(500);


mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Wash Buffer", 10, 90);
mylcd.Print_String("In plate 2", 120, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 4", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in41, LOW);
  digitalWrite(in42, HIGH);
  delay (c);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (d);
  }
delay(500);

mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing Primaries", 30, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 60, 180);

delay(120000);

//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}


  //5x SSCT wash
  //5x SSCT 1st time

for(int j=0; j<5; j++){

  mylcd.Fill_Screen(GREEN);
  mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 60, 90);
mylcd.Print_String("In plate 1", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 1", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
delay(500);


mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 60, 90);
mylcd.Print_String("In plate 2", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 2", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

  }


mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing with 5x SSCT", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 60, 180);

delay(120000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

//Perform 5x SSCT wash 2 times

for(j=0; j<2; j++){

   mylcd.Fill_Screen(GREEN);
   mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 10, 90);
mylcd.Print_String("In plate 1", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 1", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
delay(500);


mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 10, 90);
mylcd.Print_String("In plate 2", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 2", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing with 5x SSCT", 60, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 60, 180);

delay(120000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}

  //Amp buffer

  for (j=0; j<5; j++){

mylcd.Fill_Screen(GREEN);
mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Amp Buffer", 30, 90);

mylcd.Print_String("In plate 1", 30, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 5", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in51, LOW);
  digitalWrite(in52, HIGH);
  delay (c);
  digitalWrite(in51, LOW);
  digitalWrite(in52, LOW);
  delay (d);
  }
delay(500);

mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Amp Buffer", 30, 90);

mylcd.Print_String("In plate 2", 30, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 6", 100, 200);

for (i=0; i<=p; i++){ //p=350
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
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


  }


mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Inclubating in Amp Buff", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 20 mins", 80, 200);


for (int j=0; j<=40; j++){ //for 20 mins

delay(30000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}

//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

  //Secondary

  //Secondary 1
mylcd.Fill_Screen(GREEN);
mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Secondary 1", 30, 90);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 11", 100, 200);



for (i=0; i<=h; i++){ //h=1500
  digitalWrite(in111, LOW);
  digitalWrite(in112, HIGH);
  delay (c);
  digitalWrite(in111, LOW);
  digitalWrite(in112, LOW);
  delay (d);
  }
delay(500);

//Secondary 2
mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Secondary 2", 30, 90);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 13", 100, 200);



for (i=0; i<=h; i++){ //h=1500
  digitalWrite(in131, LOW);
  digitalWrite(in132, HIGH);
  delay (c);
  digitalWrite(in131, LOW);
  digitalWrite(in132, LOW);
  delay (d);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


//Inclubating secondaries
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Incubating", 60, 90);
mylcd.Print_String("Secondaries", 60, 140);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 3 hours", 60, 200);

for (int j=0; j<=360; j++){ //for 3 hrs

delay(30000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}

//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Collecting", 60, 90);
mylcd.Print_String("Secondaries", 60, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 7", 100, 200);

for (i=0; i<=1500; i++){
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }

  mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Collecting", 60, 90);
mylcd.Print_String("Secondaries", 60, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 8", 100, 200);

for (i=0; i<=1500; i++){
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (e);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)



  //5x SSCT wash: 8 times
for(j=0; j<8; j++){

   mylcd.Fill_Screen(GREEN);
   mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 60, 90);
mylcd.Print_String("In plate 1", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 1", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
delay(500);


mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 60, 90);
mylcd.Print_String("In plate 2", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 2", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing with 5xSSCT", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 60, 180);

delay(120000);



//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }

mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}

  //DAPI

  for (j=0; j<5; j++){

    mylcd.Fill_Screen(GREEN);
    mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding DAPI Buffer", 10, 90);

mylcd.Print_String("In plate 1", 60, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 14", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in141, LOW);
  digitalWrite(in142, HIGH);
  delay (c);
  digitalWrite(in141, LOW);
  digitalWrite(in142, LOW);
  delay (d);
  }
delay(500);

mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding DAPI Buffer", 10, 90);

mylcd.Print_String("In plate 2", 60, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 15", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in151, LOW);
  digitalWrite(in152, HIGH);
  delay (c);
  digitalWrite(in151, LOW);
  digitalWrite(in152, LOW);
  delay (d);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

  }

mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Inclubating in DAPI", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 5 mins", 80, 200);

for (int j=0; j<=10; j++){ //for 10 mins

delay(30000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}

//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

  //5x SSCT wash
for(j=0; j<4; j++){

   mylcd.Fill_Screen(GREEN);
   mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 60, 90);
mylcd.Print_String("In plate 1", 80, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 1", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
delay(500);


mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 60, 90);
mylcd.Print_String("In plate 2", 80, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 2", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing with 5x SSCT", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 60, 180);

delay(120000);


//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}

 

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


delay(1000);
digitalWrite(relay1, LOW);

//digitalWrite(relay2, HIGH);



for (j=0; j<5; j++){

    mylcd.Fill_Screen(GREEN);
    mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Img Buffer", 10, 90);

mylcd.Print_String("In plate 1", 60, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 16", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in161, LOW);
  digitalWrite(in162, HIGH);
  delay (c);
  digitalWrite(in161, LOW);
  digitalWrite(in162, LOW);
  delay (d);
  }
delay(500);

mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Img Buffer", 10, 90);

mylcd.Print_String("In plate 2", 60, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 17", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in171, LOW);
  digitalWrite(in172, HIGH);
  delay (c);
  digitalWrite(in171, LOW);
  digitalWrite(in172, LOW);
  delay (d);
  }
delay(500);

}

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)



//Run finished
mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Congratulations", 60, 120);
mylcd.Print_String("Run successful", 80, 180);
delay(99999999999999999999999);


}


//Calibration
if(currentState3 == pressed3)
  {

//Adjust the values to increase or decrease the volume dispensed.
//These values are calibrated for GROTHEN 12V 7W peristaltic pumps

int a=10;//test ON
int b=10;//test OFF
int c=7;//inlet ON
int d=20;//inlet OFF
int e=8;//outlet ON
int f=20;//outlet OFF

int g = 5; //For test

int h =1500; //for first dispensing
int p = 320; //for subsiquent dispensing

int q = 1500; //for removing buffer

int i;
int j;
int k;
int l;
int m;
int n;
int o;

  mylcd.Fill_Screen(WHITE); 
mylcd.Set_Text_Back_colour(WHITE);
  mylcd.Set_Text_colour(BLACK);  
  mylcd.Set_Text_Size(4);

  
mylcd.Print_String("Program: Calibrate", 40, 90);// put your main code here, to run repeatedly:
mylcd.Set_Text_Size(4);
mylcd.Print_String("Initiated", 100, 150);
mylcd.Print_String("Pump calibration", 60, 250);
delay(5000);

mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pumps", 30, 120);


//Testing pump 1
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 1", 60, 120);

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
delay(200);


//Testing pump 2
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 2", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (a);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (b);
  }
  for (i=0; i<=5; i++){
  digitalWrite(in21, HIGH);
  digitalWrite(in22, LOW);
  delay (a);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (b);
  }
delay(200);


//Testing pump 3
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 3", 60, 120);

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
delay(200);

//Testing pump 4
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 4", 60, 120);

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
delay(200);

//Testing pump 5
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 5", 60, 120);

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
delay(200);

//Testing pump 6
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 6", 60, 120);

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
delay(200);

//Testing pump 7
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 7", 60, 120);

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
delay(200);

//Testing pump 8
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 8", 60, 120);

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
delay(200);

//Testing pump 9
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 9", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (a);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in91, HIGH);
  digitalWrite(in92, LOW);
  delay (a);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (b);
  }
delay(200);

//Testing pump 10
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 10", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in101, LOW);
  digitalWrite(in102, HIGH);
  delay (a);
  digitalWrite(in101, LOW);
  digitalWrite(in102, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in101, HIGH);
  digitalWrite(in102, LOW);
  delay (a);
  digitalWrite(in101, LOW);
  digitalWrite(in102, LOW);
  delay (b);
  }
delay(200);

//Testing pump 11
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 11", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in111, LOW);
  digitalWrite(in112, HIGH);
  delay (a);
  digitalWrite(in111, LOW);
  digitalWrite(in112, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in111, HIGH);
  digitalWrite(in112, LOW);
  delay (a);
  digitalWrite(in111, LOW);
  digitalWrite(in112, LOW);
  delay (b);
  }
delay(200);

//Testing pump 12
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 12", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in121, LOW);
  digitalWrite(in122, HIGH);
  delay (a);
  digitalWrite(in121, LOW);
  digitalWrite(in122, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in121, HIGH);
  digitalWrite(in122, LOW);
  delay (a);
  digitalWrite(in121, LOW);
  digitalWrite(in122, LOW);
  delay (b);
  }
delay(200);

//Testing pump 13
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 13", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in131, LOW);
  digitalWrite(in132, HIGH);
  delay (a);
  digitalWrite(in131, LOW);
  digitalWrite(in132, LOW);
  delay (b);
  }
for (i=0; i<=g; i++){
  digitalWrite(in131, HIGH);
  digitalWrite(in132, LOW);
  delay (a);
  digitalWrite(in131, LOW);
  digitalWrite(in132, LOW);
  delay (b);
  }
delay(200);

//Testing pump 14
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 14", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in141, LOW);
  digitalWrite(in142, HIGH);
  delay (e);
  digitalWrite(in141, LOW);
  digitalWrite(in142, LOW);
  delay (f);
  }
for (i=0; i<=g; i++){
  digitalWrite(in141, HIGH);
  digitalWrite(in142, LOW);
  delay (e);
  digitalWrite(in141, LOW);
  digitalWrite(in142, LOW);
  delay (f);
  }
delay(200);

//Testing pump 15
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 15", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in151, LOW);
  digitalWrite(in152, HIGH);
  delay (e);
  digitalWrite(in151, LOW);
  digitalWrite(in152, LOW);
  delay (f);
  }
for (i=0; i<=g; i++){
  digitalWrite(in151, HIGH);
  digitalWrite(in152, LOW);
  delay (e);
  digitalWrite(in151, LOW);
  digitalWrite(in152, LOW);
  delay (f);
  }
delay(200);

//Testing pump 16
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 16", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in161, LOW);
  digitalWrite(in162, HIGH);
  delay (e);
  digitalWrite(in161, LOW);
  digitalWrite(in162, LOW);
  delay (f);
  }
for (i=0; i<=g; i++){
  digitalWrite(in161, HIGH);
  digitalWrite(in162, LOW);
  delay (e);
  digitalWrite(in161, LOW);
  digitalWrite(in162, LOW);
  delay (f);
  }
delay(200);

//Testing pump 17
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 17", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in171, LOW);
  digitalWrite(in172, HIGH);
  delay (e);
  digitalWrite(in171, LOW);
  digitalWrite(in172, LOW);
  delay (f);
  }
for (i=0; i<=g; i++){
  digitalWrite(in171, HIGH);
  digitalWrite(in172, LOW);
  delay (e);
  digitalWrite(in171, LOW);
  digitalWrite(in172, LOW);
  delay (f);
  }
delay(200);

//Testing pump 18
mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Pump 18", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }
for (i=0; i<=g; i++){
  digitalWrite(in181, HIGH);
  digitalWrite(in182, LOW);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }
delay(200);


mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Pump Testing Done", 30, 120);
delay(500);


mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Thermals", 60, 120);
delay(500);

/*
//Testing Cooling
mylcd.Fill_Screen(BLUE);
mylcd.Set_Text_Back_colour(BLUE);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Cooling", 60, 120);


digitalWrite(relay2, HIGH);

delay(10000);

digitalWrite(relay2, LOW);
*/

//Testing heating
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Heating", 60, 120);


delay(1000);
digitalWrite(relay1, HIGH);

delay(10000);


delay(1000);
digitalWrite(relay1, LOW);


//Testing done
mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Testing Done", 70, 120);
delay(1000);


//Preheating
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Preheating plate", 60, 120);

mylcd.Print_String("Please Wait", 110, 180);


delay(1000);
digitalWrite(relay1, HIGH);

delay(30000);

mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Reaction Starting", 40, 120);
delay(2000);

//Primary 1
mylcd.Fill_Screen(GREEN);
mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Primary 1", 60, 90);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 10", 100, 200);



for (i=0; i<=h; i++){ //h=1500
  digitalWrite(in101, LOW);
  digitalWrite(in102, HIGH);
  delay (c);
  digitalWrite(in101, LOW);
  digitalWrite(in102, LOW);
  delay (d);
  }
delay(500);

//Primary 2
mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Primary 2", 60, 90);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 12", 100, 200);



for (i=0; i<=h; i++){ //h=1500
  digitalWrite(in121, LOW);
  digitalWrite(in122, HIGH);
  delay (c);
  digitalWrite(in121, LOW);
  digitalWrite(in122, LOW);
  delay (d);
  }



//Inclubating primary
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Incubating Primaries", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 3 secs", 80, 180);

delay(3000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)



//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

  //Probe wash

  for(int j=0; j<2; j++){

  mylcd.Fill_Screen(GREEN);
  mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Wash Buffer", 30, 90);
mylcd.Print_String("In plate 1", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 3", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (c);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (d);
  }
delay(500);


mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Wash Buffer", 30, 90);
mylcd.Print_String("In plate 2", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 4", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in41, LOW);
  digitalWrite(in42, HIGH);
  delay (c);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (d);
  }
delay(500);

  }


mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing Primaries", 30, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 80, 180);

delay(120000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

//Perform wash 8 times
for(j=0; j<8; j++){

   mylcd.Fill_Screen(GREEN);
   mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Wash Buffer", 10, 90);
mylcd.Print_String("In plate 1", 120, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 3", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (c);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (d);
  }
delay(500);


mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Wash Buffer", 10, 90);
mylcd.Print_String("In plate 2", 120, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 4", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in41, LOW);
  digitalWrite(in42, HIGH);
  delay (c);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (d);
  }
delay(500);

mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing Primaries", 30, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 60, 180);

delay(120000);

//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}


  //5x SSCT wash
  //5x SSCT 1st time

for(int j=0; j<2; j++){

  mylcd.Fill_Screen(GREEN);
  mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 60, 90);
mylcd.Print_String("In plate 1", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 1", 100, 200);

for (i=0; i<=p; i++){
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
delay(500);


mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 60, 90);
mylcd.Print_String("In plate 2", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 2", 100, 200);

for (i=0; i<=p; i++){
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

  }


mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing with 5x SSCT", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 60, 180);

delay(120000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

//Perform 5x SSCT wash 2 times

for(j=0; j<2; j++){

   mylcd.Fill_Screen(GREEN);
   mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 10, 90);
mylcd.Print_String("In plate 1", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 1", 100, 200);

for (i=0; i<=p; i++){
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
delay(500);


mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 10, 90);
mylcd.Print_String("In plate 2", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 2", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing with 5x SSCT", 60, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 60, 180);

delay(120000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}

  //Amp buffer

  for (j=0; j<5; j++){

mylcd.Fill_Screen(GREEN);
mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Amp Buffer", 30, 90);

mylcd.Print_String("In plate 1", 30, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 5", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in51, LOW);
  digitalWrite(in52, HIGH);
  delay (c);
  digitalWrite(in51, LOW);
  digitalWrite(in52, LOW);
  delay (d);
  }
delay(500);

mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Amp Buffer", 30, 90);

mylcd.Print_String("In plate 2", 30, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 6", 100, 200);

for (i=0; i<=p; i++){ //p=350
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
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


  }


mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Inclubating in Amp Buff", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 20 mins", 80, 200);

delay(12000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

  //Secondary

  //Secondary 1
mylcd.Fill_Screen(GREEN);
mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Secondary 1", 30, 90);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 11", 100, 200);



for (i=0; i<=h; i++){ //h=1500
  digitalWrite(in111, LOW);
  digitalWrite(in112, HIGH);
  delay (c);
  digitalWrite(in111, LOW);
  digitalWrite(in112, LOW);
  delay (d);
  }
delay(500);

//Secondary 2
mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Secondary 2", 30, 90);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 13", 100, 200);



for (i=0; i<=h; i++){ //h=1500
  digitalWrite(in131, LOW);
  digitalWrite(in132, HIGH);
  delay (c);
  digitalWrite(in131, LOW);
  digitalWrite(in132, LOW);
  delay (d);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


//Inclubating secondaries
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Incubating", 60, 90);
mylcd.Print_String("Secondaries", 60, 140);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 3 secs", 60, 200);

delay(3000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Collecting", 60, 90);
mylcd.Print_String("Secondaries", 60, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 7", 100, 200);

for (i=0; i<=1500; i++){
  digitalWrite(in71, LOW);
  digitalWrite(in72, HIGH);
  delay (e);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (f);
  }

  mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Collecting", 60, 90);
mylcd.Print_String("Secondaries", 60, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 8", 100, 200);

for (i=0; i<=1500; i++){
  digitalWrite(in81, LOW);
  digitalWrite(in82, HIGH);
  delay (e);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)



  //5x SSCT wash: 8 times
for(j=0; j<8; j++){

   mylcd.Fill_Screen(GREEN);
   mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 60, 90);
mylcd.Print_String("In plate 1", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 1", 100, 200);

for (i=0; i<=p; i++){
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
delay(500);


mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 60, 90);
mylcd.Print_String("In plate 2", 100, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 2", 100, 200);

for (i=0; i<=p; i++){
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing with 5xSSCT", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 60, 180);

delay(120000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){ //q=1200
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}

  //DAPI

  for (j=0; j<5; j++){

    mylcd.Fill_Screen(GREEN);
    mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding DAPI Buffer", 10, 90);

mylcd.Print_String("In plate 1", 60, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 14", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in141, LOW);
  digitalWrite(in142, HIGH);
  delay (c);
  digitalWrite(in141, LOW);
  digitalWrite(in142, LOW);
  delay (d);
  }
delay(500);

mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding DAPI Buffer", 10, 90);

mylcd.Print_String("In plate 2", 60, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 15", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in151, LOW);
  digitalWrite(in152, HIGH);
  delay (c);
  digitalWrite(in151, LOW);
  digitalWrite(in152, LOW);
  delay (d);
  }
delay(500);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

  }

mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Inclubating in DAPI", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 3 secs", 80, 200);

delay(3000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

  //5x SSCT wash
for(j=0; j<4; j++){

   mylcd.Fill_Screen(GREEN);
   mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 60, 90);
mylcd.Print_String("In plate 1", 80, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 1", 100, 200);

for (i=0; i<=p; i++){
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (c);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (d);
  }
delay(500);


mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding 5x SSCT", 60, 90);
mylcd.Print_String("In plate 2", 80, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 2", 100, 200);

for (i=0; i<=p; i++){
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (c);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (d);
  }
delay(500);

mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Washing with 5x SSCT", 10, 90);
mylcd.Set_Text_Size(4);
mylcd.Print_String("For 120 seconds", 60, 180);

delay(120000);

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


//Remove buffers
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 1", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 9", 100, 200);

for (i=0; i<=q; i++){
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (e);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (f);
  }


mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Removing Buffer", 60, 90);
mylcd.Print_String("From plate 2", 80, 140);
mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 18", 100, 200);

for (i=0; i<=q; i++){
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (e);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (f);
  }

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)

}

for (j=0; j<5; j++){

    mylcd.Fill_Screen(GREEN);
    mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding Img Buffer", 10, 90);

mylcd.Print_String("In plate 1", 60, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 16", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in161, LOW);
  digitalWrite(in162, HIGH);
  delay (c);
  digitalWrite(in161, LOW);
  digitalWrite(in162, LOW);
  delay (d);
  }
delay(500);

mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Adding DAPI Buffer", 10, 90);

mylcd.Print_String("In plate 2", 60, 140);

mylcd.Set_Text_Size(3);
mylcd.Print_String("Using: Pump 17", 100, 200);

for (i=0; i<=p; i++){ //p=350
  digitalWrite(in171, LOW);
  digitalWrite(in172, HIGH);
  delay (c);
  digitalWrite(in171, LOW);
  digitalWrite(in172, LOW);
  delay (d);
  }
delay(500);

}

// Pull RESET pin LOW briefly
pinMode(RESET_PIN, OUTPUT);
digitalWrite(RESET_PIN, LOW);
delay(50); // Wait for 50ms
pinMode(RESET_PIN, INPUT); // Release the pin (let pull-up resistor bring it HIGH)


delay(1000);
digitalWrite(relay1, LOW);

//digitalWrite(relay2, HIGH);

//Run finished
mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Congratulations", 60, 120);
mylcd.Print_String("Run successful", 80, 180);
delay(99999999999999999999999);


}

















if(currentState4 == pressed4)//For cleaning
  {

//Adjust the values to increase or decrease the volume dispensed.
//These values are calibrated for GROTHEN 12V 7W peristaltic pumps

int a=10;//cleaning ON
int b=10;//cleaning OFF

int h = 1500; 

int k = 3;
int g = 300; 

int i;
int j;

int l;
int m;
int n;
int o;

  mylcd.Fill_Screen(WHITE); 
mylcd.Set_Text_Back_colour(WHITE);
  mylcd.Set_Text_colour(BLACK);  
  mylcd.Set_Text_Size(4);

  
mylcd.Print_String("Deep Cleanining", 60, 90);// put your main code here, to run repeatedly:

mylcd.Print_String("Initiated", 120, 150);
delay(5000);

for(int l=0; l<3; l++){


//flushing pump 1 and 2
for (j=0; j<k; j++){

mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing Pump 1", 30, 120);

for (i=0; i<=g; i++){
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
  delay (a);
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  delay (b);
  }

mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing Pump 2", 30, 120);

for (i=0; i<=g; i++){
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
  delay (a);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  delay (b);
  }

}

//flushing pump 9
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 9", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (a);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (b);
  }


//flushing pump 18
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 18", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (a);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (b);
  }


//flushing pump 3 and 4
for (j=0; j<k; j++){

mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing Pump 3", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in31, LOW);
  digitalWrite(in32, HIGH);
  delay (a);
  digitalWrite(in31, LOW);
  digitalWrite(in32, LOW);
  delay (b);
  }

mylcd.Fill_Screen(MAGENTA);
mylcd.Set_Text_Back_colour(MAGENTA);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing Pump 4", 60, 120);

for (i=0; i<=g; i++){
  digitalWrite(in41, LOW);
  digitalWrite(in42, HIGH);
  delay (a);
  digitalWrite(in41, LOW);
  digitalWrite(in42, LOW);
  delay (b);
  }


}

//flushing pump 9
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 9", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (a);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (b);
  }

//flushing pump 18
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 18", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (a);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (b);
  }

//flushing pump 5 and 6
  for (j=0; j<k; j++){

    mylcd.Fill_Screen(GREEN);
    mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 5", 10, 90);

for (i=0; i<=g; i++){
  digitalWrite(in51, LOW);
  digitalWrite(in52, HIGH);
  delay (a);
  digitalWrite(in51, LOW);
  digitalWrite(in52, LOW);
  delay (b);
  }
delay(500);

mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 6", 10, 90);

for (i=0; i<=g; i++){
  digitalWrite(in61, LOW);
  digitalWrite(in62, HIGH);
  delay (a);
  digitalWrite(in61, LOW);
  digitalWrite(in62, LOW);
  delay (b);
  }
delay(500);

  }


//flushing pump 9
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 9", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (a);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (b);
  }

//flushing pump 18
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 18", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (a);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (b);
  }


//flushing pump 7 and 8
for (j=0; j<k; j++){

  mylcd.Fill_Screen(YELLOW);
  mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 7", 30, 120);


for (i=0; i<=g; i++){
  digitalWrite(in71, HIGH);
  digitalWrite(in72, LOW);
  delay (a);
  digitalWrite(in71, LOW);
  digitalWrite(in72, LOW);
  delay (b);
}

    mylcd.Fill_Screen(GREEN);
    mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 8", 10, 90);

for (i=0; i<=g; i++){
  digitalWrite(in81, HIGH);
  digitalWrite(in82, LOW);
  delay (a);
  digitalWrite(in81, LOW);
  digitalWrite(in82, LOW);
  delay (b);
  }
}

//flushing pump 9
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 9", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (a);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (b);
  }

//flushing pump 18
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 18", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (a);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (b);
  }
  
//flushing pump 10
 mylcd.Fill_Screen(YELLOW);
 mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 10", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in101, LOW);
  digitalWrite(in102, HIGH);
  delay (a);
  digitalWrite(in101, LOW);
  digitalWrite(in102, LOW);
  delay (b);
}

//flushing pump 12
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 12", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in121, LOW);
  digitalWrite(in122, HIGH);
  delay (a);
  digitalWrite(in121, LOW);
  digitalWrite(in122, LOW);
  delay (b);
}

//flushing pump 9
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 9", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (a);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (b);
  }

//flushing pump 18
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 18", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (a);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (b);
  }

//flushing pump 11
   mylcd.Fill_Screen(YELLOW);
   mylcd.Set_Text_Back_colour(YELLOW);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 11", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in111, LOW);
  digitalWrite(in112, HIGH);
  delay (a);
  digitalWrite(in111, LOW);
  digitalWrite(in112, LOW);
  delay (b);
}

//flushing pump 13
mylcd.Fill_Screen(YELLOW);
mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 13", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in131, LOW);
  digitalWrite(in132, HIGH);
  delay (a);
  digitalWrite(in131, LOW);
  digitalWrite(in132, LOW);
  delay (b);
}


//flushing pump 9
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 9", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (a);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (b);
  }

//flushing pump 18
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 18", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (a);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (b);
  }

//flushing pump 14 and 15
  for (j=0; j<k; j++){

    mylcd.Fill_Screen(GREEN);
    mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 14", 10, 90);

for (i=0; i<=g; i++){
  digitalWrite(in141, LOW);
  digitalWrite(in142, HIGH);
  delay (a);
  digitalWrite(in141, LOW);
  digitalWrite(in142, LOW);
  delay (b);
  }
delay(500);

mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 15", 10, 90);

for (i=0; i<=g; i++){
  digitalWrite(in151, LOW);
  digitalWrite(in152, HIGH);
  delay (a);
  digitalWrite(in151, LOW);
  digitalWrite(in152, LOW);
  delay (b);
  }
delay(500);

  }


//flushing pump 9

  mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 9", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (a);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (b);
  }

//flushing pump 18
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 18", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (a);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (b);
  }

//flushing pump 16 and 17
    for (j=0; j<k; j++){

    mylcd.Fill_Screen(GREEN);
    mylcd.Set_Text_Back_colour(GREEN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 16", 10, 90);

for (i=0; i<=g; i++){
  digitalWrite(in161, LOW);
  digitalWrite(in162, HIGH);
  delay (a);
  digitalWrite(in161, LOW);
  digitalWrite(in162, LOW);
  delay (b);
  }
delay(500);

mylcd.Fill_Screen(CYAN);
mylcd.Set_Text_Back_colour(CYAN);
mylcd.Set_Text_colour(BLACK);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 17", 10, 90);

for (i=0; i<=g; i++){
  digitalWrite(in171, LOW);
  digitalWrite(in172, HIGH);
  delay (a);
  digitalWrite(in171, LOW);
  digitalWrite(in172, LOW);
  delay (b);
  }
delay(500);

  }

//flushing pump 9
  mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 9", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in91, LOW);
  digitalWrite(in92, HIGH);
  delay (a);
  digitalWrite(in91, LOW);
  digitalWrite(in92, LOW);
  delay (b);
  }

//flushing pump 18
mylcd.Fill_Screen(RED);
mylcd.Set_Text_Back_colour(RED);
mylcd.Set_Text_colour(WHITE);
mylcd.Set_Text_Size(4);
mylcd.Print_String("Flushing pump 18", 30, 120);


for (i=0; i<=h; i++){
  digitalWrite(in181, LOW);
  digitalWrite(in182, HIGH);
  delay (a);
  digitalWrite(in181, LOW);
  digitalWrite(in182, LOW);
  delay (b);
  }

}

mylcd.Fill_Screen(WHITE);
mylcd.Set_Text_Back_colour(WHITE);
mylcd.Set_Text_colour(BLACK);
mylcd.Print_String("Deep Cleanining", 50, 90);
mylcd.Print_String("Successful", 100, 150);

delay(999999999999999999999999999999999);


  } //button4

} //end
/*********************************************************************
LBento
The following is an ESP8266 Digital Speedo based on the Wemos D1 Mini and its matching 64x48 OLED sheild.
The tested GPS module was an U-blox NEO 6M part number GY-GPS6MV2.

Required libraries include:
Adafruit Graphics
https://github.com/adafruit/Adafruit-GFX-Library
A modified version of Adafruit's SSD1306 library (This is important as the standard library won't display on our tiny diplay correctly)
https://github.com/mcauser/Adafruit_SSD1306
Tiny GPS++
http://arduiniana.org/libraries/tinygpsplus
Font 
https://github.com/robjen/GFX_fonts 
Notes on using different fonts https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts

Other Notes:
For Head Up Display (HUD) mode you could change line 247 of adafruit_ssd1306.ccp in your library folder as below
change ssd1306_command(SSD1306_COMSCANDEC); to ssd1306_command(SSD1306_COMSCANINC);

If you are alreading using the official Adafruit library for something else, this site below has a good tip on using the modified library 
as part of a wemos oled project.
https://diyprojects.io/shield-oled-wemos-d1-mini-ssd1306-64x48-pixels-review-esp-easy-adafruit_ssd1306

A shout out to these two projects for insperation and bits of code.
http://theelectromania.blogspot.it/2016/03/esp8266-esp-12e-nodemcu-and-ds18b20.html
https://github.com/mkconer/ESP8266_GPS/blob/master/ESP8266_GPS_OLED_Youtube.ino
*********************************************************************/

// NOTE: Remove adafruit splash screen https://forum.arduino.cc/index.php?topic=548785.0

//wemos D1 mini pin out 
 // D1 and D2, 3.3V and GND > OLED 
 // D6 and D7 , 5v and GND > GPS (wemos D6 to tx on gps module)(wemos D7 to Rx on gps module)
 // D3  and GND > Button

#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>                         //include from general library folder on the documnet/arduino
#include "Adafruit_SSD1306.h"                           //include from this project folder
#include <Fonts/FreeSans9pt7b.h>                        //choose font here & 2nd line of loop, try and keep same size for this display 
#include <Fonts/Font5x7Fixed.h>                       //smaller font from https://github.com/robjen/GFX_fonts install in Documents\Arduino\libraries\Adafruit_GFX_Library\Fonts
#include <TinyGPS++.h>                                  // Tiny GPS Plus Library
#include <SoftwareSerial.h>                             // Software Serial Library so we can use other Pins for communication with the GPS module

static const int RXPin = 12, TXPin = 13;                // Ublox 6m GPS module to pins 12 (wemos D6 to tx on gps module) and 13 (wemos D7 to Rx on gps module) 
static const uint32_t GPSBaud = 9600;                   // Ublox GPS default Baud Rate is 9600
                                                        //gps module 5v or 3.3v and GND
int gps_speed;
int num_sat;
int max_speed = 0;
int time_state = 1;

int mil;
float mil2;
int tsec; 
uint32_t startMillis;  //some global variables available anywhere in the program
uint32_t currentMillis;

int targetSpeed = 30; //set target speed for 0 to 
int incrementState = LOW;

//*******short and lond button press implementation********
//Short= reset status
//long = change target speed for 0 to target
const int button = 0; // D3 on wemos 
const int SHORT_PRESS_TIME = 2000; // 2 seconds
const int LONG_PRESS_TIME = 3000; //3 seconds

int lastState = LOW;  // the previous state from the input pin
int stateButton;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;


TinyGPSPlus gps;                                        // Create an Instance of the TinyGPS++ object called gps
SoftwareSerial ss(RXPin, TXPin);                        // The serial connection to the GPS device

#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);                   // constructor to call OLED display using adafruit library

const unsigned char logo_bmp [] PROGMEM = {
	// 'speedometer (2), 64x48px
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf1, 0xf3, 0x80, 0x00, 0x00, 
	0x00, 0x00, 0x0f, 0xf1, 0xf7, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf1, 0xe7, 0xb8, 0x00, 0x00, 
	0x00, 0x00, 0xff, 0xf1, 0xcf, 0x3e, 0x00, 0x00, 0x00, 0x03, 0xff, 0xf1, 0xcf, 0x3e, 0x00, 0x00, 
	0x00, 0x07, 0xff, 0xf1, 0x9f, 0x3e, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf1, 0x9f, 0x7c, 0x70, 0x00, 
	0x00, 0x03, 0xf8, 0x00, 0x3e, 0x78, 0x78, 0x00, 0x00, 0x63, 0xc0, 0x00, 0x3e, 0x70, 0xfc, 0x00, 
	0x00, 0xf1, 0x00, 0x00, 0x7e, 0x71, 0xfe, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x7e, 0x63, 0xff, 0x00, 
	0x03, 0xf8, 0x00, 0x00, 0xfc, 0x07, 0xff, 0x80, 0x07, 0xf0, 0x00, 0x00, 0xfc, 0x0f, 0xff, 0x80, 
	0x07, 0xe0, 0x00, 0x01, 0xfc, 0x07, 0xff, 0xc0, 0x0f, 0xc0, 0x00, 0x01, 0xfc, 0x03, 0xff, 0xe0, 
	0x0f, 0x80, 0x00, 0x03, 0xf8, 0x03, 0xff, 0xf0, 0x1f, 0x00, 0x00, 0x03, 0xf8, 0x01, 0xff, 0xf0, 
	0x1e, 0x00, 0x00, 0x07, 0xf8, 0x00, 0xff, 0xf8, 0x3c, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x7f, 0xf8, 
	0x3c, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x7f, 0xe0, 0x18, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x3f, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xf0, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x1e, 
	0x60, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x01, 0xfe, 0xe0, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x1f, 0xfe, 
	0xe0, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x1f, 0xff, 0xe0, 0x00, 0x00, 0xff, 0xe0, 0x00, 0x1f, 0xff, 
	0xe0, 0x00, 0x00, 0xf3, 0xc0, 0x00, 0x1f, 0xff, 0xc0, 0x00, 0x00, 0xe1, 0xc0, 0x00, 0x1f, 0xff, 
	0xc0, 0x00, 0x01, 0xc1, 0xc0, 0x00, 0x1f, 0xff, 0xc0, 0x00, 0x01, 0xc1, 0xc0, 0x00, 0x1f, 0xff, 
	0xc0, 0x00, 0x00, 0xe3, 0x80, 0x00, 0x1f, 0xff, 0xc0, 0x00, 0x00, 0xff, 0x80, 0x00, 0x1f, 0xff, 
	0xc0, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x1f, 0xff, 0x43, 0xc1, 0xfc, 0x1c, 0x00, 0x01, 0x1f, 0xff, 
	0x03, 0xc3, 0xfe, 0x00, 0x00, 0x07, 0x0f, 0x00, 0x03, 0xc3, 0xff, 0x3e, 0x7f, 0x8f, 0x9f, 0x80, 
	0x03, 0xc3, 0xfe, 0x7f, 0x7f, 0xdf, 0xbf, 0x80, 0x03, 0xc3, 0xfe, 0x7f, 0x7b, 0xcf, 0x3f, 0xc0, 
	0x03, 0xc3, 0xef, 0x7b, 0x7b, 0xcf, 0x3f, 0xc0, 0x03, 0xfb, 0xff, 0x7f, 0x7b, 0xcf, 0xbf, 0xc0, 
	0x03, 0xfb, 0xfe, 0x3f, 0x7b, 0xc7, 0x9f, 0x80, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x06, 0x00
};
//  Use someting like https://diyusthad.com/image2cpp to converte img to arduino bitmap and https://icons-for-free.com/ for icons

const unsigned char sat_icon [] PROGMEM = {
	// 'satellite+icon-1320186618395236640_16, 14x14px
	0x02, 0x00, 0x1c, 0x00, 0x32, 0x00, 0x68, 0x00, 0x54, 0x20, 0x88, 0x70, 0xa3, 0xf0, 0x03, 0xf0, 
	0x03, 0xf0, 0x07, 0xf0, 0x0f, 0xf0, 0x07, 0xf8, 0x01, 0xfc, 0x01, 0xfc
};


void setup()   {                

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);   
// display.setRotation(2);                            // uncomment for 180 display flip, may aid in mounting display etc
                                                      // init done
  Serial.begin(9600);
  pinMode(button, INPUT_PULLUP);                             // pin D4 as input

  // commenting out the above three lines would hide the adafruit splash screen (but you realy shouldn't)
  // same goes for below 4 lines to hide our custom ka logo, feel free to mod or get rid of this. 
  
  display.drawBitmap(0, 0, logo_bmp, 64, 48, 1);      // load our own splashscreen (x , y, name , size x, size y, color 1 white)
  display.display();
  delay(1500);                                        // wait 1.5 secs before clear splash
  display.clearDisplay();

  display.setTextWrap(false);   //prevent text from going to the next line  
  
  ss.begin(GPSBaud);  
  }




void loop() {

num_sat = gps.satellites.value();
gps_speed = gps.speed.kmph();  

display.setTextColor(WHITE);  
//display.setFont(&FreeSans9pt7b); //font type
display.setFont(&Font5x7Fixed); //smaller font
display.setTextSize(1);
 
  ////Display instant speed
  display.setCursor(15,45);
  display.println(gps_speed , DEC);
  display.setCursor(35,45);
  display.println ("km/h");
 
  //Display number of sat
  display.drawBitmap(50, 3, sat_icon, 14, 14, 1); //(x,y, name , size x, size y, color 1 white)
  display.setCursor(40,15);
  display.println(num_sat , DEC);   

  //Max speed calc
  if (gps_speed > max_speed){
    max_speed = gps_speed;
  }
  //Display max speed
  display.setCursor(3, 31);
  display.println("MaxS:");
  display.setCursor(35,31);
  display.println(max_speed, DEC); 
  
  //0-30km/h calc
  if (gps_speed > 1 && time_state == 1){
    startMillis = millis();
    time_state = 0;
  } 
  if (gps_speed >= targetSpeed && time_state == 0){ //change where if you want to call to a different speed
    currentMillis = millis();
    //calculate seconds with decimal case for more acurate mesure
    tsec = (currentMillis - startMillis)/1000;
    mil = (currentMillis - startMillis)%1000;
    mil2 = ((currentMillis - startMillis) / 1000.0, 1);
    if (mil < 100){
    mil = 0;
    }
    if (mil < 10){
    mil = 0;
    }
        
    time_state = 2;
  }
    //inf to let know user when timmer start or end and no dot=ready and wating to count
  if(time_state == 0){
    display.setCursor(3, 9);
    display.println(".");
  }
  else if(time_state == 2){
    display.setCursor(31, 9);
    display.println(".");
  } else{
    display.setCursor(3, 9);
    display.println(" ");
    display.setCursor(31, 9);
    display.println(" ");
  }

  //reset timer in case of bug
  if(tsec > 60){
    tsec=0;
  }

  //0-30km/h display
  display.setCursor(7, 9);
  display.println("0-"+ String(targetSpeed)); //change here to a display desire speed
  //display.println(targetSpeed);
  display.setCursor(9, 18);
  //display.println(tsec);
  //display.println(String(tsec)+"."+String(mil));//display seconds with decimal cases
  display.println(String(mil2));
  Serial.println(mil2);
  display.setCursor(24, 18);
  display.println("s");


//display grid
  display.drawFastVLine(35,0,20,1); //(x,y,length,color 1 white)
  display.drawFastHLine(0,20,64,1);
  display.drawFastHLine(0,35,64,1);
  display.drawRect(0,0,64,48,1);

// end code
display.display();
  delay(200); 
  
  smartDelay(500);                                   // Run Procedure smartDelay

  if (millis() > 5000 && gps.charsProcessed() < 10){
    display.println(F("No GPS data received: check wiring"));
  }

display.clearDisplay();

//reset max speed and timers on  button press short and long press change target speed
  stateButton = digitalRead(button);

  if(lastState == HIGH && stateButton == LOW){        // button is pressed
    pressedTime = millis();
    //Serial.println("press");
  }
  else if(lastState == LOW && stateButton == HIGH) { // button is released
    releasedTime = millis();
    //Serial.println("release");
    long pressDuration = releasedTime - pressedTime;

    if( pressDuration < SHORT_PRESS_TIME ){
      //Serial.println("A short press is detected");
      max_speed = 0;
      time_state = 1;
      tsec = 0;
      }
    
    if( pressDuration > LONG_PRESS_TIME ){
      //Serial.println("A Long press is detected");
      if ((10 >= targetSpeed <= 90) && incrementState == LOW) //increment speed by 10 with long button press untill 90
        targetSpeed = targetSpeed + 10;
      }if( targetSpeed > 90){
        targetSpeed = 10;
        }
      
  }

  // save the the last state
  lastState = stateButton;
}

static void smartDelay(unsigned long ms)           // This custom version of delay() ensures that the gps object is being "fed".
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
 }

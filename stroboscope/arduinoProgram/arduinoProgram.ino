/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x32 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ModifiedRotaryEncoder.h"
#include <EEPROM.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define ROTARY_SWITCH_PIN   4
#define LED_PIN 13
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 


#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

long pos;
long eprom_timer;
int eprom_dirty;
ModifiedRotaryEncoder encoder(2, 3);


void tickInterrupt() {
  encoder.tick(); // just call tick() to check the state.

}

void update_frequency(long f) {
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  char buffer[10];
  sprintf(buffer, "%05lu", f);
  display.println(buffer);
  int step = encoder.getStep();
  int log = 5;
  while (step > 0) {
    step /= 10;
    --log;
  }
  
 
  display.drawLine(12 * log, 20, 12 * log + 10, 20, WHITE);
   
  display.display();

  setPwm(f) ;
  

   
}

void setup()   {                
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), tickInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), tickInterrupt, CHANGE);


  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)

  long eprom_value = EEPROM.read(0) + (((long) EEPROM.read(1)) << 8);
  

  encoder.setNonLinearPosition(eprom_value);
  


  pinMode(ROTARY_SWITCH_PIN, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);
  eprom_timer = millis();
  eprom_dirty = 0;
 
   

}

int last_rotary_switch_ts = 0;
int last_rotary_status = LOW;
int st = LOW;
int st1 = LOW;


void setPwm(long f) {
 TCCR1A =   _BV(COM1B1) | _BV(WGM10);  // PWM, Phase and Frequency Correct, Non Inverting
  
 
   
  DDRB |= _BV(PB1) | _BV(PB2); 
 
 long prescales[] = {1L, 8L, 64L, 256L, 1024L};

  int prescale = 0;
  long top;
  while (1) {
    top = 16000000 / (2L * prescales[prescale] * (long) f);
    Serial.println(top);
    if (top < 65536)
      break;
    prescale++;
    if (prescale >= 5) {
      break;
    }
  }; 
  Serial.println("prescaler:");
  Serial.println(prescales[prescale]);
  Serial.println("number:");
  Serial.println(prescale);
  Serial.println(_BV(CS12));
  TCCR1B =   (prescale + 1)| _BV(WGM13);  // no prescaling
  OCR1A = top;
  OCR1B = 40 / prescales[prescale]; // 5 us width
  
  
  
}

void loop() {
 // encoder.tick();

  int rs =  digitalRead(ROTARY_SWITCH_PIN);
  if ((rs != last_rotary_status) && (millis() - last_rotary_switch_ts > 200)) {
    last_rotary_switch_ts = millis();
    last_rotary_status = rs;
    
    st = !st;
    if (st) { // rotary switch pressed
     
      encoder.tickSwitch();
      update_frequency(pos);
      
  

    }
    
    
  }
  
  long newPos = encoder.getNonLinearPosition();
  
 
 if (eprom_dirty && (millis() > eprom_timer + 1000)) {
   eprom_timer = millis();
   st1 = !st1;
   digitalWrite(LED_PIN, st1);
    
   
    eprom_dirty = 0;

    EEPROM.write(0, pos & 0xFF);
    EEPROM.write(1, pos >> 8);
    
 }
 
  if (pos != newPos) {
   
    update_frequency(newPos);
    pos = newPos;
    eprom_dirty = 1;
    eprom_timer = millis();
   
  } 

 
    
  
}


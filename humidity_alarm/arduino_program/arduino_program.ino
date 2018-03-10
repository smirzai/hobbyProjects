
#include <TM1637Display.h>
#include <DHT12.h>
#include <Wire.h>     //The DHT12 uses I2C comunication.
DHT12 dht12;          //Preset scale CELSIUS and ID 0x5c.

const int CLK = 10; //Set the CLK pin connection to the display
const int DIO = 11; //Set the DIO pin connection to the display

TM1637Display display(CLK, DIO);

void setup() {
  Wire.begin();
  Serial.begin(19200);
  display.setBrightness(0x0a);
}

void loop() {

  float tm = dht12.readTemperature();
  float hm = dht12.readHumidity();
  Serial.print("Temperatura: ");
  Serial.print(tm);
  

  Serial.print("*C  Humedad: ");
  Serial.println(hm);
  display.showNumberDec((int) hm + 100 * (int) tm);

  delay(5000);
}



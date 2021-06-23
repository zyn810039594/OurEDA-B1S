#include <Wire.h>
#include "MS5837.h"

typedef unsigned char u8;
typedef unsigned short u16;

MS5837 sensor;

u8* WString=0;
u16* WTemp=0;
u16* WDepth=0;

u8 LightFlag=0;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  WString=(u8*)malloc(4);
  WTemp=(u16*)WString;
  WDepth=(u16*)(WString+2);
  Wire.setSDA(PB7);
  Wire.setSCL(PB6);
  Serial.begin(921600);
  
  Wire.begin();

  // Initialize pressure sensor
  // Returns true if initialization was successful
  // We can't continue with the rest of the program unless we can initialize the sensor
  while (!sensor.init()) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
  }
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);
  sensor.setModel(MS5837::MS5837_30BA);
  sensor.setFluidDensity(997); // kg/m^3 (freshwater, 1029 for seawater)
}

void loop()
{
  // Update pressure and temperature readings
  sensor.read();
  if(LightFlag==0)
  {
    LightFlag=1;
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    LightFlag=0;
    digitalWrite(LED_BUILTIN, LOW);
  }
  *WTemp=sensor.temperature()*100; 
  *WDepth=sensor.depth()*100; 
  Serial.write(WString,4);
}

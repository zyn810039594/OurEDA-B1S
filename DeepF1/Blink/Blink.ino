#include <Wire.h>

#include <MS5837.h>

MS5837 sensor;

void setup() {
  Serial.begin(921600);
  Serial.println("Starting");
  
  Wire.begin();

  if (!sensor.init()) {
    Serial.println("Init failed!");
    Serial.println("\n\n\n");
    delay(5000);
  }
  sensor.setModel(MS5837::MS5837_30BA);
  sensor.setFluidDensity(997); // kg/m^3 (freshwater, 1029 for seawater)
}

// the loop function runs over and over again forever
void loop() {
  sensor.read();
  Serial.print("Pressure: "); 
  Serial.print(sensor.pressure()); 
  Serial.println(" mbar");
  
  Serial.print("Temperature: "); 
  Serial.print(sensor.temperature()); 
  Serial.println(" deg C");
  
  Serial.print("Depth: "); 
  Serial.print(sensor.depth()); 
  Serial.println(" m");
  
  Serial.print("Altitude: "); 
  Serial.print(sensor.altitude()); 
  Serial.println(" m above mean sea level");

  delay(1000);
}

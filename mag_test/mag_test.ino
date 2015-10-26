#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
 
/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
float Pi = 3.14159;
 
void setup() 
{
  Serial.begin(9600);
  Serial.println("Magnetometer Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
}
 
void loop() 
{
  while (getMag() < 50 || getMag() > 60) {
    Serial.println(getMag());
  }
  Serial.println("stopped");
}

int getMag() {
  sensors_event_t event; 
  mag.getEvent(&event);
  float heading = (atan2(event.magnetic.y,event.magnetic.x) * 180) / Pi;
  if (heading < 0) {
    heading += 360;
  }
  return heading;
}


#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

#define CMD               (byte)0x10                                                      //Access Byte for Command Register

#define MD25              0x58                                                            //MD25 I2C Address
#define SPEED             (byte)0x00                                                      //Send Speed Byte
#define TURN              0x01                                                            //Send Turn Byte
#define VOLTS             0x0A                                                            //Read Byte for Battery Voltage
#define ENCODER           0x02                                                            //Read Byte for Encoder 1
#define RESETENCODERS     0x20                                                            //Byte to Reset Encoders
#define MOTORMODE         0x0F                                                            //Byte To Access Operation Mode Register

#define LINSPEED          50                                                              //Define Motor Speeds for Linear Travel
#define ROTSPEED          50                                                              //Define Motor Speeds for Turning Travel
#define fTURNSPEED        10                                                              //Define Motor Speeds for (F)ixed Point Rotation
#define AXELWIDTH         126                                                              //Define Half Distance Between Wheels
#define WHEELRAD          50                  

Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);   

const float Pi = 3.14159;


void setup() {
  Serial.begin(9600);
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
  Wire.begin();
  Wire.beginTransmission(MD25);
  Wire.write(MOTORMODE );
  Wire.write(0x02);
  Wire.endTransmission();
  
  Wire.begin();
  Wire.beginTransmission(MD25);
  Wire.write(0x10);
  Wire.write(0x32);
  Wire.endTransmission();
  encoderReset();

  line(340);
  turn('R', 90, 0);
  line(260);
  turn('L', 90, 0);
  line(500);
  turn('L', 90, 0);
  turn('R', 90, 260);
  turn('R', 90, 0);
  line(660);
  turn('L', 90, 0);
  line(400);
  turn('L', 90, 0);
  line(400);
  turn('L', 90, 0);
  line(400);
  turn('L', 40, 0);
  line(622);//change to float
  turn('L', 140, 0);
  line(180);
  turn('R', 90, 0);
  turn('R', 270, 180);
  turn('L', 143, 0);//change to float
  line(362);//change to float
  line(428);//change to float

  encoderReset();

}
 
void loop() {
}

long encoder(){//fully working                                                                          //Returns MD25 Encoder Value
  Wire.beginTransmission(MD25);
  Wire.write(ENCODER);
  Wire.endTransmission();  
  Wire.requestFrom(MD25, 4);
  while(Wire.available() < 4);
  long poss1 = Wire.read();
  poss1 <<= 8;
  poss1 += Wire.read();
  poss1 <<= 8;
  poss1 += Wire.read();
  poss1 <<= 8;
  poss1  +=Wire.read();  
  return(poss1);
}

void encoderReset() {//fully working                                                                   //Reset MD25 Encoder Value
  Wire.beginTransmission(MD25);
  Wire.write(CMD);
  Wire.write(RESETENCODERS);
  Wire.endTransmission();
}

float distanceTravelled() {
  float e = encoder();
  return ((e/360L)*2L*Pi*WHEELRAD);
}
void line(int Distance) {
      encoderReset();
      Wire.beginTransmission(MD25);
      Wire.write(SPEED);
      Wire.write(178);//arbitrary atm, change later
      Wire.endTransmission();
    while (distanceTravelled() < Distance) {//encoder() may need to be converted to different units. Needs testing
      Serial.println("encoder:");
      Serial.print(encoder());
      Serial.println("Distance:");
      Serial.print(distanceTravelled());
    }
    encoderReset();
    
    stopMotorLine();
    encoderReset();
    delay(500);
}

void turn(char Direction, int Phi, int Radius) {//Needs testing
  int linearSpeed;
  int rotationalSpeed;
  float arcLength;
  float ratio = 126.0 / Radius;
  if (Radius == 0) {
    linearSpeed = 0;
    rotationalSpeed = ROTSPEED;
  } else { 
    linearSpeed = LINSPEED;
    rotationalSpeed = round(linearSpeed * ratio);
  }
  
  if (Direction == 'R') {
    Wire.beginTransmission(MD25);
    Wire.write(SPEED);
    Wire.write(128 + linearSpeed);
    Wire.endTransmission();
  
    Wire.beginTransmission(MD25);
    Wire.write(TURN);
    Wire.write(128 + rotationalSpeed);
    Wire.endTransmission();
    
    arcLength = (((Phi * 2 * Pi) / 360) * (WHEELRAD + Radius));
  } else if (Direction == 'L') {
    Wire.beginTransmission(MD25);
    Wire.write(SPEED);
    Wire.write(128 + linearSpeed);
    Wire.endTransmission();
  
    Wire.beginTransmission(MD25);
    Wire.write(TURN);
    Wire.write(128 - rotationalSpeed);
    Wire.endTransmission();
    arcLength = (((Phi * 2 * Pi) / 360) * (Radius - WHEELRAD));
  }
    
    while (distanceTravelled() < arcLength) {//encoder() may need to be converted to different units. Needs testing
    }
  encoderReset();
  
  stopMotorLine();
  stopMotorTurn();
  encoderReset();
  delay(500);
}

void stopMotorLine()  {                                                                       //Stops motors
  Wire.beginTransmission(MD25);
  Wire.write(SPEED);
  Wire.write(128);
  Wire.endTransmission();
}

void stopMotorTurn() {
  Wire.beginTransmission(MD25);
  Wire.write(TURN);
  Wire.write(128);
  Wire.endTransmission();
}

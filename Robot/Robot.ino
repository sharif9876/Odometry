#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

#define CMD               (byte)0x00                                                      //Access Byte for Command Register

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
#define AXELWIDTH         10                                                              //Define Half Distance Between Wheels
#define WHEELRAD          10                                                              //Define Wheel Radius


Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);                     //Assign Sensor ID

const float Pi = 3.14159;
const int inputPin = 3;
const int soundPin = 2
const int ledPinR = 1;
const int ledPinG = 0;
 
void setup() {
  Serial.begin()
  Wire.begin();
  pinMode(inputPin, INPUT);
  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  /* Initialise the sensor */
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
  delay(100);
  Wire.beginTransmission(MD25);
  Wire.write(MOTORMODE);
  Wire.write(3);
  Wire.endTransmission();
}
 
void loop() {
  digitalWrite(ledPinG, HIGH);
  digitalWrite(ledPinR, LOW);
  if (digitalRead inputPin == LOW){
    checkpointIndicate(0);
    //LIST COMMANDS TO DRAW PATH HERE. INDICATE CHECKPOINT AFTER EACH ACTION. TYPE = 0.
    checkpointIndicate(1);
  } 
}


int getHeading() {                                                                        //Returns magnetic heading of device
  sensors_event_t event; 
  mag.getEvent(&event);
  float heading = (atan2(event.magnetic.y,event.magnetic.x) * 180) / Pi;
  if (heading < 0) {
    heading += 360;
  }
  return heading;
}

void stopMotor()  {                                                                       //Stops motors
  Wire.beginTransmission(MD25);
  Wire.write(SPEED);
  Wire.write(0);
  Wire.endTransmission();

  Wire.beginTransmission(MD25);
  Wire.write(TURN);
  Wire.write(0);
  Wire.endTransmission();
}


void line(int Distance){                                                                  //Takes Distance to travel Parameter
  digitalWrite(ledPinR, HIGH);
  do {
    Wire.beginTransmission(MD25);
    Wire.write(SPEED);
    Wire.write(LINSPEED);
    Wire.endTransmission();
  }while(((((encoder())/360)*2*Pi*WHEELRAD) < Distance));
  stopMotor();
  encoderReset();
  digitalWrite(ledPinR, LOW);  
}

void turn(char Direction, int Phi, int Radius) {                                          //Takes Parameters:
  digitalWrite(ledPinR, HIGH);                                                            //Direction as 'L' or 'R' 
  int dv;                                                                                 //Phi angle to turn, always positive
  int v;                                                                                  //Radius of turn (set 0 for point turn)
  int startHeading = getHeading();
  int targetHeading;
  if (radius > 0){
    v = ROTSPEED;
    dv = (v*((Radius + AXELWIDTH)/Radius))-v;
  }
  else {
    v = 0;
    dv = fTURNSPEED;
  }
  if (Direction == 'R'){
    dv = -dv;
    if ((startHeading + Phi) > 360){
      targetHeading = (startHeading + Phi) - 360;
    }
    else {
      targetHeading = (startHeading + Phi);
    }
  }
  else if (Direction == 'L'){
    if ((startHeading - Phi) < 0){
      targetHeading = (startHeading - Phi) + 360;
    }
    else {
      targetHeading = (startHeading - Phi);
    }
  }

  do {
    Wire.beginTransmission(MD25);
    Wire.write(SPEED);
    Wire.write(v);
    Wire.endTransmission();
  
    Wire.beginTransmission(MD25);
    Wire.write(TURN);
    Wire.write(dv);
    Wire.endTransmission()
  }while(getHeading() < (targetHeading - 1) || getHeading() > (targetHeading + 1));
  stopMotor(); 
  encoderReset();
  digitalWrite(ledPinR, LOW);
}

long encoder(){                                                                           //Returns MD25 Encoder Value
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
  delay(50);  
  return(poss1);

void encoderReset() {                                                                     //Reset MD25 Encoder Value
  Wire.beginTransmission(MD25);
  Wire.write(CMD);
  Wire.write(RESETENCODERS);
  Wire.endTransmission();
}

void checkpointIndicate(int type) {                                                       //Checkpoint indicator. Type 0 or 1 for different indicators                                                      
  digitalWrite(ledPinR, LOW);
  int i;
  int n;
  int jingle[];
  if (type == 0){
    jingle[] = {44, 44, 65};
    n = 3;
  }
  else if (type == 1){
    jingle[] = {44, 65, 44, 65};
    n = 4;
  }
  for (i=0; i=n; i++) {
    digitalWrite(ledPinG, HIGH);
    tone(soundPin, jingle[i]);
    delay(300);
    digitalWrite(ledPinG, LOW);
    noTone();
    delay(100);
  } 
}



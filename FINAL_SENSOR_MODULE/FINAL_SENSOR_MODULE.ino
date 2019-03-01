#include <VirtualWire.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <Narcoleptic.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <L3G4200D.h>
#include "HMC5883L.h"
#include <SD.h>

Adafruit_BMP085 bmp;
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
L3G4200D gyro;
HMC5883L mag;

int16_t avx, avy, avz;
int16_t mx, my, mz;

#define LED_PIN 13
bool blinkState = false;

int chipSelect = 10;
int pow_pin = 8;

long id = 1;  //Serial No. in the SD Card File

void setup()
{
  Serial.begin(9600);
  Serial.println("set up");
  Wire.begin();

  // Initialise the IO and ISR
  vw_set_tx_pin(3);
  vw_setup(2000);       

  if (!bmp.begin())
  {
    Serial.println(F("Could not find a valid BMP085 sensor, check wiring!"));
    while (1) 
    {
    }
  }

  Serial.println(F("Accelerometer Test")); 
  Serial.println("");
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println(F("Ooops, no ADXL345 detected ... Check your wiring!"));
    while(1);
  }

  // initialize device
  Serial.println(F("Initializing I2C devices..."));
  gyro.initialize();

  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(gyro.testConnection() ? "L3G4200D connection successful" : "L3G4200D connection failed");

  // configure LED for output
  pinMode(LED_PIN, OUTPUT);

  // data seems to be best when full scale is 2000
  gyro.setFullScale(2000);

  Serial.println(F("Initializing I2C devices..."));
  mag.initialize();

  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(mag.testConnection() ? "HMC5883L connection successful" : "HMC5883L connection failed");

  // configure Arduino LED for
  pinMode(LED_PIN, OUTPUT);

  Serial.println(F("Initializing SD Card Data Logger"));

  pinMode(chipSelect, OUTPUT);

  pinMode(pow_pin,OUTPUT);
  digitalWrite(pow_pin, HIGH);

  //Checking if card is ready

  if(!SD.begin(chipSelect))
  {
    Serial.println(F("Card Initialization Failed"));
    return;
  }
  else
  {
    Serial.println(F("Card INITIALIZED"));
  }

  File logFile = SD.open("logger.txt", FILE_WRITE);
  if(logFile)
  {
    logFile.println(", , ");
    String header = "ID, Temp, Pa";
    logFile.println(header);
    logFile.close();
    Serial.println(header);
  }
  else
  {
    Serial.println(F("Couldn't Open File in setup"));
  }
  Serial.println(F("----------------------------------------------------------------------------------------"));
}


void loop()
{
  Narcoleptic.delay(500); // During this time power consumption is minimised
 
  int temp = (bmp.readTemperature()) ;
  int Pa = (bmp.readPressure()*0.000145037738);

  Serial.print(F("Temperature = "));
  Serial.print(temp);
  Serial.println(F(" *C"));
  Serial.print(F("Pressure = "));
  Serial.print(Pa);
  Serial.println(F(" Psi"));


  Serial.println();
  delay(500);

  sensors_event_t event; 
  accel.getEvent(&event);

  int acc_x = event.acceleration.x;
  int acc_y = event.acceleration.y;
  int acc_z = event.acceleration.z;

  int acc_pitch = atan(acc_x/sqrt(pow(acc_y,2) + pow(acc_z,2)));
  int acc_roll = atan(acc_y/sqrt(pow(acc_x,2) + pow(acc_z,2)));

  int  pitch = acc_pitch*(180.0/PI);
  int  roll = acc_roll*(180.0/PI);

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print(F("X: ")); 
  Serial.print(acc_x); 
  Serial.print(F("  "));
  Serial.print(F("Y: ")); 
  Serial.print(acc_y); 
  Serial.print(F("  "));
  Serial.print(F("Z: ")); 
  Serial.print(acc_z); 
  Serial.print(F("  "));
  Serial.println(F("m/s^2 "));
  Serial.println(F(" "));

  Serial.print(F("Pitch :\t"));
  Serial.print(pitch);
  Serial.print(F("\t"));
  Serial.print(F("Roll :\t"));
  Serial.println(roll);
  delay(500);

  gyro.getAngularVelocity(&avx, &avy, &avz);

  Serial.print(F("angular velocity:\t"));
  Serial.print(avx); 
  Serial.print(F("\t"));
  Serial.print(avy); 
  Serial.print(F("\t"));
  Serial.println(avz);
  Serial.println(F(" "));
  // blink LED to indicate activity
  blinkState = !blinkState;
  digitalWrite(LED_PIN, blinkState);

  // read raw heading measurements from device
  mag.getHeading(&mx, &my, &mz);


  // display tab-separated gyro x/y/z values
  Serial.print(F("mag:\t"));
  Serial.print(F("x: "));
  Serial.print(mx); 
  Serial.print(F("\t"));
  Serial.print(F("\t"));
  Serial.print(F("y: "));
  Serial.print(my); 
  Serial.print(F("\t"));
  Serial.print(F("\t"));
  Serial.print(F("z: "));
  Serial.println(mz); 
  Serial.println(F(" "));
  Serial.print(F("\t"));

  // To calculate heading in degrees. 0 degree indicates North
  int heading = atan2(my, mx);
  if(heading < 0)
    heading += 2 * M_PI;

  Serial.print(F("heading:\t"));
  int head = heading * 180/M_PI;
  Serial.println(head);
  Serial.println(F(" "));

  // blink LED to indicate activity
  blinkState = !blinkState;
  digitalWrite(LED_PIN, blinkState);

  //  const int bytes = 30; 
  // uint8_t name[bytes];
  const int bytes = 3 * sizeof(int); 
  int name[bytes];

  //Serial.print("Size of buffer: ");
  //Serial.println(sizeof(name));
  name[0]=243; 
  name[1]=temp; 
  name[2]=Pa;
  //  name[3]=acc_x;
  //  name[4]=acc_y;
  //  name[5]=acc_z;
  //  name[6]=avx;
  //  name[7]=avy;
  //  name[8]=avz;
  //  name[9]=pitch;
  //  name[10]=roll;
  //  name[11]=mx;
  //  name[12]=my;
  //  name[13]=mz;
  //  name[14]=heading * 180/M_PI;


  send((byte*)name,bytes);
  delay(2000);

  // }

  // String dataString = String(id) + "," + String(temp) + "," + String(Pa) + "," + String(acc_x) + "," + String(acc_y) + "," + String(acc_z) + "," + String(avx) + "," + String(avy) + "," + String(avz) + "," + String(pitch) + "," + String(roll) + "," + String(mx) + "," + String(my) + "," + String(mz) + "," + String(head);
  String dataString = String(id) + "," + String(temp) + "," + String(Pa);

  //Open a file to write to
  //Only one file can be opened at a time
  File logFile = SD.open("logger.txt", FILE_WRITE);
  if(logFile)
  {
    logFile.println(dataString);
    logFile.close();
    Serial.println(dataString);
  }
  else
  {
    Serial.println(F("Couldn't access file"));
  }

  id++;      //Increment id file

  delay(1000);
}

//void displaySensorDetails(void)
//{
//  sensor_t sensor;
//  accel.getSensor(&sensor);
//  Serial.println("------------------------------------");
//  Serial.print  ("Sensor:       "); 
//  Serial.println(sensor.name);
//  Serial.print  ("Driver Ver:   "); 
//  Serial.println(sensor.version);
//  Serial.print  ("Unique ID:    "); 
//  Serial.println(sensor.sensor_id);
//  Serial.print  ("Max Value:    "); 
//  Serial.print(sensor.max_value); 
//  Serial.println(" m/s^2");
//  Serial.print  ("Min Value:    "); 
//  Serial.print(sensor.min_value); 
//  Serial.println(" m/s^2");
//  Serial.print  ("Resolution:   "); 
//  Serial.print(sensor.resolution); 
//  Serial.println(" m/s^2");  
//  Serial.println("------------------------------------");
//  Serial.println("");
//  delay(500);
//}
//
//void displayDataRate(void)
//{
//  Serial.print  ("Data Rate:    "); 
//
//  switch(accel.getDataRate())
//  {
//  case ADXL345_DATARATE_3200_HZ:
//    Serial.print  ("3200 "); 
//    break;
//  case ADXL345_DATARATE_1600_HZ:
//    Serial.print  ("1600 "); 
//    break;
//  case ADXL345_DATARATE_800_HZ:
//    Serial.print  ("800 "); 
//    break;
//  case ADXL345_DATARATE_400_HZ:
//    Serial.print  ("400 "); 
//    break;
//  case ADXL345_DATARATE_200_HZ:
//    Serial.print  ("200 "); 
//    break;
//  case ADXL345_DATARATE_100_HZ:
//    Serial.print  ("100 "); 
//    break;
//  case ADXL345_DATARATE_50_HZ:
//    Serial.print  ("50 "); 
//    break;
//  case ADXL345_DATARATE_25_HZ:
//    Serial.print  ("25 "); 
//    break;
//  case ADXL345_DATARATE_12_5_HZ:
//    Serial.print  ("12.5 "); 
//    break;
//  case ADXL345_DATARATE_6_25HZ:
//    Serial.print  ("6.25 "); 
//    break;
//  case ADXL345_DATARATE_3_13_HZ:
//    Serial.print  ("3.13 "); 
//    break;
//  case ADXL345_DATARATE_1_56_HZ:
//    Serial.print  ("1.56 "); 
//    break;
//  case ADXL345_DATARATE_0_78_HZ:
//    Serial.print  ("0.78 "); 
//    break;
//  case ADXL345_DATARATE_0_39_HZ:
//    Serial.print  ("0.39 "); 
//    break;
//  case ADXL345_DATARATE_0_20_HZ:
//    Serial.print  ("0.20 "); 
//    break;
//  case ADXL345_DATARATE_0_10_HZ:
//    Serial.print  ("0.10 "); 
//    break;
//  default:
//    Serial.print  ("???? "); 
//    break;
//  }  
//  Serial.println(" Hz");  
//}
//
//void displayRange(void)
//{
//  Serial.print  ("Range:         +/- "); 
//
//  switch(accel.getRange())
//  {
//  case ADXL345_RANGE_16_G:
//    Serial.print  ("16 "); 
//    break;
//  case ADXL345_RANGE_8_G:
//    Serial.print  ("8 "); 
//    break;
//  case ADXL345_RANGE_4_G:
//    Serial.print  ("4 "); 
//    break;
//  case ADXL345_RANGE_2_G:
//    Serial.print  ("2 "); 
//    break;
//  default:
//    Serial.print  ("?? "); 
//    break;
//  }  
//  Serial.println(" g");  
//}
//

void send(byte *name,int length)
{
  vw_send(name,length);
  vw_wait_tx();
  /*  vw_send((uint8_t *)name, strlen(length));
   vw_wait_tx(); // Wait until the whole message is gone*/

}










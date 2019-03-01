#include <SD.h>
#include <Time.h>
#include <VirtualWire.h>
#include <stdio.h>

int chipSelect = 10;
int pow_pin = 8;

long id = 1;  //Serial No. in the SD Card File

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

int menu_item = 0; // defining the integers data type
int newNode[15];
int system_id;

const int BUZZER = 7; 

String room_names[15];

int temp_led = 5;
int p_led = 4;

int readings [60][5] = {
  0, 0, 0, 0,0,
  1, 0, 0, 0,0,
  2, 0, 0, 0,0,
  3, 0, 0, 0,0,
  4, 0, 0, 0,0,
  5, 0, 0, 0,0,
  6, 0, 0, 0,0,
  7, 0, 0, 0,0,
  8, 0, 0, 0,0,
  9, 0, 0, 0,0,
  10, 0, 0, 0,0,
  11, 0, 0, 0,0,
  12, 0, 0, 0,0,
  13, 0, 0, 0,0,
  14, 0, 0, 0,0,
  15, 0, 0, 0,0,
  16, 0, 0, 0,0,
  17, 0, 0, 0,0,
  18, 0, 0, 0,0,
  19, 0, 0, 0,0,
  20, 0, 0, 0,0,
  21, 0, 0, 0,0,
  22, 0, 0, 0,0,
  23, 0, 0, 0,0 };

void setup()
{
  Serial.begin(9600);	  // Debugging only
  Serial.println(F("setup"));

  setSyncProvider(requestSync);  //set function to call when sync required
  Serial.println(F("Waiting for sync message"));
  delay(500);
  settime();
  delay(500);

  vw_setup(2000);	 // Bits per sec
  vw_set_tx_pin(3); //Transmitter Data Pin to Digital Pin 3

    vw_setup(2000);
  vw_set_rx_pin(6);
  vw_rx_start();

  pinMode(temp_led,OUTPUT);
  pinMode(p_led,OUTPUT);
    
  Serial.println(F("----------------------------------------------------------------------------------------"));

  do
  {
    displaymenu();
    menu_item = 0;
  }
  while (menu_item > 1);

}//close setup

void loop()
{

  if (Serial.available() > 0)
  {
    int sendByte = Serial.read();//Starts reading data from the serial monitor once condition is met


    switch (sendByte)
    {

    case 'f':  //if the controller type f
      {
        Serial.println(F("forward"));
        char *msg2 = "f";
        digitalWrite(13, true); // Flash a light to show transmitting
        vw_send((uint8_t *)msg2, strlen(msg2));//send byte to the receiver
        vw_wait_tx(); // Wait until the whole message is gone
        digitalWrite(13, false);
        delay(500);
        break;

      }
    case 'b':  //if controller types b
      {
        Serial.println(F("backward"));
        char *msg2 = "b";
        digitalWrite(13, true); // Flash a light to show transmitting
        vw_send((uint8_t *)msg2, strlen(msg2));//send byte to the receiver
        vw_wait_tx(); // Wait until the whole message is gone
        digitalWrite(13, false);
        delay(500);
        break;
      }

    case 'x': //if controller types s
      {
        Serial.println(F("stop"));
        char *msg2 = "x";
        digitalWrite(13, true); // Flash a light to show transmitting
        vw_send((uint8_t *)msg2, strlen(msg2));//send byte to the receiver
        vw_wait_tx(); // Wait until the whole message is gone
        digitalWrite(13, false);
        delay(500);
        break;
      }

    case 'l': //if controller types l
      {  
        char *msg2 = "l";
        digitalWrite(13, true); // Flash a light to show transmitting
        vw_send((uint8_t *)msg2, strlen(msg2));//send byte to the receiver
        vw_wait_tx(); // Wait until the whole message is gone
        digitalWrite(13, false);
        break;
      }     
    case 'r': //if controller types r
      {
        char *msg2 = "r";
        digitalWrite(13, true); // Flash a light to show transmitting
        vw_send((uint8_t *)msg2, strlen(msg2));//send byte to the receiver
        vw_wait_tx(); // Wait until the whole message is gone
        digitalWrite(13, false);
        break;
      }
    case'T':            // if user enters 'T' then print the unix time
      {
        settime();   
        break;    
      }
    case 'i':
      {
        setSystemID();
        break;
      }
    case 'c':
      {
        Connect();
        break;
      }
    case 'd':  
      { 
        Disconnect();    // if user enters 'x' then erase database
        break;
      }    
    case 'w':
      {
        dumpSD();
        break;

      }
    case 's':
      {
        Serial.println(F("string scan"));
        msgscan();
        Serial.println(F("stopped *****string scan"));
        displaymenu();
        break;
      }      
    default://if any other value is entered
      {

        Serial.println(F("wrong entry"));//print wrong entry in the serial monitor
      }
    }// close switch-case
  }//close if 
}//close loop

void displaymenu()
{
  //digitalClockDisplay();
  Serial.println(F("In-Pipe Inspection Robot"));
  Serial.println(F("Menu options"));
  Serial.println(F("-------------------------"));
  Serial.println(F("T - set time"));
  Serial.println(F("i - set system id"));
  Serial.println(F("c - Connect to robot"));
  Serial.println(F("d - Disconnect"));
  Serial.println(F("w - dump to CSV"));
  Serial.println(F("--------------------------"));
  Serial.println(F("motor control"));
  Serial.println(F("--------------------------"));
  Serial.println(F("l - expand limbs"));
  Serial.println(F("r - contract limbs"));
  Serial.println(F("f - forward"));
  Serial.println(F("b - backward"));
  Serial.println(F("x - stop"));
  Serial.println(F("--------------------------"));
  Serial.println(F("s - start"));
  Serial.println(F("q - quit"));
  Serial.println(F("--------------------------"));
  return;

}
int settime()
{
  if (Serial.available()) 
  {
    processSyncMessage();
  }
  if (timeStatus()!= timeNotSet)
  {
    digitalClockDisplay();  
  }

  delay(1000);
  return 1;
}


void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013
  int daylight = 3600;

  if(Serial.find(TIME_HEADER)) {
    pctime = Serial.parseInt();
    pctime += daylight;
    if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
      setTime(pctime); // Sync Arduino clock to the time received on the serial port
    }
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}



int setSystemID()
{
  system_id= 15;
  Serial.println(F("setSystemID"));
  Serial.println(system_id); 
  return 1; 
} 

int Connect()
{
  Serial.println(F("Connecting....."));
  delay(500);
  boolean gotit = false;
  int option;
  while (!gotit)
  {
    if (Serial.available() > 0)
    {
      // read the incoming byte:
      option = Serial.parseInt();
      Serial.println(F(" "));
      Serial.print(F("Node= "));
      Serial.println(option);
      delay(100);
      if (option > 0 && option < 16)
      {
        gotit = true;
        newNode[option] = 1;
        //delay(500);

      }
      Serial.print(F("Connected to node "));
      delay(500);

    }
    else
    {
      Serial.print(option);
      Serial.print(F("   ** out of range - enter a number between 1 and 15 **"));
    }
  }
}

int Disconnect()
{

  boolean gotit = false;
  int option;
  while (!gotit)
  {
    if (Serial.available() > 0)
    {
      // read the incoming byte:
      option = Serial.parseInt();
      Serial.println(F(" "));
      if (option > 0 && option < 16)
      {
        gotit = true;
        newNode[option] = 0;
        Serial.print(F("Disconnect from node"));
        //delay(1000);

      }
      else
      {
        Serial.print(option);
        Serial.print(F("   ** out of range - enter a number between 1 and 15 **"));
      }
    }
  }
  return option;
}

int dumpSD()
{
    Serial.println(F("Initializing SD Card Data Logger"));

  pinMode(chipSelect, OUTPUT);

  pinMode(pow_pin,OUTPUT);
  digitalWrite(pow_pin, HIGH);

  //Checking if card is ready

  if(!SD.begin(chipSelect))
  {
    Serial.println(F("Card Initialization Failed"));
  }
  else
  {
    Serial.println(F("Card INITIALIZED"));
  }

  File logFile = SD.open("logger2.txt", FILE_WRITE);
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
   for(int ll = 0 ; ll<23 ;ll++)
  {
  String dataString = String(id) + "," + String(readings[ll][1]) + "," + String(readings[ll][2]);

  //Open a file to write to
  //Only one file can be opened at a time
  File logFile = SD.open("logger2.txt", FILE_WRITE);
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
}
void msgscan()
{
  //Serial.read();
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  uint8_t buf[buflen];

  int exitnow = 0;

  while(!exitnow)
  {
    if (Serial.available() > 0) // reading the data from the serial portn which is already stored in the serial reciever buffer
    {
      if(Serial.read() == 'q')
        exitnow = 1;

    }

    if (vw_get_message(buf, &buflen)) // check to see if anything has been received
    {
      int i;
      // Message with a good checksum received. 
      for (i = 0; i < buflen; i++)
      {
        //Serial.println(i);
        Serial.println(buf[i]);
        //        char* message = (char*)buf;
        //        int value = atoi(message);
        //        Serial.println(value); 
        // the received data is stored in buffer
      }
      //      Serial.println("");
    }
    //    Serial.println(buf[0]);

    int temp = buf[0];

    int nid = temp - 240;
    int hid = temp - nid;


    int err = 0 ;
    if(hid == 240)

    {

      if( newNode[nid] == 1)
      {
        Serial.println(F("--------------------------------------------------------------------------------------------------"));
        Serial.println(F("--------------------------------------------------------------------------------------------------"));

        Serial.print(F("System ID: \t"));
        Serial.println(hid,DEC); // printing the value of the system ID in decimal

        Serial.print(F("NODE ID: "));
        Serial.println(nid,DEC); // printing the value of Node ID in decimal

        Serial.print(F("Temperature = "));
        Serial.print(buf[2],DEC); // printing the value of humidity in decimal
        Serial.println(" *C");
        //Serial.print("\t");
        Serial.println(F(""));
        if(buf[2]>25)
        {
          makenoise();
          Serial.println(F("System temperature is Exceeded Critical Limit"));
          Serial.println(F(""));
          digitalWrite(temp_led,HIGH);
          delay(1000);
          digitalWrite(temp_led,LOW);
          delay(500);
        }
        else if(buf[2]<=20)
        {
          makenoise();
          Serial.println(F("System temperature is fallen below Minimum Limit"));
          Serial.println(F(""));
          digitalWrite(temp_led,HIGH);
          delay(1000);
          digitalWrite(temp_led,LOW);
          delay(500);          
        }
        else
        {
          Serial.println(F("System temperature is in control"));
          Serial.println(F(""));
          digitalWrite(temp_led,HIGH);    
        }

        Serial.print(F("Pressure = "));
        Serial.print(buf[4],DEC); // printing the value of temperature in decimal
        Serial.println(" Psi");
        Serial.println(F(""));
        if(buf[4]>20)
        {
          makenoise();
          Serial.println(F("System preassure is Exceeded Critical Limit"));
          Serial.println(F(""));
          digitalWrite(p_led,HIGH);
          delay(1000);
          digitalWrite(p_led,LOW);
          delay(500);
        }
        else if(buf[4]<=12)
        {
          makenoise();
          Serial.println(F("System preassure is fallen below Minimum Limit"));
          Serial.println(F(""));
          digitalWrite(p_led,HIGH);
          delay(1000);
          digitalWrite(p_led,LOW);
          delay(500);
        }
        else
        {
          Serial.println(F("System preassure is in control"));
          Serial.println(F(""));
          digitalWrite(p_led,HIGH); 
        }

        Serial.println(F("--------------------------------------------------------------------------------------------------"));
        Serial.println(F("--------------------------------------------------------------------------------------------------"));

        //update the array with details read
        readings[nid][0] = nid;          //node id
        readings[nid][1] = buf[2];       //temp
        readings[nid][2] = buf[4];       //pressure

        delay(1000);

      }
      else
      {
        err = 1;
      }

    }

    else
    {
      err = 1;
    }
    if(err)
    {
      Serial.println("not recognized System ID:  ");

    }
  }
}


void makenoise(void)  
{      // This function is basically made because we are asked to make the buzzer sound three short beeps
  // These three different noises will tell us that the reaction timer has ended
  tone(BUZZER,2000,400); // tone function is used on 1500 frequency and 400 milliseconds
  delay(300);              // delay function is used to cause delay for 200 milliseconds
  tone(BUZZER,2000,400);
  delay(300);        // delay function is used to cause delay for 200 milliseconds
  tone(BUZZER,2000,400);
}











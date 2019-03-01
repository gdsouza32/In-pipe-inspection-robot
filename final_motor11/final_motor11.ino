
#include <VirtualWire.h>
#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 5,6,11,12);   

// Motor 1
int dir1PinA = 2;
int dir2PinA = 3;
int speedPinA = 9; // Needs to be a PWM pin to be able to control motor speed

// Motor 2
int dir1PinB = 8;
int dir2PinB = 7;
int speedPinB = 10; // Needs to be a PWM pin to be able to control motor speed

int motorPin[] = {
  2, 3, 8, 7, 9, 10};//array for storing pin nos

void setup()
{
  Serial.begin(9600);	// Debugging only
  Serial.println("setup");

  //  pinMode(VCC,OUTPUT);
  //  pinMode(GND,OUTPUT);
  //  digitalWrite(VCC,HIGH);
  //  digitalWrite(GND,LOW);

  // Initialise the IO and ISR
  vw_setup(2000);	 // Bits per sec
  vw_set_rx_pin(4);//Receiver at Digital Pin 2
  vw_rx_start();// Start the receiver PLL running
  // Serial.println("setup");
  /*
  for (int i = 0; i < 6; i++) 
   { 
   pinMode(motorPin[i], OUTPUT);
   //Serial.println("pinmode");
   }*/

  //This is basically what the for loop does :-
  pinMode(dir1PinA,OUTPUT);
  pinMode(dir2PinA,OUTPUT);
  pinMode(speedPinA,OUTPUT);
  pinMode(dir1PinB,OUTPUT);
  pinMode(dir2PinB,OUTPUT);
  pinMode(speedPinB,OUTPUT); 

  // set the stepper motor speed :
  myStepper.setSpeed(100);

}

void loop()
{
  //Serial.println("loop");
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  //Serial.println("-------------------------------------");
  //delay(500);

  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    Serial.println("vw_msg");
    int i;

    digitalWrite(13, true); // Flash a light to show received good message
    // Message with a good checksum received, dump it.

    for (i = 0; i < buflen; i++)
    {
      Serial.print(buf[i]);

      if(buf[i] == 'f')
      {
        forward();//go forward when f is pressed
      }
      if(buf[i] == 'b')
      {
        backward();//go backward when b is pressed
      }
      if(buf[i] == 'x')
      {
        stopMotor();//stop/brake when s is pressed
      }
      if(buf[i] == 'l')
      {
        stepperXpand();//go left when l is presed
      }
      if(buf[i] == 'r')
      {
        stepperContract();//go right when r is pressed
      }

    }//close for loop

    digitalWrite(13, false);

  }//close main if
}//close loop
//you can print the data entered when debugging by adding Serial.println

////////////////////////////////////////////////////////////////////////////////////////

//set of functions
void forward()
{ 
  Serial.println("forward");
  analogWrite(speedPinA, 255);//Sets speed variable via PWM 
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
  Serial.println("Motor 1 Forward"); // Prints out “Motor 1 Forward” on the serial monitor
  Serial.println("   "); // Creates a blank line printed on the serial monitor
  analogWrite(speedPinB, 255);
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);
  Serial.println("Motor 2 Forward");
  Serial.println("   ");
}


void backward()
{
  Serial.println("backward");
  analogWrite(speedPinA, 255);
  digitalWrite(dir1PinA, HIGH);
  digitalWrite(dir2PinA, LOW);
  Serial.println("Motor 1 Reverse");
  Serial.println("   ");
  analogWrite(speedPinB, 255);
  digitalWrite(dir1PinB, HIGH);
  digitalWrite(dir2PinB, LOW);
  Serial.println("Motor 2 Reverse");
  Serial.println("   ");
}

void stepperXpand()
{
  // step one revolution  in one direction:
  Serial.println("stepper clockwise");
  myStepper.step(500);
  delay(500);
}

void stepperContract()
{
  Serial.println("stepper counterclockwise");
  myStepper.step(-200);
  delay(500);
}

void stopMotor()
{
  Serial.println("stop");
  analogWrite(speedPinA, 0);
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, LOW);
  Serial.println("Motor 1 Stop");
  Serial.println("   ");
  analogWrite(speedPinB, 0);
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, LOW);
  Serial.println("Motor 2 Stop");
  Serial.println("   ");
}
//End Of Code













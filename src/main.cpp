//
//include all the necessary libraries
#include <Arduino.h> //arduino library when using VSCode PlatformIO
#include <SPI.h> //SPI library for Arduino
#include <RF24.h> //RF24 library for Arduino

//code adapted from https://mkinventions.blogspot.com/2019/12/nrf24l01-part-3-controling-Joy1readvals.html

//==============PINOUT==============
/*
---Joystick---
A0 - x-axis, can be any ANALOG pin to read pot value
A1 - y-axis, can be any ANALOG pin to read pot value
D6 - click, can be analog or digital, works as push button
---don't forget the VCC and Gnd for the joystick, 5V arduino output is plenty---

---NRF24---
D11(MOSI) - MOSI on NRF24 module, ---CANNOT USE D11 pin for anything else now!---
D12(MISO) - MISO on NRF24 module, ---CANNOT USE D12 pin for anything else now!---
D13(SCK) - SCK on NRF24 module, ---CANNOT USE D13 pin for anything else now!---
D7 - CE on NRF24 module, can be any analog or digital pin
D10 - CS on NRF24 module, can be any digital pin
---don't forget the the VCC and Gnd for the NRF24 module, MUST BE 3.3V!!!
Can use the arduino 3V3 output. Recommend a 10uF cap across the module VCC and GND---
*/

#define joy1xinpin A0 //sets all occurences of "joy1xinpin" (the pin for the joystck x axispin) in the code to A0
#define joy1yinpin A1 //same for joystick y axis
#define joy1butinpin 6 //joystick button

int joy1xval = 0; //variable to hold the value of x axis
int joy1yval = 0; //y axis
int joy1butval = 0; //button state, important to note that with INPUT_PULLUP the logic is "reversed", 1 is NOT pressed, 0 = pressed

RF24 TXmodule(7, 10); // CE, CS pins where the NRF24 module is connected
 
const byte address[6] = {"42"}; //address for the module, AFAIK both TX and RX modules should have same value here
 
int Joy1readvals = 0; //variable to hold the read vaule before writing to NRF transmission
 
void setup() 
{
  // put your setup code here, to run once:
Serial.begin(115200); //set to whatever speed you prefer, for serial debugging

pinMode(joy1xinpin, INPUT);
pinMode(joy1yinpin, INPUT);
pinMode(joy1butinpin, INPUT_PULLUP); //using this mode allows to avoid extra resistors and makes wiring buttons easier

TXmodule.begin(); //initialize the TX module
TXmodule.openWritingPipe(address); //open communication pipe
TXmodule.setPALevel(RF24_PA_MIN); //set power of NRF module, can be changed to RF24_PA_MAX for better range, BUT MUST BE SAME ON THE TX MODULE!!!
TXmodule.stopListening(); //puts the NRF module into TRANSMIT MODE
}

void loop() 
{
  // put your main code here, to run repeatedly:
  joy1xval = analogRead(joy1xinpin); //read joystick or button input values
  joy1yval = analogRead(joy1yinpin); //same
  joy1butval = digitalRead(joy1butinpin); //same

  int Joy1readvals[3]; //initialize array to hold values before writing them to NRF transmission

  Joy1readvals[0] = map(joy1xval, 0, 1023, 179, 0); //sets the array "slots"
  //map() function recalibrates the raw input from the joystick (0-1023) to something that a servo understands (0-179)
  //also REVERSING the direction. To invert the servo direction, change the last two values from 179, 0 to 0, 179 
  Joy1readvals[1] = map(joy1yval, 0, 1023, 0, 179); //same as above
  Joy1readvals[2] = map(joy1butval, 0, 1, 1, 0); //reverses button logic to 1 = button pressed and 0 = button not pressed
  
TXmodule.write(&Joy1readvals, sizeof(Joy1readvals)); //sends the values to be transmitted
 
  delay(5); //stability pause

//serial monitor for debugging
  Serial.print("Joy1x: ");
  Serial.print(Joy1readvals[0]);
  Serial.print(" Joy1y: ");
  Serial.print(Joy1readvals[1]);
  Serial.print(" Joy1butraw: ");
  Serial.print(joy1butval);
  Serial.print(" Joy1butmap: ");
  Serial.println(Joy1readvals[2]);
}
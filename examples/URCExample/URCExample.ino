/*  
  ATHandler - Library for Handling AT Commands Communication

  This example shows the usage of the ATHandler Library to 
  handle incoming URC (Unsolicited Result Codes) over a
  Serial AT Commands Communication
  The library is non-blocking so the MCU can keep working
  while waiting for the URC to come.
  
  Created by Ing. Sebastian M. Irazabal, November 27, 2016.
  www.sebasira.com.ar
*/
#include <ATHandler.h>
#include <SoftwareSerial.h>

// Create a SoftwareSerial or Hardware Serial where communicacion will be stablish
#define RxPin 2
#define TxPin 3
SoftwareSerial  cmdSerial(RxPin, TxPin);

// Create an object to handle AT Commands
ATHandler  atHandler;


/* SETUP */
/*********/
void setup() {
  cmdSerial.begin(9600);

  // Initialize AT CMD Hanlder
  atHandler.begin();
}

/* LOOP */
/********/
void loop() {
  // Every time new data arrives from serial interface, fed it to the AT CMD Hanlder
  if (cmdSerial.available()){
    atHandler.feed(cmdSerial.read());
  }

  // Compare incoming AT Command URC with the desiered one
  if (atHandler.match("+CMTI")){
    /*
     * Do Something when URC arrives
     */
    
    // After processing it, move to the next one
    atHandler.moveNext();
  }

  // If no-one handles the incoming response, because it's not what's desired
  // then move to the next one (a.k.a. skip the incoming response)
  atHandler.moveNext();
}

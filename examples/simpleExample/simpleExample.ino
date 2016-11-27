/*  
  ATHandler - Library for Handling AT Commands Communication

  This example shows the usage of the ATHandler Library to 
  wait for a desired or expected response over a Serial
  AT Commands Communication.
  The library is non-blocking so the MCU can keep working
  while waiting for the response to come.
  
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

bool waitingResponse = false;


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

  // Just to test it, send a dummy AT Command and wait for response
  if (!waitingResponse){
    waitingResponse = true
    cmdSerial.println("AT");
  }

  // Compare incoming AT Command (actually it's a response) with
  // the desired response
  if (atHandler.match("OK")){
    /*
     * Do Something when response arrives
     */
    
    // After processing the response, move to the next one
    atHandler.moveNext();
    waitingResponse = false
  }

  // If no-one handles the incoming response, because it's not what's desired
  // then move to the next one (a.k.a. skip the incoming response)
  atHandler.moveNext();
}

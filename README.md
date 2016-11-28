# ATHandler
Simple &amp; Lightweight Arduino library for Handling AT Commands Communication


This is a very simple to use library for dealing with AT Commands responses and URC (Unsolicited Result Codes).

Instead of blocking code executing until the desired response or URC arrives, the MCU can still work as usual while waiting.

### How to use it
To operate with it you first need to create an ATHandler object
~~~~
  ATHandler atHandler;
~~~~


This object needs to be initialized in the setup
~~~~
void setup(){
  atHandler.begin;
}
~~~~

And then you have to feed it with every data that arrives from the serial interface (it could be hardware or software serial)
~~~~
  if (Serial.available()){
    atHandler.feed(Serial.read());
  }
~~~~

To check for the desired answer or URC just call the match function over the method. There's also a match_N if you wish to test only a few characters.
And finally when the expected response or URC arrives, user must move to the next to deal with the next one. This method should also be called if received a command that no-one was expecting.
~~~~
  if (atHandler.match("OK")){
    /*
     * Do Something when response arrives
     */

    atHandler.moveNext();
  }
~~~~


### Suggestions
If you use this in the loop, I suggest you feed the object right after it starts then do what you want "in the middle" and finally before the loop function roll back you call the moveNext method and ensures that if it was not processed "in the middle" is because no-one knows what it is.

Another suggestion **when dealing with modules with AT Commands, use a Finite State Machine**. They were designed to work that way.


### Configurations
In the .h file you will encounter 2 configurations. One of them is the receive buffer size just set it to whatever (int) you want. To ensure the correct operation, choose the buffer size wisely. I use two pointer to handle the reception buffer: one is called PUT pointer (pointer that stores data) and the other is the GET pointer (pointer that extracts data). When feeding, if the PUT pointer reaches the GET pointer with pending commands to be processed, then it stops storing data, to prevent an overflow. It just discard incoming data until GET pointer moves forward with moveNext.

And the second configuration is a label to enable debug information over hardware serial.

### To-Do
I'm still working on it, one thing that's missing is the ability to handle PROMPT when request to send SMS or Data. Also I've to test it when receiving SMS text or Data.

It was designed to deal with responses and URC, because sending an AT Command is very easy just using a println.


*Distributed under MIT License - Copyright (c) 2016 Sebastian M. Irazabal www.sebasira.com.ar*

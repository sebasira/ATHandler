/*
    ATHandler.h - Library for Handling AT Commands Communication
  
  This is a small and lighweight library to handle incoming AT COMMANDS
  responses
  
  Created by Ing. Sebastian M. Irazabal, November 27, 2016.
  www.sebasira.com.ar
  
  ----------------------------------------------------------------------
  
  MIT License

  Copyright (c) 2016 Sebastian M. Irazabal

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "ATHandler.h"


	/* CONSTRUCTOR */
	/***************/
	/*
	 * Void constructor
	 */
	ATHandler::ATHandler(void){
		
	}

/*******************************************************************************************/
/** PUBLIC METHODS **/
/********************/
	/* BEGIN */
	/*********/
	/* 
	 * Initialize the object
	 */
	void ATHandler::begin(void){
		#ifdef AT_HANDLER_DEBUG_ENABLE
			Serial.begin(9600);
			Serial.println(F("AT HANDLER created"));
		#endif
		putptr = cmd_buffer;
		getptr = cmd_buffer;
	  
		pending_cnt = 0;
	}
	
	
	/* GET POINTER COPY */
	/********************/
	/*
	 * It returns a duplicate of the current GET pointer, so user can used it to
	 * retrieve specific data from the buffer
	 */
	char* ATHandler::copyGET(void){
		 return(getptr);
	}
	 
	 
	 /* INCREASE POINTER */
	 /********************/
	 /*
	  * Performs an increment on the given pointer, treating it as a circular buffer pointer
	  */
	char* ATHandler::increasePointer(char* ptr){
		if (ptr == cmd_buffer + AT_HANDLER_RxBuffer_Size - 1){
			ptr = cmd_buffer;
		}else{
			ptr++;
		}
		
		return ptr;
	}
	  
	
			
	/* FEED */
	/********/
	/* 
	 * Feeds the RxBuffer with data given. It's supposed that this data came for the Serial port
	 */
	void ATHandler::feed(char newData){
		static char prevData;
		static bool cmdJustArrived;
		char* auxptr;

		if (getptr == putptr && pending_cnt > 0){
			// If PUT pointer reaches GET pointer and there are pending commands to be processed then
			// it means that an overflow is occurring (there's no more room in the buffer for the
			// incomming serial data). So I won't insert the newData in the buffer
			#ifdef AT_HANDLER_DEBUG_ENABLE
				Serial.println(F("AT HANDLER no room for more data"));
			#endif
		}else{
			
			// When some modules start up (power up) they send garbage over the serial. Sometimes
			// that garbage is 0x00 (a termination character). This should never be present in
			// the buffer unless it is a command end. So if 0x00 arrives, I will transform it to 0xFF
			if (newData == 0x00){
				#ifdef AT_HANDLER_DEBUG_ENABLE
					Serial.println(F("AT HANDLER garbage received!! -> 0xFF"));
				#endif
				newData = 0xFF;
			}
			
			*putptr = newData;     				// Add newData to the RxBuffer
			
			// Determines if an "end-of-command" : <CR> <LF> equals "\r\n"
			// Added a new condition: If data is > then treat it as a new command has arrive
			if ((prevData == '\r' && newData == '\n') || newData == '>'){
				// If "\rn" has arrived then it means a new command has just arrived. That is if there
				// hasn't just arrive a prior command, because AT COMMANS starts and end with "\r\n" and
				// if we don't put that filter we may think a command has arrived but actually it would
				// mean that a command is stating
				#ifdef AT_HANDLER_DEBUG_ENABLE
					Serial.println(F("AT HANDLER new <CR><LF>"));
				#endif
				if (!cmdJustArrived || newData == '>'){
					pending_cnt++;              // Increase pending command count
					#ifdef AT_HANDLER_DEBUG_ENABLE
						Serial.println(F("AT HANDLER new command pending"));
					#endif
				}else{
					// As said above if a new "\r\n" arrived right after the previous one, then it
					// means that a new command is starting. So in order not to have two 0x00 next
					// to each other I will decrease the pointer
					if (putptr == cmd_buffer){
						putptr = cmd_buffer + AT_HANDLER_RxBuffer_Size - 1;
					}else{
						putptr--;
					}
				}

				cmdJustArrived = true;			// Indicates that an AT Command has arrived

				// When the command arrives as <CR><LF> then I need to decrease pointer in order to
				// overwrite <CR> and set ii to 0x00. But if the command arrives as '>' then I need
				// to increase pointer and add 0x00
				if (newData == '>'){
					// Increase PUT pointer (circular buffer)
					auxptr = putptr;			// Save a copy of the pointer
					if (putptr == cmd_buffer + AT_HANDLER_RxBuffer_Size - 1){
						putptr = cmd_buffer;
					}else{
						putptr++;
					}
					
					// If after increasing the PUT pointer I reach the GET pointer => That's not right
					// overflow is ocurring. RollBack to prevent it
					if (getptr == putptr){
						putptr = auxptr;
						#ifdef AT_HANDLER_DEBUG_ENABLE
							Serial.println(F("AT HANDLER rollback no room"));
						#endif
					}
					
				}else{
					// Decreases PUT pointer (circular buffer)
					if (putptr == cmd_buffer){
						putptr = cmd_buffer + AT_HANDLER_RxBuffer_Size - 1;
					}else{
						putptr--;
					}
				}
				*putptr = 0x00;               	// Adds a new command end (string end)

			}else{
				// If newData is a "\r" I won't clear the justArrived flag because this could
				// be the beginning of a new AT Command. Otherwise, clear the flag
				if (newData != '\r'){
					cmdJustArrived = false; 
				}
			}

			prevData = *putptr;            		// Stores this data so it would be next-data's previous data

			// Increase PUT pointer (circular buffer)
			if (putptr == cmd_buffer + AT_HANDLER_RxBuffer_Size - 1){
				putptr = cmd_buffer;
				//Serial.println(F("----------------- PUT ROLL OVER"));
			}else{
				putptr++;
				//Serial.print(F("PUT "));
				//Serial.println(putptr - cmd_buffer);
			}
		}
	}


	/* CHECK PENDING AT CMD */
	/************************/
	/* 
	 * Checks if there is or are any AT CMD pending
	 */
	bool ATHandler::chkPending(void){
		return(pending_cnt > 0);
	}
	
	
	
	/* MATCH N */
	/***********/
	/* 
	 * Checks if the received AT CMD matches a specific given command comparing N characters
	 */
	bool ATHandler::match_N(char* expected, unsigned int N){
		char* getptr_copy;
		unsigned int maxIterations;
		bool match = false;

		// Only process request if there is any cmd pending
		if (pending_cnt > 0){
			getptr_copy = getptr;           // Make a copy of get pointer

			maxIterations = N;
			do{
				if (*getptr_copy == *expected){
					match = true;
					#ifdef AT_HANDLER_DEBUG_ENABLE
						Serial.print(F("AT HANDLER matched: "));
						Serial.println(*getptr_copy);
					#endif
				}else{
					match = false;
					#ifdef AT_HANDLER_DEBUG_ENABLE
						Serial.print(F("AT HANDLER don't match: "));
						Serial.println(*getptr_copy);
					#endif
				}

				getptr_copy++;
				if (getptr_copy == cmd_buffer + AT_HANDLER_RxBuffer_Size){
					getptr_copy = cmd_buffer;
				}

				expected++;
				maxIterations--;
			}while(*getptr_copy != 0 && *expected != 0 && maxIterations > 0 && match);
		}

		return match;
	}
	
	
	/* MATCH */
	/*********/
	/* 
	 * Checks if the received AT CMD matches a specific given command
	 * This is a MASK of the match_N method using the maximum interation count
	 */
	bool ATHandler::match(char* expected){
		return(match_N(expected, AT_HANDLER_RxBuffer_Size));
	}
	
	
	
	/* MOVES NEXT */
	/**************/
	/* 
	 * Moves the GET pointer to the next command
	 */
	void ATHandler::moveNext(void){
		if (pending_cnt > 0){
			pending_cnt--;
			
			#ifdef AT_HANDLER_DEBUG_ENABLE
				Serial.print(F("AT HANDLER pending: "));
				Serial.println(pending_cnt);
			#endif

			while(*getptr != 0x00){
				getptr++;
				if (getptr == cmd_buffer + AT_HANDLER_RxBuffer_Size){
					getptr = cmd_buffer;
					//Serial.println(F("----------------- GET ROLL OVER 2"));
				}
			}
			
			// One more increment to skip 0x00
			getptr++;
			if (getptr == cmd_buffer + AT_HANDLER_RxBuffer_Size){
				getptr = cmd_buffer;
				//Serial.println(F("----------------- GET ROLL OVER"));
			}
			
			//Serial.print(F("GET "));
			//Serial.println(getptr - cmd_buffer);
		}
	}
/*
    ATHandler.h - Library for Handling AT Commands Communication
  
  This is a small and lighweight library to handle incoming AT COMMANDS
  responses
  
  Created by Ing. Sebastian M. Irazabal, November 27, 2016.
  www.sebasira.com.ar
  
  ----------------------------------------------------------------------
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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
			Serial.println("AT HANDLER created");
		#endif
		putptr = cmd_buffer;
		getptr = cmd_buffer;
	  
		pending_cnt = 0;
	}
	
		
	/* FEED */
	/********/
	/* 
	 * Feeds the RxBuffer with data given. It's supposed that this data came for the Serial port
	 */
	void ATHandler::feed(char newData){
		static char prevData;
		static bool cmdJustArrived;

		if (putptr == getptr && pending_cnt > 0){
			// If PUT pointer reaches GET pointer and there are pending commands to be processed then
			// it means that an overflow is occurring (there's no more room in the buffer for the
			// incomming serial data). So I won't insert the newData in the buffer
			#ifdef AT_HANDLER_DEBUG_ENABLE
				Serial.println("AT HANDLER no room for more data");
			#endif
		}else{
			
			*putptr = newData;     				// Add newData to the RxBuffer
			// Determines if an "end-of-command" : <CR> <LF> equals "\r\n"
			
			if (prevData == '\r' && newData == '\n'){
				// If "\rn" has arrived then it means a new command has just arrived. That is if there
				// hasn't just arrive a prior command, because AT COMMANS starts and end with "\r\n" and
				// if we don't put that filter we may think a command has arrived but actually it would
				// mean that a command is stating
				#ifdef AT_HANDLER_DEBUG_ENABLE
					Serial.println("AT HANDLER new <CR><LF>");
				#endif
				if (!cmdJustArrived){
					pending_cnt++;              // Increase pending command count
					#ifdef AT_HANDLER_DEBUG_ENABLE
						Serial.println("AT HANDLER new command pending");
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

				// Decreases PUT pointer (circular bgffer)
				if (putptr == cmd_buffer){
					putptr = cmd_buffer + AT_HANDLER_RxBuffer_Size - 1;
				}else{
					putptr--;
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
			}else{
				putptr++;
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
				}else{
					match = false;
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
		return(match_N(expected, AT_HANDLER_RxBuffer_Size);
	}
	
	
	
	/* MOVES NEXT */
	/**************/
	/* 
	 * Moves the GET pointer to the next command
	 */
	void ATHandler::moveNext(void){
	if (pending_cnt > 0){
		pending_cnt--;

		while(*getptr != 0x00){
			getptr++;
			if (getptr == cmd_buffer + AT_HANDLER_RxBuffer_Size){
				getptr = cmd_buffer;
			}
		}
		
		// One more increment to skip 0x00
		getptr++;
		if (getptr == cmd_buffer + AT_HANDLER_RxBuffer_Size){
			getptr = cmd_buffer;
		}
	}
}
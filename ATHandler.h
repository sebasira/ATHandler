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

#ifndef AT_HANDLER_H
	#define    AT_HANDLER_H

	/* INCLUDES */
	/************/
    #include "Arduino.h"
    

    /* CONFIGS */
    /***********/
    // Size of Rx Serial Buffer
	#define AT_HANDLER_RxBuffer_Size      5
	
	// UnCommrnt in order to have Debug Information over Serial port @9600bps
	//#define AT_HANDLER_DEBUG_ENABLE

    
    /* CLASS */
    /*********/
    class ATHandler{
      public:
        ATHandler(void);
		void begin(void);
		void feed(char newData);
        bool chkPending(void);
		bool match(char* expected);
		bool match_N(char* expected, unsigned int N);
		void moveNext(void);
		
      private:
		char cmd_buffer[AT_CMD_HANDLER_RxBuffer_Size];
		char* putptr;
		char* getptr;
		int pending_cnt;
	};
#endif

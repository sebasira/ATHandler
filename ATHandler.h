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

#ifndef AT_HANDLER_H
	#define    AT_HANDLER_H

	/* INCLUDES */
	/************/
    #include "Arduino.h"
    

    /* CONFIGS */
    /***********/
    // Size of Rx Serial Buffer
	#define AT_HANDLER_RxBuffer_Size      80
	
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

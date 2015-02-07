/*
                               _                
 ___  _   _  _ __ ___    ___  | |_  ___   _   _ 
/ __|| | | || '_ ` _ \  / _ \ | __|/ _ \ | | | |
\__ \| |_| || | | | | || (_) || |_| (_) || |_| |
|___/ \__,_||_| |_| |_| \___/  \__|\___/  \__, |
                                          |___/ 

--------------------------------------------------------------------------------
A fast C-Library for HPDL1414 or DLR1414 or DLG1414 intelligent displays
that works with just 2/3 wires though 2 shift registers or 1 GPIO extender chip,
for most Arduinos, Arduino 2 and Teensy (3 included).
++++++++++++++++++++++++++++++++++
VERSION 0.5 (06 febb 2015)
++++++++++++++++++++++++++++++++++
coded by Max MC Costa for s.u.m.o.t.o.y - sumotoy@gmail.com
note: if you want to use (even parts), inform to the author, thanks!
--------------------------------------------------------------------------------
I have already coded another similar library but it's old and has several errors. 
HPDL1414 doesn't use CE pins so the old library it's useless with those displays.
I just got several HPDL so I decided to code another library and here's.
It uses the MCP23S17 from Microchip as the old one so you can drive 6x4 chars directly,
if you need more you have to modify the setDisplayUnit function to drive a 4to15 decoder
or similar and you can get 15x4 chars with just 3 wire that can be shared with other
7 MCP gpio's thanks to HAEN features from Microchip.
--------------------------------------------------------------------------------
dependancy:
This library use my gpio_expander library for addressing GPIO
https://github.com/sumotoy/gpio_expander
You need to download and install this first!
Of course, you'll need a MCP23S17 chip from microchip. 
--------------------------------------------------------------------------------
version note:
First fully working version.
Still not in this version the clear command (for single display, really easy to do) and a couple of useful
other functions but fully usable!
Compatible with Teensy3,any 8 bit arduino (included 1284P e 640P), really probably DUE and IDE 1.0.6 and 1.5.8
with fuyll support of SPI transactions!
--------------ToDo:
Optimizations
More addressing methods
use Teensy 3 timer for scrolling
*/

#ifndef _HPDL1414_H_
#define _HPDL1414_H_

#include "Arduino.h"
#include <inttypes.h>
#include "Print.h"
#include <../SPI/SPI.h>//this chip needs SPI
#include <../gpio_expander/mcp23s17.h>

#define USESPITRANSACTIONS//uncomment will force to use the standard SPI library

#define INVERTED_ADDRESSING

#define HPDL1414_DEBUG

/*
---------------------------- WIRING -------------------------------------------------------
_valueData format:(bit7)[xx/D6/D5/D4/D3/D2/D1/D0](bit0)
display-----bit-----mcp-pin--nam-------------------------------note----------------------
MCP_DATA0 	[0]		pin 21	(data)					address char
MCP_DATA1 	[1]		pin 22	(data)					address char
MCP_DATA2 	[2]		pin 23	(data)					address char
MCP_DATA3 	[3]		pin 24	(data)					address char
MCP_DATA4 	[4]		pin 25	(data)					address char
MCP_DATA5 	[5]		pin 26	(data)					address char
MCP_DATA6 	[6]		pin 27	(data)					address char

_contrData format:(bit7)[WR6/WR5/WR4/WR3/WR2/WR1/A1/A0](bit0)
MCP_A0 		[0]		pin 1	(digit selector)		addresses the digit for single module
MCP_A1 		[1]		pin 2   (digit selector)		addresses the digit for single module
MCP_WR1 	[2]		pin 3   (display selector)		addresses display unit
MCP_WR2 	[3]		pin 4	(display selector)		addresses display unit
MCP_WR3 	[4]		pin 5	(display selector)		addresses display unit
MCP_WR4 	[5]		pin 6	(display selector)		addresses display unit
MCP_WR5 	[5]		pin 6	(display selector)		addresses display unit
MCP_WR6 	[5]		pin 6	(display selector)		addresses display unit

The other pins of the GPIO:
pin 9 -> VDD (5V)
pin 10 -> ground
pin 15	HAEN addressing A0
pin 16	HAEN addressing A1 
pin 17	HAEN addressing A2
(00100A2A1A0) If tied to ground:00100000 = 0x20)
pin 18	tie to 5V

The other pins from display:
Of course you need to connect ground and positive (refere to datasheet)
CAUTION! Do NOT invert positive or ground or you will destroy display in  1 millisecond!

*/

class HPDL1414 : public Print {
 public:
	HPDL1414(const uint8_t cs_pin,const uint8_t addr);//SPI
	
	void		begin(const uint8_t howManyDisp);
	void 		clearAll(void);
	void 		clear(uint8_t dispUnit);
	void 		clearDigit(uint8_t digit);
	void		fillDigitsFrom(uint8_t digit,char c=0x20);
	void 		fillDigitsTo(uint8_t digit,char c=0x20);
	void 		fillDigitsBetween(uint8_t start,uint8_t to,char c=0x20);
	void 		writeChar(char c,uint8_t digit,bool autoPosition);
	void 		writeChar(char c,bool autoPosition);
	void 		printString(char const* stringToDisplay,uint8_t efxAmount=0);
	void 		printNumber(long number,uint8_t efxAmount=0);
	void 		scroll(char const* testo,unsigned int speed);
	void 		blankFromTo(uint8_t from,uint8_t to);
	void 		trigBlank(bool state);
	void 		stopBlank();
	void		setCursor(uint8_t digit);
	uint8_t 	getCursor();
	void 		setAutoclean(bool autoclean);
	#if defined(HPDL1414_DEBUG)
	void 		printData(uint16_t data,uint8_t ln);//for debug
	#endif
	virtual size_t 		write(uint8_t);
	using Print::write;
 protected:
	
 private:
	mcp23s17     mygpio;//here the instance
	uint8_t 			_CS;
	uint8_t 			_ADDRS;
	uint8_t				_dispDrived;
	uint8_t				_digitPerUnit;
	uint8_t				_maxDigits;
	uint8_t				_cursorPos;
	volatile uint8_t	_contrData;
	volatile uint8_t	_valueData;
	boolean				_autoclean;
	uint8_t				_blanking;
	uint8_t				_bufferVal[9];
	uint8_t				_parkedVal[9];
	void 				updateBuffer(uint8_t pos,uint8_t val);
	void 				clearBuffers();
	void 				writeCharacter(char c, uint8_t digit, bool autoPosition,bool updateBuff=true);
	uint8_t 			_digitToUnit(uint8_t digit);
	uint8_t 			_selectDigit(uint8_t digit);
	void 				setDisplayUnit(uint8_t dispNo,boolean state);
	void 				_sideDelay(uint8_t val);
	void 				sendData(uint8_t disp);
	uint8_t 			charReplace(char c);
	void 				_scrollEngine(char const* testo,uint8_t lenght,uint8_t advance);
};

#endif
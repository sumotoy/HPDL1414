#include "HPDL1414.h"

HPDL1414::HPDL1414(const uint8_t cs_pin,const uint8_t addr){
	_CS = cs_pin;
	_ADDRS = addr;
	_digitPerUnit = 4;
	_cursorPos = 0;
	_autoclean = false;
	_blanking = 0b00000000;;
	//_valueData format:(bit7)[xx/D6/D5/D4/D3/D2/D1/D0](bit0)
	_valueData = 0b00100000;//space
	//_contrData format:(bit7)[WR6/WR5/WR4/WR3/WR2/WR1/A1/A0](bit0)
	_contrData = 0b11111100;
}

void HPDL1414::begin(const uint8_t howManyDisp){
	_dispDrived = howManyDisp;
	
	if (howManyDisp < 1) _dispDrived = 1;//at list 1!
	_maxDigits = _dispDrived*_digitPerUnit;//calculate once _maxDigits

	 mygpio.postSetup(_CS,_ADDRS);
	 mygpio.begin();
	 mygpio.gpioPinMode(OUTPUT);
	 mygpio.gpioPort(0b1111110000100000);

	 clearAll();
}

void HPDL1414::clearAll(void){

	mygpio.gpioPort(0b00100000,0b00000000);
	_sideDelay(3);
	mygpio.gpioPort(0b00100000,0b11111100);
	_sideDelay(3);;
	
	mygpio.gpioPort(0b00100000,0b00000010);
	_sideDelay(3);
	mygpio.gpioPort(0b00100000,0b11111110);
	_sideDelay(3);

	mygpio.gpioPort(0b00100000,0b00000001);
	_sideDelay(3);
	mygpio.gpioPort(0b00100000,0b11111101);
	_sideDelay(3);
	
	mygpio.gpioPort(0b00100000,0b00000011);
	_sideDelay(3);
	mygpio.gpioPort(0b00100000,0b11111111);
	_sideDelay(3);
	clearBuffers();
	_cursorPos = 0;
}

void HPDL1414::clearBuffers(){
	for (uint8_t i=0;i<=_maxDigits;i++){
		_parkedVal[i] = 0x20;
		_bufferVal[i] = 0x20;
	}
	_blanking = 0b00000000;//suppress all blanking flags
}

void HPDL1414::clear(uint8_t dispUnit){
	_valueData = 0b00100000;//space

	//sendData();
}


void HPDL1414::clearDigit(uint8_t digit){
	if (digit < _maxDigits){
		writeCharacter(0x20,digit,false,false);
		bitClear(_blanking,digit);//suppress blanking for that digit
	}
}

void HPDL1414::fillDigitsFrom(uint8_t digit,char c){
	if (digit < _maxDigits){
		for (uint8_t i=digit;i<_maxDigits;i++){
			writeCharacter(c,i,false,true);
		}
	}
}

void HPDL1414::fillDigitsTo(uint8_t digit,char c){
	if (digit < _maxDigits){
		for (uint8_t i=0;i<=digit;i++){
			writeCharacter(c,i,false,true);
		}
	}
}

void HPDL1414::fillDigitsBetween(uint8_t start,uint8_t to,char c){
	if (start < to && to < _maxDigits){
		for (uint8_t i=start;i<=to;i++){
			writeCharacter(c,i,false,true);
		}
	}
}

void HPDL1414::setCursor(uint8_t digit){
	if (digit < _maxDigits-1) _cursorPos = digit;
}

uint8_t HPDL1414::getCursor(){
	return _cursorPos;
}

void HPDL1414::setAutoclean(bool autoclean){
	_autoclean = autoclean;
}

/*
Set the correct address bit, after this you still have to send data to gpio together
value data by using
*/
void HPDL1414::setDisplayUnit(uint8_t dispNo,boolean state){
	if (dispNo > (_dispDrived - 1)) dispNo = _dispDrived - 1;
	if (state){
		_contrData |= 0b11111100;// set bit 2 to 7 as 1
		switch(dispNo){
			case 0:
			_contrData &= 0b11111011;// clear
			break;
			case 1:
			_contrData &= 0b11110111;// clear
			break;
			case 2:
			_contrData &= 0b11101111;// clear
			break;
			case 3:
			_contrData &= 0b11011111;// clear
			break;
			case 4:
			_contrData &= 0b10111111;// clear
			break;
			case 5:
			_contrData &= 0b01111111;// clear
			break;
		}
	} else {
		_contrData |= 0b11111100;// set bit 2 to 7 as 1
	}
}

uint8_t HPDL1414::_selectDigit(uint8_t digit){
	if (digit >= _maxDigits) digit = _maxDigits - 1;//3-7-etc.
#if defined(INVERTED_ADDRESSING)
	digit = (_maxDigits-1) - digit;
#endif
	uint8_t disp = _digitToUnit(digit);//witch display for the digit?
	digit = digit - (_digitPerUnit * disp);
	_contrData &= 0b11111100;//clear out bits 0 e 1
	switch (digit){
		case 1:
		_contrData |= 0b00000001; // set bit 0 as 1
		break;
		case 2:
		_contrData |= 0b00000010; // set bit 0 as 1
		break;
		case 3:
		_contrData |= 0b00000011; // set bit 0,1 as 1
		break;
	}
	return disp;
}

/*
Return the display unit that belong to the selected digit.
It return -1 if digit it's out of boundary.
*/
uint8_t HPDL1414::_digitToUnit(uint8_t digit){
	if (digit >= _maxDigits) return _dispDrived - 1;
	return digit / 4;
}

void HPDL1414::writeCharacter(char c, uint8_t digit, bool autoPosition,bool updateBuff) {
	uint8_t unit = _selectDigit(digit);//got the display interested and set his digit
	_valueData = charReplace(c);
	if (updateBuff == true) updateBuffer(digit,_valueData);
	
	sendData(unit);
	if (autoPosition) _cursorPos++;
}


void HPDL1414::writeChar(char c,uint8_t digit,bool autoPosition) {
	if (digit < _maxDigits){
		writeCharacter(c,digit,false,true);
		if (autoPosition) _cursorPos = digit;
	} 
}

//public:
void HPDL1414::writeChar(char c,bool autoPosition) {
	if (_cursorPos < _maxDigits) {
		writeCharacter(c,_cursorPos,autoPosition,true);
	}
}

inline size_t HPDL1414::write(uint8_t value) {
  writeChar(value,true);
  return 1; // assume sucess
}

void HPDL1414::updateBuffer(uint8_t pos,uint8_t val){
	if (pos < _maxDigits && val != 0x20){// 
		_bufferVal[pos] = (char)val;
	}
}

void HPDL1414::printString(char const* stringToDisplay,uint8_t efxAmount){
	uint8_t stringLenght = strlen(stringToDisplay);
	if (stringLenght == 0) { // la stringa è vuota
	} else if (stringLenght > 0 && stringLenght <= _maxDigits){//la stringa rimane dentro il display
		uint8_t cpos;
		for (cpos = 0; cpos <  stringLenght; cpos++) {
			if (_cursorPos < _maxDigits){
				#if defined(INVERTED_ADDRESSING)
				if (efxAmount > 0){
					for (uint8_t i = 0x30; i < 0x39; i++) {//0x41 to 0x59
						writeCharacter(i,_cursorPos,false,false);
						delay(efxAmount);
					}
					writeCharacter(stringToDisplay[cpos],_cursorPos,true);
				} else {
					writeCharacter(stringToDisplay[cpos],_cursorPos,true);
				}
				#else
				if (efxAmount > 0){
					for (uint8_t i = 0x30; i < 0x39; i++) {//0x41 to 0x59
						writeCharacter(i,_cursorPos,false,false);
						delay(efxAmount);
					}
					writeCharacter(stringToDisplay[stringLenght-cpos-1],_cursorPos,true);
				} else {
					writeCharacter(stringToDisplay[stringLenght-cpos-1],_cursorPos,true);
				}					
				#endif
			}
		}
		if (_autoclean) fillDigitsFrom(getCursor()+1);	
   } else { //stringa troppo lunga, usa lo scroll
		scroll(stringToDisplay,100);
   }
}

void HPDL1414::printNumber(long number,uint8_t efxAmount){
	char ascii[_maxDigits];
	if (number > 32768 || number < -32767){
		ltoa(number,ascii,10);
	} else {
		itoa(number,ascii,10);
	}
	printString(ascii,efxAmount);
}

uint8_t HPDL1414::charReplace(char c){
	if (c < 32) c = 0x20;
	if (c == 96) c = 39;
	if (c > 96 && c < 123) c = c - 32;
	if (c > 121) c = 0x20;
	return c;
}

void HPDL1414::_sideDelay(uint8_t val){
	if (val > 0){
		delayMicroseconds(val);
	}
}

void HPDL1414::_scrollEngine(char const* testo,uint8_t lenght,uint8_t advance){
	uint8_t disp;
	uint8_t tindex;
	uint8_t pos = 0;
	char tchar;
	for (disp=0;disp<_maxDigits;disp++){//display loop feed all digits at once
		tindex = disp+advance;
		if ((tindex < _maxDigits) || (tindex - _maxDigits) >= lenght){
			tchar = 0x20;//space
		} else {
			tchar = testo[tindex - _maxDigits];
		}
		#if defined(INVERTED_ADDRESSING)
		writeCharacter(tchar,pos,false,false);
		#else
		writeCharacter(tchar,(_maxDigits-1)-pos,false,false);
		#endif
		if (pos < (_maxDigits-1)){
			pos++;
		} else {
			pos = 0;
		}
	}
}

void HPDL1414::scroll(char const* testo,unsigned int speed) {
	uint8_t testolen = strlen(testo);//15
	if (testolen > 0){
		uint8_t i;
		clearAll();
		for (i=0;i<=(_maxDigits+testolen);i++){//main loop
			_scrollEngine(testo,testolen,i);
			delay(speed);
		}
		setCursor(0);
	}
} 

void HPDL1414::blankFromTo(uint8_t from,uint8_t to){
	if (from <= to && to < _maxDigits){
		for (uint8_t i=from;i<=to;i++){

			if (_bufferVal[i] != 0x20){//suitable for blanking
				bitSet(_blanking,i);//enable blanking for that digit
				//copy the char in blanking buffer
				_parkedVal[i] = _bufferVal[i];
				//now read bit 5
				if (bitRead(_parkedVal[i],5) == 1){
					bitSet(_parkedVal[i],6);
				} else {
					bitClear(_parkedVal[i],6);
				}
				//writeCharacter(_parkedVal[i], i, false,false);
			} else {
				bitClear(_blanking,i);
			}
		}
	}
}

void HPDL1414::stopBlank(){
	_blanking = 0b00000000;
}

void HPDL1414::trigBlank(bool state){
	if (_blanking != 0){
		for (uint8_t i=0;i<_maxDigits;i++){
			if (bitRead(_blanking,i) == 1){//blank activated for this one
				if (state){
					writeCharacter(_parkedVal[i], i, false,false);
				} else {
					writeCharacter(_bufferVal[i], i, false,false);
				}
			}
		}
	}
}

void HPDL1414::sendData(uint8_t disp){
	uint16_t temp;
	setDisplayUnit(disp,true);
	temp =  _valueData | (_contrData << 8);//combine 2 bytes
	mygpio.gpioPort(temp);
	_sideDelay(3);
	setDisplayUnit(disp,false);
	temp =  _valueData | (_contrData << 8);//combine 2 bytes
	mygpio.gpioPort(temp);
}

#if defined(HPDL1414_DEBUG)
void HPDL1414::printData(uint16_t data,uint8_t ln){
	Serial.println("-+-+-+-+-+-+-+-+-++-+-+-+-+-+");
  for (int i=ln-1; i>=0; i--){
    if (bitRead(data,i)==1){
      Serial.print("1");
    } 
    else {
      Serial.print("0");
    }
  }
  Serial.print(" -> 0x");
  Serial.print(data,HEX);
  Serial.print("\n-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
}
#endif
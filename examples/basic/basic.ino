#include <SPI.h>
#include <mcp23s17.h>
#include <HPDL1414.h>


HPDL1414 disp(4,0x20);

void setup(){
  Serial.begin(38400);
  disp.begin(2);
  /*
  disp.setCursor(2);
  disp.writeChar('A',true);
  delay(1000);
  disp.fillDigitsBetween(2,4,0x58);
  */
  //disp.printString("HARUCHAN",20);
  disp.scroll("HPDL1414 DISPLAY TEST, srolling....",100);
  //disp.print("HARUCHAN");
  disp.printString("HARUCHAN",20);
  delay(1000);
  disp.blankFromTo(4,7);
}
uint8_t flip = 1;
void loop(){
  disp.trigBlank(flip);
  if (flip == 1){
    flip = 0;
  } else {
    flip = 1;
  }
  delay(200);
}
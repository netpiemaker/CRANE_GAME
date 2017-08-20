#include <Arduino.h>
#include "qrcode.h"
#include "qrencode.h"

int offsetsX = 20;
int offsetsY = 36;
int screenwidth = 128;
int screenheight = 160;
bool QRDEBUG = false;
int qrsize = 2;
bool qrstatus = false;

QRcode::QRcode(){
	//this->display = display;
}

void QRcode::init(){
	if(QRDEBUG)
		Serial.println("QRcode init");

	tft.setTextColor(ST7735_WHITE);
	tft.fillScreen(ST7735_WHITE);
	tft.fillRect(0, 0, screenwidth, screenheight, ST7735_WHITE);
}

void QRcode::debug(){
	QRDEBUG = true;
}

void QRcode::setStatus(bool status){
	qrstatus = status;
}

bool QRcode::getStatus(){
	return qrstatus;
}

void QRcode::setSize(int qrsize){
	qrsize = qrsize;
}

void QRcode::render(int x, int y, int color){
  x=x*qrsize;
  y=y*qrsize;
  x=x+offsetsX;
  y=y+offsetsY;
  
  if(color==1) {
  	// for(int i=x; i<x+qrsize; i++){
  	// 	for(int j=y; j<y+qrsize; j++){
  	// 		tft.drawPixel(i, j, ST7735_BLACK);
  	// 	}
  	// }
    tft.fillRect(x, y, 2, 2, ST7735_BLACK);
  }
  else {
  	// for(int i=x; i<x+qrsize; i++){
  	// 	for(int j=y; j<y+qrsize; j++){
  	// 		tft.drawPixel(i, j, ST7735_WHITE);
  	// 	}
  	// }
    //tft.drawPixel(x, y, ST7735_WHITE);
    tft.fillRect(x, y, 2, 2, ST7735_WHITE);
  }
}

void QRcode::screenwhite(){
	tft.setTextColor(ST7735_WHITE);
  tft.fillScreen(ST7735_WHITE);
	//tft.fillRect(0, 0, 128, 160, ST7735_WHITE);
}

void QRcode::create(String message) {

  // create QR code
  message.toCharArray((char *)strinbuf,260);
  qrencode();
  screenwhite();

  if(QRDEBUG){
	Serial.println("QRcode render");
	Serial.println();
  }
  // print QR Code
  for (byte x = 0; x < WD; x+=2) {
    for (byte y = 0; y < WD; y++) {
      if ( QRBIT(x,y) &&  QRBIT((x+1),y)) {
        // black square on top of black square
        if(QRDEBUG)
			Serial.print("@");
        render(x, y, 1);
        render((x+1), y, 1);
      }
      if (!QRBIT(x,y) &&  QRBIT((x+1),y)) {
        // white square on top of black square
        if(QRDEBUG)
			Serial.print(" ");
        render(x, y, 0);
        render((x+1), y, 1);
      }
      if ( QRBIT(x,y) && !QRBIT((x+1),y)) {
        // black square on top of white square
        if(QRDEBUG)
			Serial.print("@");
        render(x, y, 1);
        render((x+1), y, 0);
      }
      if (!QRBIT(x,y) && !QRBIT((x+1),y)) {
        // white square on top of white square
        if(QRDEBUG)
			Serial.print(" ");
        render(x, y, 0);
        render((x+1), y, 0);
      }
    }
    if(QRDEBUG) Serial.println();
  }
  if(QRDEBUG) Serial.println();
  //qrstatus = false;
}

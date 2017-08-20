#include <Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

int val1 = 160;   // (default) max 160, (pickup) min 120
int val2 = 120;    // (backward) max 120, (forward) min 70
int val3 = 90;    // (down) max 170, (up) min 90
int val4 = 90;    // (left) max 150, (right) min 10

int maxval1 = 160, minval1 = 120;
int maxval2 = 120, minval2 = 70;
int maxval3 = 170, minval3 = 90;
int maxval4 = 150, minval4 = 10;
int moveA = val1-10;
int moveB = val2-10;
int moveC = val3-10;
int moveD = val4-10;
int degree = 10;
int delayMove = 10;

void servoA() {
  if(val1>maxval1) val1 = maxval1;
  else if(val1<minval1) val1 = minval1;
  servo1.write(val1);
  delay(delayMove);
}

void servoB() {
  if(val2>maxval2) val2 = maxval2;
  else if(val2<minval2) val2 = minval2;
  servo2.write(val2);
  delay(delayMove);
}

void servoC() {
  if(val3>maxval3) val3 = maxval3;
  else if(val3<minval3) val3 = minval3;
  servo3.write(val3);
  delay(delayMove);
}

void servoD() {
  if(val4>maxval4) val4 = maxval4;
  else if(val4<minval4) val4 = minval4;
  servo4.write(val4);
  delay(delayMove);
}

void resetCrane(){
  val1 = 160;
  val2 = 120;
  val3 = 90;
  val4 = 90;
  servoA();
  delay(2000);
  servoB();
  delay(2000);
  servoC();
  delay(2000);
  servoD();
  delay(2000);
  Serial.println("NEWGAME");
}

void setup() { 
  servo1.attach(11);               
  servo2.attach(10);  
  servo3.attach(9);               
  servo4.attach(6);
  
  Serial.begin(9600);
  Serial.println("Start...");
  servoA();
  servoB();
  servoC();
  servoD();
}

void loop() {
  String readString = "";
  while (Serial.available()) {
    delay(2);  //delay to allow byte to arrive in input buffer
    char c = Serial.read();
    readString += c;

    if(readString == "PICK" || readString == "UP" || readString == "DOWN" || readString == "FORWARD" || readString == "BACKWARD" || readString == "LEFT" || readString == "RIGHT" || readString == "RESET") {
      break;
    }
  }

  if(readString == "PICK") {
    Serial.println("PICKUP");
    val1 = minval1;
    servoA();
    delay(2000);
    val2 = maxval2;
    servoB();
    delay(2000);
    val3 = minval3;
    servoC();
    delay(2000);
    val4 = minval4;
    servoD();
    delay(2000);
    val1 = minval1;
    servoA();
    delay(2000);
    resetCrane();
    readString = "";
  }
  
  if(readString == "UP") {
    val2 += degree;
    servoB();
    readString = "";
  }
  
  if(readString == "DOWN") {  
    val2 -= degree;
    servoB();
    readString = "";
  }
      
  if(readString == "FORWARD") {
    val3 += degree;
    servoC();
    readString = "";
  }
      
  if(readString == "BACKWARD") {  
    val3 -= degree;
    servoC();
    readString = "";
  }
      
  if(readString == "LEFT") {  
    val4 += degree;
    servoD();
    readString = "";
  }
    
  if(readString == "RIGHT") {  
    val4 -= degree;
    servoD();
    readString = "";
  }
    
  if(readString == "RESET") {
    resetCrane();
    readString = "";
  }
}
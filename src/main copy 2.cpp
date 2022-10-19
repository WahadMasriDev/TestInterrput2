#include <Arduino.h>

#define buttonPinB  2
#define buttonPinG  3
//variables

int stop = 0;
int mode = 0;

volatile int  sec=0;

//initialises the timer 
void initTimer(){   
  TCCR1A = 0;            
  TCCR1B = 0;           
  TCNT1  = 0;            
  OCR1A  = 62500 - 1;    
  TCCR1B = _BV(WGM12) | _BV(CS12);    
  TIMSK1 = _BV(OCIE1A);  
}

// incrementer of the seconds
ISR(TIMER1_COMPA_vect)
{
  sec++;
  Serial.print(sec);
  if (stop ==1){
    sec = 0;
  }
}

//codes for each mode

void standard(){
  mode = 1;
  Serial.println("ur in Standard t");
}

void eco(){
  mode = 2;
  Serial.println("ur in eco");
}

void config(){
  mode = 3;
  Serial.println("ur in config");
  delay(3000);
  mode = 1;
}

void maint(){
  mode = 4;
  Serial.println("ur in maint");
  delay(3000);
  mode = 1;
}

//interrupts function for button G :
void condG()
{
  if (digitalRead(buttonPinG) == 0) {
    Serial.println("PRESSED GREEN "); //debug for 
    initTimer();
    stop=0; //stop the timer
    sec = 1;//start the timer at 1 
  }
//----------RELEASE BUTTON G-----------
  if (digitalRead(buttonPinG) == 1) {
    stop=1;
    if (sec >= 5){
      sec=0;
      if (mode == 2){
        Serial.println("Transfering to Standard");
        mode = 1;
      }
      else if(mode == 1 || mode == 2){
        Serial.println("Transfering to maintanance");
        mode = 4;
      }
      else if (mode == 4){
        Serial.println("Transfering to Standard");
        mode = 1;
        //previous();
      }
    } 
  }
}

// interrupt for button B
void condB()
{
  if (digitalRead(buttonPinB) == 0) {
    initTimer();
    stop=0;
    Serial.println("PRESSED BLUE ");
    sec = 1;
  }
//------------RELEASE BUTTON B-----------------
  if (digitalRead(buttonPinB) == 1) {
    stop=1;
    if (sec >= 5){
      sec=0;
      Serial.println("SUCCESS B");
      if(mode == 1){
        Serial.println("Transfering to economie");
        mode = 2;
      }
    }
  }
}

// setup
void setup() {
  Serial.begin(9600);
  pinMode(buttonPinG, INPUT);
  pinMode(buttonPinB, INPUT);

  if (digitalRead(buttonPinG) == 0){

    mode = 3;
  }
  else {

    mode = 1;
  }
  attachInterrupt(digitalPinToInterrupt(buttonPinG),condG,CHANGE);
  attachInterrupt(digitalPinToInterrupt(buttonPinB),condB,CHANGE);
}

void loop() {
  if (mode == 1) {
    standard();
  }
  else if (mode == 2) { eco(); }
  else if (mode == 3) { config(); }
  else if (mode == 4) { maint(); }
}



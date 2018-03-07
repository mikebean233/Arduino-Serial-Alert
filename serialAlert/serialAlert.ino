#include<Servo.h>

#define DO_SERVO
#define SERVO_PIN 9
#define SERVO_MIN 650
#define SERVO_MAX 2390

#define DO_LCD
#define LCD_BAUD 19200;

#ifdef DO_SERVO
  Servo myservo;
#endif

const int ODD = 0;
const int EVEN = 1;
int currentPosition = ODD;

void setup() {
  #ifdef DO_LCD
    Serial3.begin(LCD_BAUD);
    delay(500);
  
    // backlight on 
    Serial3.write(17);
    printString("Starting...");
  #endif

  #ifdef DO_SERVO
    myservo.attach(SERVO_PIN);
    delay(1000);
    setServo(0);
  #endif

  // set the servo
}

void loop() {
  
  long time = millis() / 1000;

  if(time % 2 == 0 && currentPosition != EVEN){
    #ifdef DO_LCD
      printString("Even  ");
    #endif
    
    #ifdef DO_SERVO
      setServo(0);
    #endif
    currentPosition = EVEN;
  }
  else if(time % 2 != 0 && currentPosition != ODD){
    #ifdef DO_LCD
      printString("Odd  ");
    #endif
    
    #ifdef DO_SERVO
      setServo(100);
    #endif
    currentPosition = ODD;
  }
}

#ifdef DO_SERVO

  // Accepts values from 0 to 100
  void setServo(int value)
  {
    if(value <   0) value = 0;
    if(value > 100) value = 100;

    int outValue = SERVO_MIN + (value / 100) * (SERVO_MAX - SERVO_MIN);
    appendInt(outValue);
    
    myservo.writeMicroseconds(outValue);

  }
#endif


#ifdef DO_LCD
  
  void printString(String input){
    // clear
    Serial3.write(12);
    Serial3.print(input);
   }

  void appendInt(int value){
    appendString(String(value));  
  }

   void appendString(String input){
    Serial3.print(input);
   }
#endif


//#define DO_SERVO
#define SERVO_PIN 9
#define SERVO_MIN 650
#define SERVO_MAX 2390

#define LCD_BAUD 19200
#define ALERT_DISPLAY_DURATION_MS 750

#define PC_INPUT_BAUD 9600

#ifdef DO_SERVO
  #include<Servo.h>

  Servo myservo;
#endif

byte alertValues[256];
long thisAlertDisplayStartTime = 0;
byte currentAlertNumber = 0;

int maxLevel = 0;
//                       0    1    2    3
int levelSchedule[] = {0,  25,  50,  75, 100};

void setup() {
  Serial.begin(PC_INPUT_BAUD);
  
  Serial3.begin(LCD_BAUD);
  delay(500);
  backlightOn();
  printString("Starting...");
  delay(1000);
  #ifdef DO_SERVO
    myservo.attach(SERVO_PIN);
    delay(1000);
    setServo(0);
  #endif

  thisAlertDisplayStartTime = -1;

  // Initialize array
  memset(alertValues, 0, sizeof(alertValues));
  clearLCD();
}

void serialEvent(){
  while(Serial.available() >= 2){
    byte thisAlert = Serial.read();  
    byte thisLevel = Serial.read();
    alertValues[thisAlert] = thisLevel;
  }
}

void loop() {
  long currentTime = millis();
  displayAlerts(currentTime);
  //getMaxLevel();
  //displayMaxLevel();
  //delay(250);
}

int getSimpleLevel(int level){
  int length = sizeof(levelSchedule) / sizeof(int);
  for(int i = 0; i < length; ++i){
      if(i + 1 == length)
        return i;
       else if(levelSchedule[i] <= level && level < levelSchedule[i + 1])
          return i;  
  }
  return 0;
}

void getMaxLevel(){
  maxLevel = 0;
  for(int i = 0; i < 256; ++i)
    if(alertValues[i] > maxLevel)
      maxLevel = alertValues[i];
}

void displayMaxLevel(){
  clearLCD();
  printString("Max Level: ");
  appendInt(maxLevel);
}

void displayAlerts(long currentTime){
  if((currentTime - thisAlertDisplayStartTime) > ALERT_DISPLAY_DURATION_MS){
    int startingAlertNumber = currentAlertNumber;
    backlightOn();
    
    int alertId = (currentAlertNumber + 1) % 256;
    for(; alertValues[alertId] == 0 && alertId != currentAlertNumber; alertId = (alertId + 1) % 256)
    {/*Do nothing*/}
    
    if(alertId == startingAlertNumber && alertValues[alertId] == 0){
      // We didn't find an alert to display
      backlightOff();
      clearLCD();
      thisAlertDisplayStartTime = -1;
    } else {
      // We did find an alert to display
      backlightOn();
      printAlert(alertId, alertValues[alertId]);  
      thisAlertDisplayStartTime = currentTime;
      currentAlertNumber = alertId;
    }
    
  }  
}

#ifdef DO_SERVO

  // Accepts values from 0 to 100
  void setServo(int value)
  {
    if(value <   0) value = 0;
    if(value > 100) value = 100;

    int outValue = SERVO_MIN + (value / 100) * (SERVO_MAX - SERVO_MIN);
    myservo.writeMicroseconds(outValue);
  }
#endif

void startFirstLine(){
  Serial3.write(128);  
}

void startSecondLine(){
  Serial3.write(148);
}

void clearLCD(){
  Serial3.write(12);  
  delay(5);
}
  
void backlightOn(){
  Serial3.write(17);
}

void backlightOff(){
  Serial3.write(18);
}

void printString(String input){
  clearLCD();
  Serial3.print(input);
}

void printAlert(byte alertId, byte alertValue){
  clearLCD();
  appendString("Alert: ");
  appendString(String(alertId));
  startSecondLine();
  appendString("Level: ");
  appendString(String(alertValue));
}

void appendInt(int value){
  appendString(String(value));  
}

void appendString(String input){
  Serial3.print(input);
}


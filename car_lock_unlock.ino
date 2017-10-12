#include<SoftwareSerial.h>

#define BT_LOCK_UNLOCK      3
#define BT_RESET            4
#define LD_OK_STATUS        5
#define LD_ERROR_STATUS     6
#define LD_RESET_STATUS     7
#define LD_BUSY_STATUS      8
#define LD_WAIT_STATUS      9
#define BT_RX_PIN           11
#define MF_LOCK             10
#define BT_TX_PIN           12
#define MF_UNLOCK           13

#define printError()          digitalWrite(LD_ERROR_STATUS,HIGH); \
  delay(1000); \
  digitalWrite(LD_ERROR_STATUS,LOW); \
  digitalWrite(LD_ERROR_STATUS,HIGH); \
  delay(500); \
  digitalWrite(LD_ERROR_STATUS,LOW); \
  digitalWrite(LD_WAIT_STATUS,HIGH);
#define resetOK()             digitalWrite(LD_OK_STATUS,LOW);
#define printReset()          digitalWrite(LD_RESET_STATUS,HIGH); \
  delay(2500); \ digitalWrite(LD_RESET_STATUS,LOW); \
  digitalWrite(LD_WAIT_STATUS,HIGH);
#define printOK()             digitalWrite(LD_OK_STATUS,HIGH); \
  delay(2000);\
  digitalWrite(LD_OK_STATUS,LOW);\
  digitalWrite(LD_WAIT_STATUS,HIGH);
#define printWait()           digitalWrite(LD_WAIT_STATUS,HIGH);
#define pauseWait()           digitalWrite(LD_WAIT_STATUS,LOW);
#define printBusy()           digitalWrite(LD_BUSY_STATUS,HIGH);
#define resetBusy()           digitalWrite(LD_BUSY_STATUS,LOW);
#define __LockCar()           digitalWrite(MF_LOCK,HIGH); \
  delay(2000);                \
  digitalWrite(MF_LOCK,LOW);
#define __unLockCar()         digitalWrite(MF_UNLOCK,HIGH);\
  delay(2000);                  \
  digitalWrite(MF_UNLOCK,LOW);
#ifdef DEBUG
#define DEBUG_PRINT(x)  Serial.println (x)
#else
#define DEBUG_PRINT(x)
#endif
#define SOP '<'
#define EOP '>'
#define DATA_RATE 80
#define ARRAYSIZE 2
//istanzio oggetto SoftwareSerial (il nostro futuro bluetooth)
SoftwareSerial bt            = SoftwareSerial(BT_RX_PIN, BT_TX_PIN);
bool           isLocked      = false; //i put it to false to default
String         authCodes[2] = {"LOCK-TwD-r7!_kJ8#*y_Sz3*-QK?VS", "UNLOCK-TwD-r7!_kJ8#*y_Sz3*-QK?VS"};
char           temp;
/*
void getSerialData()
{
  // Read all serial data available, as fast as possible
  while(Serial.available() > 0)
  {
    char inChar = Serial.read();
    if(inChar == SOP)
    {
       index = 0;
       inData[index] = '\0';
       started = true;
       ended = false;
    }
    else if(inChar == EOP)
    {
       ended = true;
       break;
    }
    else
    {
      if(index < 79)
      {
        inData[index] = inChar;
        index++;
        inData[index] = '\0';
      }
    }
  }

  // We are here either because all pending serial
  // data has been read OR because an end of
  // packet marker arrived. Which is it?
  if(started && ended)
  {
    // The end of packet marker arrived. Process the packet

    // Reset for the next packet
    started = false;
    ended = false;
    index = 0;
    inData[index] = '\0';
  }
}
*/
void setup() {
  // put your setup code here, to run once:


  pinMode(BT_RX_PIN, INPUT);
  pinMode(BT_TX_PIN, OUTPUT);
  // initialize the LEDS pin as an output:
  pinMode(LD_ERROR_STATUS, OUTPUT);
  pinMode(LD_OK_STATUS, OUTPUT);
  pinMode(LD_RESET_STATUS, OUTPUT);
  pinMode(LD_BUSY_STATUS, OUTPUT);
  pinMode(LD_WAIT_STATUS, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(BT_LOCK_UNLOCK, INPUT);
  pinMode(BT_RESET, INPUT);
  pinMode(MF_LOCK, OUTPUT);
  pinMode(MF_UNLOCK, OUTPUT);
  //initialize Serial
  Serial.begin(9600);

  //inizializzo comunicazione Bluetooth
  bt.begin(9600);

  //wait operations
  printWait();
}

void loop() {
  // put your main code here, to run repeatedly:


  /**
    The main app had two ways to lock and unlock your car
    - based on a button
    - based on bluetooth module
  */
  if (digitalRead(BT_LOCK_UNLOCK) == HIGH) {
    //first i need to check if car is already locked -> if yes is a wasted operation to try another lock
    DEBUG_PRINT("Button was pressed");
    if (isLocked) {
      //car is locked so i need to unlock it
      DEBUG_PRINT("Car Status: locked");
      printBusy();
      __unLockCar();
      resetBusy();
      isLocked = false;
      printOK();
    }
    else
    {
      //car is unlocked so i need to lock it
      DEBUG_PRINT("Car Status: unlocked");
      printBusy();
      __LockCar();
      resetBusy();
      isLocked = true;
      printOK();
    }
  }
  else {
    DEBUG_PRINT("Bluetooth comunucation");
    if (bt.available())
    {
      Serial.write(bt.read());
      //now i check the code
      //getSerialData();
      //str=String(inData);
      temp=bt.read();
      if (temp=='L')
      {
        Serial.println("Codice lock corretto");
        //il codice di autenticazione è un lock con dentro anche il codice giusto
        if (isLocked)
        {
          //already locked so no operation needed
          Serial.println("Already locked");
          printError();
          isLocked=true;
        } 
        else
        {
          //car is unlocked so i need to lock it
          Serial.println("Car Status: unlocked");
          printBusy();
          __LockCar();
          resetBusy();
          isLocked = true;
          printOK();
        }
      }
      else if (temp=='U')
      {
        Serial.println("Codice unlock corretto");
        if (isLocked)
        {
          //unlock the car
          DEBUG_PRINT("Car Status: locked");
          printBusy();
          __unLockCar();
          resetBusy();
          isLocked = false;
          printOK();
        }
        else
          printError();
      }
      else
      {
        printError();
      }
    }
  }
}



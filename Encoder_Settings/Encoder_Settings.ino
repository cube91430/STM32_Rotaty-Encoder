#include <SD.h>
#include <SPI.h>
#include <EEPROM.h>

const int cs_0 = PA4;

const int BUTTON_PIN = PB11;       // the number of the pushbutton pin
const int LONG_PRESS_TIME = 5000;  // 1000 milliseconds

// Variables will change:
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime = 0;
unsigned long releasedTime = 0;

//Define Encoder
#define encodeA PB13
#define encodeB PB14
#define encodeZ PB15
volatile int countA = 0;
volatile int countB = 0;

volatile int cumuCountA = 0;
volatile int cumuCountB = 0;

int Dir = 0;  //  CW = 1, Not MOving = 0, CCW = -1

//LED OUTPUT
int LED1 = PB6;
int LED2 = PA0;
int LED3 = PC15;
int LED4 = PC14;

void setup() {
  Serial.begin(9600);
  while (!Serial) {};
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  pinMode(encodeA, INPUT);
  pinMode(encodeB, INPUT);
  pinMode(encodeZ, INPUT);
  attachInterrupt(encodeA, pulseA, RISING);
  attachInterrupt(encodeB, pulseB, RISING);
  attachInterrupt(encodeZ, pulseZ, RISING);

  if (!SD.begin(cs_0)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1)
      ;
  }
  Serial.println("card initialized.");
}

void loop() {
  // read the state of the switch/button:
  currentState = digitalRead(BUTTON_PIN);

  if (lastState == HIGH && currentState == LOW)  // button is pressed
    pressedTime = millis();
  else if (lastState == LOW && currentState == HIGH) {  // button is released
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if (pressDuration > LONG_PRESS_TIME) {
      File dataFile = SD.open("Encoder.txt", FILE_WRITE);

      // if the file is available, write to it:
      if (dataFile) {
        dataFile.print(countA);  //Print Encoder Data to SD Card
        dataFile.print('/t');    //Print Encoder Data to SD Card
        dataFile.print(countB);  //Print Encoder Data to SD Card
        dataFile.print('/t');    //Print Encoder Data to SD Card
        dataFile.print(Dir);     //Print Encoder Data to SD Card
        dataFile.print('/t');    //Print Encoder Data to SD Card
        dataFile.close();
        // print to the serial port too:
        Serial.println();  //Print Encoder Data
      }
      // if the file isn't open, pop up an error:
      else {
        Serial.println("error opening datalog.txt");
      }
    } else if (pressDuration < LONG_PRESS_TIME) {
      /* ------------ Safety Command --------------------------- */
    }
  }
  // save the the last state
  lastState = currentState;
}

void checkDirection() {
  if (digitalRead(encodeB) == HIGH) {
    Dir = 1;
  } else {
    Dir = -1;
  }
}

void pulseA() {
  checkDirection();
  countA += Dir;
  cumuCountA += Dir;

  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
}

void pulseB() {
  countB += Dir;
  cumuCountB += Dir;

  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
}

void pulseZ() {
  countA = 0;
  countB = 0;
}

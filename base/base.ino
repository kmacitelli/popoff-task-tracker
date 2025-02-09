//RadioHead radio transceiver libraries
#include <SPI.h>
#include <RH_RF69.h>
#include <string.h>

//Demo flags 
//Reset interval set to 10 for testing/demo, set to 86400 for daily
#define RESET_INTERVAL_SECONDS 10
//Enable requirement of matching of remote ID to base ID
#define REQUIRE_UID_MATCH false

//IO Pins
#define LIGHT_PIN_1 5
#define LIGHT_PIN_2 6
#define LIGHT_PIN_3 7
#define LIGHT_PIN_4 8
#define LIGHT_PIN_5 9
#define BUZZER_PIN 1
#define BUTTON_PIN 0

//Radio Pins
#define RF69_FREQ 433.0
#define RFM69_CS    4
#define RFM69_INT   3
#define RFM69_RST   2
//#define RADIO_MOSI_PIN 11
//#define RADIO_MISO_PIN 12
//#define RADIO_SCLK_PIN 13
//#define RECEIVER_PIN 13

//Thresholds for analog input for counter numbers
#define COUNTER_0 73
#define COUNTER_1 244
#define COUNTER_2 65
#define COUNTER_3 396
#define COUNTER_4 60
#define COUNTER_5 95
#define COUNTER_6 113
#define COUNTER_7 137
#define COUNTER_8 82
#define COUNTER_9 176

//Unique ID of the module for use pairing to remote
#define ID_PIN A0

#define HELD_PRESSES_THRESHOLD 20
#define READOUT_PRESSES_THRESHOLD 3

int LIGHT_STATE;
unsigned long previousMillis = 0;
unsigned long interval = (1000 * RESET_INTERVAL_SECONDS);
int sensorValue = 0;
int baseUID;
int remoteUID;
uint8_t completionState = false;
bool buttonPressed = false;
int consecButtonPress = 0;
String remoteIdRec;

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

void setup() {
  //Initialize IO
  pinMode(LIGHT_PIN_1, OUTPUT);
  pinMode(LIGHT_PIN_2, OUTPUT);
  pinMode(LIGHT_PIN_3, OUTPUT);
  pinMode(LIGHT_PIN_4, OUTPUT);
  pinMode(LIGHT_PIN_5, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  initializeRadio();

  LIGHT_STATE=0;
  baseUID = readCounterVoltageAsDigit(analogRead(ID_PIN));
  Serial.begin(9600);
}

void loop() {

  //BASE BUTTON
  uint8_t buttonState = digitalRead(BUTTON_PIN);
  //Short button press will trigger audio state readout, long press will callibrate
  if (buttonState == HIGH){
    //Serial.println("is pressed");
    buttonPressed == true;
    consecButtonPress++;
  } 
  //triggered when button stops being pressed
  else if (buttonPressed == true){
    //Serial.println("Press ended");
    if (consecButtonPress > HELD_PRESSES_THRESHOLD){
      calibrate();
    }
    else{
      readout();
    }
    buttonPressed == false;
    consecButtonPress = 0;
  }

  //RADIO 
  //Check if we have a transmission available
  if (rf69.available() && LIGHT_STATE==0){
    String remoteUID = receiveTransmission();
    //Flag to easily control if module requires matching ID in signal or be more permissive
    if ((!REQUIRE_UID_MATCH) || (REQUIRE_UID_MATCH && (remoteUID == baseUID))){
      turnOnLights();
      LIGHT_STATE = 1;
      Serial.println("Received signal with ID " + String(remoteUID));
    }
  }

  //RESET LOGIC
  //if time elapsed since last reset is > some interval
  if (millis() - previousMillis > interval){
    reset();
  }
  delay(100);
}

//TODO callibratee logic
void calibrate(){
  Serial.println("Calibrating");
}

//Communicate completion state of task with beeps
void readout(){
  if (LIGHT_STATE == 0){
    //TODO beep once
    Serial.println("state is 0");
  }
  if (LIGHT_STATE == 1){
    //TODO beep twice
    Serial.println("state is 1");
  }
}

//Turn off light, reset time elapsed
void reset(){
    //Serial.println("Resetting!");
    previousMillis += interval;
    turnOffLights();
    LIGHT_STATE = 0;
}

void initializeRadio(){
  //Radio initialization 
  while (!Serial) {
    delay(1);}

  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);  

  if (!rf69.init()){
    Serial.println("RFM69 radio init failed");
    while (1);}
  Serial.println("RFM69 radio init OK!");
  if (!rf69.setFrequency(433.0)){
    Serial.println("setFrequency failed");
  }
  rf69.setTxPower(20, true);  
  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}

String receiveTransmission(){
    // Should be a message for us now
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    //Serial.println(counterUID);
    Serial.println("got a message!");

    if (rf69.recv(buf, &len)) {
      if (!len) return;
      buf[len] = 0;
      Serial.print("Received [");
      Serial.print(len);
      Serial.print("]: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf69.lastRssi(), DEC);
  } else {
    Serial.println("Receive failed");
  }
  return buf;
}

void turnOnLights(){
  digitalWrite(LIGHT_PIN_1,  HIGH);
  digitalWrite(LIGHT_PIN_2,  HIGH);
  digitalWrite(LIGHT_PIN_3, HIGH);
  digitalWrite(LIGHT_PIN_4, HIGH);
  digitalWrite(LIGHT_PIN_5, HIGH);
}

void turnOffLights(){
  digitalWrite(LIGHT_PIN_1,  LOW);
  digitalWrite(LIGHT_PIN_2, LOW);
  digitalWrite(LIGHT_PIN_3, LOW);
  digitalWrite(LIGHT_PIN_4, LOW);
  digitalWrite(LIGHT_PIN_5, LOW);
}

int avg(int num1, int num2){
  return (num1+num2)/2;
}

int readCounterVoltageAsDigit(int counterVolt){
  int digitRepresented;

  if (counterVolt<avg(COUNTER_4, COUNTER_2)){
    digitRepresented=4;
  } else if (counterVolt<avg(COUNTER_2, COUNTER_0)){
    digitRepresented=2;
  } else if (counterVolt<avg(COUNTER_0, COUNTER_8)){
    digitRepresented=0;
  } else if (counterVolt<avg(COUNTER_8, COUNTER_5)){
    digitRepresented=8;
  } else if (counterVolt<avg(COUNTER_5, COUNTER_6)){
    digitRepresented=5;
  } else if (counterVolt<avg(COUNTER_6, COUNTER_7)){
    digitRepresented=6;
  } else if (counterVolt<avg(COUNTER_7, COUNTER_9)){
    digitRepresented=7;
  } else if (counterVolt<avg(COUNTER_9, COUNTER_1)){
    digitRepresented=9;
  } else if (counterVolt<avg(COUNTER_1, COUNTER_3)){
    digitRepresented=1;
  } else {
    digitRepresented=3;
  } 

  return digitRepresented;
}




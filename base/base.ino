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
#define BUZZER_PIN A1
#define BUTTON_PIN 10

//Radio Pins
#define RF69_FREQ 433.0
#define RFM69_CS    4 //CS
#define RFM69_INT   3 //G0
#define RFM69_RST   2 //RST
//#define RADIO_MOSI_PIN 11
//#define RADIO_MISO_PIN 12
//#define RADIO_SCLK_PIN 13
//#define RECEIVER_PIN 13

//Thresholds for analog input for counter numbers
#define ID_SIZE 64
int dipSwitchVals[ID_SIZE] = {0, 21, 47, 68, 98, 117, 138, 156, 191, 207, 225, 239, 260, 272, 287, 299,
                        341, 352, 365, 375, 389, 398, 408, 417, 435, 442, 451, 458, 469, 476, 484, 490,
                        512, 518, 525, 530, 538, 543, 549, 554, 565, 570, 575, 579, 586, 590, 595, 599,
                        615, 619, 623, 626, 632, 635, 639, 642, 649, 653, 656, 659, 664, 666, 670, 672};
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
  Serial.begin(9600);
  //Initialize IO
  pinMode(LIGHT_PIN_1, OUTPUT);
  pinMode(LIGHT_PIN_2, OUTPUT);
  pinMode(LIGHT_PIN_3, OUTPUT);
  pinMode(LIGHT_PIN_4, OUTPUT);
  pinMode(LIGHT_PIN_5, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);



  LIGHT_STATE=0;
  baseUID = readCounterVoltageAsDigit(analogRead(ID_PIN));
  //Serial.println("Input: " + String(readCounterVoltageAsDigit(analogRead(ID_PIN))));

  initializeRadio();

}

void loop() {
  //BASE BUTTON
  uint8_t buttonState = digitalRead(BUTTON_PIN);
  //Short button press will trigger audio state readout, long press will callibrate

  if (buttonState == LOW){
    Serial.println("is pressed");
    buttonPressed = true;
    consecButtonPress++;
  } 
  //triggered when button stops being pressed
  else if (buttonPressed == true){
    Serial.println("Press ended");
    if (consecButtonPress > HELD_PRESSES_THRESHOLD){
      calibrate();
    }
    else{
      readout();
    }
    buttonPressed = false;
    consecButtonPress = 0;
  }

  //Serial.println("LIGHT_STATE is " + String(LIGHT_STATE));
  //RADIO 
  //Check if we have a transmission available
  if (rf69.available()){// && LIGHT_STATE==0){
    Serial.println("attempting receive");
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
  delay(200);
}

//Callibration logic
void calibrate(){
  //set reset time to half the reset interval from now
  Serial.println("Calibrating");
  previousMillis = millis() - RESET_INTERVAL_SECONDS/2.0*1000;

  //Flash lights and beep 3x
  //Keep light on or off afterwards
  if (LIGHT_STATE == 0){
    turnOnLights();
  }
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  turnOffLights();
  delay(200);
  digitalWrite(BUZZER_PIN, HIGH);
  turnOnLights();
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  turnOffLights();
  delay(200);
  digitalWrite(BUZZER_PIN, HIGH);
  turnOnLights();
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  turnOffLights();
  if (LIGHT_STATE == 1){
    turnOnLights();
  }

  
}

//Communicate completion state of task with beeps
void readout(){
  if (LIGHT_STATE == 0){
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("state is 0");
  }
  if (LIGHT_STATE == 1){
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(300);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
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

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

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

  boolean foundRange = false;
  int arrIndex = 0;

  while ((!foundRange) && (arrIndex < ID_SIZE-1)){
    //find avg current index num and next num, commpare to that
    int compareNum = avg(dipSwitchVals[arrIndex], dipSwitchVals[arrIndex+1]);

    if (counterVolt < compareNum){
      digitRepresented = arrIndex;
      foundRange = true;
    }
    arrIndex++;
  }

  if (!foundRange){
    digitRepresented = 63;
  }

  return digitRepresented;
}




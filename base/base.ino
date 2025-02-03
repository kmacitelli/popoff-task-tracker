//RadioHead radio transceiver libraries
#include <SPI.h>
#include <RH_RF69.h>

//Demo flags 
//Reset interval set to 10 for testing/demo, set to 86400 for daily
#define RESET_INTERVAL_SECONDS 10
//Enable requirement of matching of remote ID to base ID
#define REQUIRE_UID_MATCH false

//IO Pins
#define LIGHT_PIN_1 5
#define LIGHT_PIN_2 6
#define LIGHT_PIN_3 7
#define BUZZER_PIN 8
#define BUTTON_PIN 9

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

int LIGHT_STATE;
unsigned long previousMillis = 0;
unsigned long interval = (1000 * RESET_INTERVAL_SECONDS);
int sensorValue = 0;
int baseUID;
int remoteUID;
uint8_t completionState = false;

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

void setup() {
  pinMode(LIGHT_PIN_1, OUTPUT);
  pinMode(LIGHT_PIN_2, OUTPUT);
  pinMode(LIGHT_PIN_3, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  LIGHT_STATE=0;
  baseUID = readCounterVoltageAsDigit(analogRead(ID_PIN));
  Serial.begin(9600);

  initializeRadio();
}

void loop() {
  //digitalRead(RECEIVER_PIN);
  uint8_t buttonState = digitalRead(BUTTON_PIN);

  //Serial.println(counterUID);

  //TODO: Callibration button, light on until callibration interval passes
  //Check if button held down
  if (buttonState == HIGH){
    //Serial.println("is pressed");
  }
  else{
    //Serial.println("Not pressed");
  }

  //When we get signal button is pressed, light state 1, turn on light
  //TODO: receive radio signal
  if (rf69.available()){
    Serial.println("got a message!");
    receiveTransmission();
  }

  //if timer > reset interval, light state 0, turn off light, reset timer
  if (millis() - previousMillis > interval){
    //Serial.println("Resetting!");
    previousMillis += interval;
    turnOffLights();
    LIGHT_STATE = 0;
  }
  delay(200);
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

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(433.0)){
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW
  
  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}

void receiveTransmission(){
    // Should be a message for us now
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (!len) return;
      buf[len] = 0;
      Serial.print("Received [");
      Serial.print(len);
      Serial.print("]: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf69.lastRssi(), DEC);
      if (strstr((char *)buf, "Hello World")) {
        // Send a reply!
        uint8_t data[] = "And hello back to you";
        rf69.send(data, sizeof(data));
        rf69.waitPacketSent();
        Serial.println("Sent a reply");
        //Blink(LED, 40, 3); //blink LED 3 times, 40ms between blinks
      }
  } else {
    Serial.println("Receive failed");
  }

  if (LIGHT_STATE == 0)
  {
    //Flag to easily control if module requires matching ID in signal or be more permissive
    if ((!REQUIRE_UID_MATCH) || (REQUIRE_UID_MATCH && (remoteUID == baseUID))){
      turnOnLights();
      LIGHT_STATE = 1;
      Serial.println("Received signal with ID " + String(remoteUID));
    }
  }
}

void turnOnLights(){
  digitalWrite(LIGHT_PIN_1,  HIGH);
  digitalWrite(LIGHT_PIN_2,  HIGH);
  digitalWrite(LIGHT_PIN_3, HIGH);
}

void turnOffLights(){
  digitalWrite(LIGHT_PIN_1,  LOW);
  digitalWrite(LIGHT_PIN_2, LOW);
  digitalWrite(LIGHT_PIN_3, LOW);
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




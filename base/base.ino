//IO Pins
#define LIGHT_PIN_1 5
#define LIGHT_PIN_2 6
#define LIGHT_PIN_3 7
#define BUZZER_PIN 8
#define BUTTON_PIN 9

//Radio Pins
#define RADIO_RST_PIN 2
#define RADIO_IRQ_PIN 3
#define RADIO_CS_PIN 4
#define RADIO_MOSI_PIN 11
#define RADIO_MISO_PIN 12
#define RADIO_SCLK_PIN 13
#define RECEIVER_PIN 13

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

//Reset interval set to 10 for testing/demo, set to 86400 for daily
#define RESET_INTERVAL_SECONDS 10

//Unique ID of the module for use pairing to remote
#define ID_PIN A0

int LIGHT_STATE;
unsigned long previousMillis = 0;
unsigned long interval = (1000 * RESET_INTERVAL_SECONDS);
int sensorValue = 0;
int counterUID;

void setup() {
  pinMode(RECEIVER_PIN, INPUT_PULLUP);
  pinMode(LIGHT_PIN_1, OUTPUT);
  pinMode(LIGHT_PIN_2, OUTPUT);
  pinMode(LIGHT_PIN_3, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(9600);
  LIGHT_STATE=0;

  counterUID = readCounterVoltageAsDigit(analogRead(ID_PIN));
}

void loop() {
  uint8_t completionState = digitalRead(RECEIVER_PIN);
  uint8_t buttonState = digitalRead(BUTTON_PIN);

  //Serial.println(counterUID);

  //TODO: Callibration button, light on until callibration interval passes
  //Check if button held down
  if (buttonState == HIGH){
    Serial.println("is pressed");
  }
  else{
    Serial.println("Not pressed");
  }

  //When we get signal button is pressed, light state 1, turn on light
  //todo: receive radio signal
  if ((completionState == LOW) && (radioId == counterUID))
  {
    //Serial.println("Activated!");
    turnOnLights();
    LIGHT_STATE = 1;
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


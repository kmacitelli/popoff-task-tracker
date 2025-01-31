//Remote IO
#define LIGHT_PIN 5
#define BUZZER_PIN 3
#define BUTTON_PIN 1
#define ID_PIN A6

//Remote radio stuff
#define RADIO_RST_PIN 2
#define RADIO_IRQ_PIN 3
#define RADIO_CS_PIN 4
#define RADIO_MOSI_PIN 11
#define RADIO_MISO_PIN 12
#define RADIO_SCLK_PIN 13
#define TRANSMITTER_PIN 13

//Thresholds for analog input for counter numbers
#define COUNTER_0 388
#define COUNTER_1 193
#define COUNTER_2 222
#define COUNTER_3 310
#define COUNTER_4 255
#define COUNTER_5 1023
#define COUNTER_6 154
#define COUNTER_7 770
#define COUNTER_8 171
#define COUNTER_9 515

//Unique ID of the module for use pairing to base
int counterUID;

void setup() {
  //pinMode(TRANSMITTER_PIN, OUTPUT_PULLUP);
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);

  counterUID = readCounterVoltageAsDigit(analogRead(ID_PIN));
}

void loop() {
  //Read from button
  uint8_t buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == HIGH){
    //Serial.println("not pressed");
  }
  else{
    //Serial.println("is pressed");
    buttonPressed();
  }
  delay(100);
}
void buttonPressed(){
  //if button is pressed, we need to send signal to base, beep, and light up
  digitalWrite(LIGHT_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  sendButtonSignal();

  //Noise for shorter time than light
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
  delay(1400);
  digitalWrite(LIGHT_PIN, LOW);
}

//use our radio module to send a signal to base including our UID
void sendButtonSignal(){
  //todo radio here
}

//Given analog input from counter, determine digit based on the reading
int readCounterVoltageAsDigit(int counterVolt){
  int digitRepresented;

  if (counterVolt<avg(COUNTER_6, COUNTER_8)){
    digitRepresented=6;
  } else if (counterVolt<avg(COUNTER_8, COUNTER_1)){
    digitRepresented=8;
  } else if (counterVolt<avg(COUNTER_1, COUNTER_2)){
    digitRepresented=1;
  } else if (counterVolt<avg(COUNTER_2, COUNTER_4)){
    digitRepresented=2;
  } else if (counterVolt<avg(COUNTER_4, COUNTER_3)){
    digitRepresented=4;
  } else if (counterVolt<avg(COUNTER_3, COUNTER_0)){
    digitRepresented=3;
  } else if (counterVolt<avg(COUNTER_0, COUNTER_9)){
    digitRepresented=0;
  } else if (counterVolt<avg(COUNTER_9, COUNTER_7)){
    digitRepresented=9;
  } else if (counterVolt<avg(COUNTER_7, COUNTER_5)){
    digitRepresented=7;
  } else {
    digitRepresented=5;
  } 

  return digitRepresented;
}

//quick and dirty way to make ranges for counter input values
int avg(int num1, int num2){
  return (num1+num2)/2;
}

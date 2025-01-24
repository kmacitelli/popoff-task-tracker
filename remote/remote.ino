#define RADIO_RST_PIN 2
#define RADIO_IRQ_PIN 3
#define RADIO_CS_PIN 4
#define LIGHT_PIN 5
#define BUZZER_PIN 8
#define BUTTON_PIN 9

#define RADIO_MOSI_PIN 11
#define RADIO_MISO_PIN 12
#define RADIO_SCLK_PIN 13

#define TRANSMITTER_PIN 13
#define ID_PIN A6

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

int sensorValue;

void setup() {
  //pinMode(TRANSMITTER_PIN, OUTPUT_PULLUP);
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  //uint8_t buttonState = digitalRead(BUTTON_PIN);
  sensorValue = readCounterVoltageAsDigit(analogRead(ID_PIN));
  Serial.println(sensorValue);

  //if (buttonState == HIGH){
  //  Serial.println("is pressed");
  //  buttonPressed();
  //}
  //else{
  //  Serial.println("Not pressed");
  //}
  delay(500);
}
void buttonPressed(){
  //When button is pressed, blink light, beep, and signal
  //turn on light
  //beep noise
  //delay
  //send signal to base
  //turn off light and noise
  delay(100);
}

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

int avg(int num1, int num2){
  return (num1+num2)/2;
}

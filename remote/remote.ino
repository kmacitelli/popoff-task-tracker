#define RADIO_RST_PIN 2
#define RADIO_IRQ_PIN 3
#define RADIO_CS_PIN 4
#define LIGHT_PIN
#define BUZZER_PIN 8
#define BUTTON_PIN 9

#define RADIO_MOSI_PIN 11
#define RADIO_MISO_PIN 12
#define RADIO_SCLK_PIN 13

#define TRANSMITTER_PIN 13
#define ID_PIN A0

#define COUNTER_0 73
#define COUNTER_1 244
#define COUNTER_2 65
#define COUNTER_3 396
#define COUNTER_4 58
#define COUNTER_5 95
#define COUNTER_6 113
#define COUNTER_7 137
#define COUNTER_8 82
#define COUNTER_9 176


void setup() {
  pinMode(TRANSMITTER_PIN, OUTPUT_PULLUP);
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  uint8_t buttonState = digitalRead(BUTTON_PIN);
  sensorValue = analogRead(ID_PIN);
  Serial.println(readCounterVoltageAsDigit(sensorValue));

  if (buttonState == HIGH){
    Serial.println("is pressed");
    buttonPressed();
  }
  else{
    Serial.println("Not pressed");
  }
  delay(10);
}
void buttonPressed(){
  //When button is pressed, blink light, beep, and signal
  //turn on light
  //beep noise
  //delay
  //send signal to base
  //turn off light and noise
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

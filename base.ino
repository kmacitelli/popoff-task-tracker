#define RADIO_RST_PIN 2
#define RADIO_IRQ_PIN 3
#define RADIO_CS_PIN 4
#define LIGHT_PIN_1 5
#define LIGHT_PIN_2 6
#define LIGHT_PIN_3 7
#define BUZZER_PIN 8
#define BUTTON_PIN 9

#define RADIO_MOSI_PIN 11
#define RADIO_MISO_PIN 12
#define RADIO_SCLK_PIN 13

#define RESET_INTERVAL_SECONDS 10
#define RECEIVER_PIN 13

int LIGHT_STATE;
unsigned long previousMillis = 0;
unsigned long interval = (1000 * RESET_INTERVAL_SECONDS);

void setup() {
  pinMode(RECEIVER_PIN, INPUT_PULLUP);
  pinMode(LIGHT_PIN_1, OUTPUT);
  pinMode(LIGHT_PIN_2, OUTPUT);
  pinMode(LIGHT_PIN_3, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(9600);
  LIGHT_STATE=0;
}

void loop() {
  //TODO: Callibration button, light on until callibration interval passes
  uint8_t completionState = digitalRead(RECEIVER_PIN);
  uint8_t buttonState = digitalRead(BUTTON_PIN);

  //When button is pressed, light state 1, turn on light
  if (completionState == LOW)
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

  //Check if button pressed
  if (buttonState == HIGH){
    Serial.println("is pressed");
  }
  else{
    Serial.println("Not pressed");
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

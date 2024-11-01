#define BTN_PIN 6
#define LIGHT_PIN 4
#define RESET_INTERVAL_SECONDS 10

int LIGHT_STATE;
unsigned long previousMillis = 0;
unsigned long interval = (1000 * RESET_INTERVAL_SECONDS);

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(LIGHT_PIN, OUTPUT);
  Serial.begin(9600);
  LIGHT_STATE=0;
}

void loop() {
  //TODO: Callibration button, light on until callibration interval passes
  uint8_t state = digitalRead(BTN_PIN);

  //When button is pressed, light state 1, turn on light
  if (state == LOW)
  {
    //Serial.println("Activated!");
    digitalWrite(LIGHT_PIN,  HIGH);
    LIGHT_STATE = 1;
  }

  //if timer > reset interval, light state 0, turn off light, reset timer
  if (millis() - previousMillis > interval){
    //Serial.println("Resetting!");
    previousMillis += interval;
    digitalWrite(LIGHT_PIN,  LOW);
    LIGHT_STATE = 0;
  }
  delay(200);
}

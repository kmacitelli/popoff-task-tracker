/*#define BTN_PIN 6
#define TRANSMITTER_PIN 4

int prevState = 0;

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(TRANSMITTER_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  //TODO: Callibration button, light on until callibration interval passes
  uint8_t state = digitalRead(BTN_PIN);

  //When button is pressed, transmit a signal
  if (state == LOW)
  {
    //Serial.println("Activated!");
    //SEND signal here
    digitalWrite(TRANSMITTER_PIN,  HIGH);
    prevState = 1;
  }
  //When button press ends, stop transmitting signal
  else if (prevState == 1 && state == HIGH){
    prevState = 0;
  }

  delay(200);
}*/

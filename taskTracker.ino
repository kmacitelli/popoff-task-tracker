#define BTN_PIN 6
#define LIGHT_PIN 4
#define RESET_INTERVAL_SECONDS 15

void setup() {
  // put your setup code here, to run once:
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(LIGHT_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Callibration with hold down button
  //Once pressed, light on until callibration interval passes

  uint8_t state = digitalRead(BTN_PIN);

  if (state == HIGH)
  {
    Serial.println("Activated!");
    digitalWrite(LIGHT_PIN,  LOW);
  }
  else if (state == LOW)
  {
    Serial.println("Not activated.");
    digitalWrite(LIGHT_PIN,  HIGH);
  }
  delay(200);
}

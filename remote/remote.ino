//RadioHead radio libraries
#include <SPI.h>
#include <RH_RF69.h>

//Remote IO Nano
// #define LIGHT_PIN 7
// #define BUZZER_PIN 6
// #define BUTTON_PIN 5
// #define ID_PIN A1

//Remote IO MKR
#define BUTTON_PIN 5
#define LIGHT_PIN 3
#define BUZZER_PIN    1
#define ID_PIN A6

//Remote radio stuff 
#define RF69_FREQ 433.0
//Ardunio Nano
// #define RFM69_CS    10
// #define RFM69_INT   2
// #define RFM69_RST   9
// #define RADIO_MOSI_PIN 11
// #define RADIO_MISO_PIN 12
// #define RADIO_SCLK_PIN 13

//Ardunio mkr
#define RFM69_CS    6
#define RFM69_INT   7
#define RFM69_RST   11
#define RADIO_MOSI_PIN 8
#define RADIO_MISO_PIN 10
#define RADIO_SCLK_PIN 9

//Thresholds for analog input for counter numbers
#define ID_SIZE 64
int dipSwitchVals[ID_SIZE] = {0, 21, 47, 68, 98, 117, 138, 156, 191, 207, 225, 239, 260, 272, 287, 299,
                        341, 352, 365, 375, 389, 398, 408, 417, 435, 442, 451, 458, 469, 476, 484, 490,
                        512, 518, 525, 530, 538, 543, 549, 554, 565, 570, 575, 579, 586, 590, 595, 599,
                        615, 619, 623, 626, 632, 635, 639, 642, 649, 653, 656, 659, 664, 666, 670, 672};

//Unique ID of the module for use pairing to base
int counterUID;
// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);
int16_t packetnum = 0;  // packet counter, we increment per xmission

void setup() {
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);

  initRadio();

  counterUID = readCounterVoltageAsDigit(analogRead(ID_PIN));
}

void loop() {
  //Read from button
  uint8_t buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == HIGH){
    Serial.println("not pressed");
  }
  else{
    Serial.println("is pressed");
    buttonPressed();
  }
  delay(200);
}

void initRadio(){
  while (!Serial) delay(1); 
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("RFM69 TX Test!");

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }
  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW
  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}

void buttonPressed(){
  //if button is pressed, we need to send signal to base, beep, and light up
  digitalWrite(LIGHT_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  sendButtonSignal();

  //Noise for shorter time than light
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  delay(1400);
  digitalWrite(LIGHT_PIN, LOW);
}

//use our radio module to send a signal to base including our UID
void sendButtonSignal(){
  char radiopacket[20] = "Hello World #";
  itoa(packetnum++, radiopacket+13, 10);
  Serial.print("Sending "); Serial.println(radiopacket);

  // Send a message!
  rf69.send((uint8_t *)radiopacket, strlen(radiopacket));
  rf69.waitPacketSent();
}

//Given analog input from counter, determine digit based on the reading
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

//quick and dirty way to make ranges for counter input values
int avg(int num1, int num2){
  return (num1+num2)/2;
}

#define DVD // use the DVD setting on the remote
#define WIFI // use Serial connection to Wi-Fi chip

//#define DEBUG // print commands and other debug messages to the serial port

const uint8_t IR_REMOTE = 2;
const uint8_t SPEED_SENSE = 3;
const uint8_t LINE_LED = 4;
const uint8_t SPEED_L = 5;
const uint8_t SPEED_R = 6;
const uint8_t DIRECTION_L = 7;
const uint8_t DIRECTION_R = 8;

const uint8_t DATA = 11;
const uint8_t LATCH = 12;
const uint8_t CLOCK = 13;

const uint8_t LIGHT_SENSOR = A0;
const uint8_t LINE_LEFT = A1;
const uint8_t LINE_RIGHT = A2;
const uint8_t BUZZER = A3;

#include "IRLib.h"
#include "Commands.h"
#include "drive.hpp"
#include "lights.hpp"

Drive drive;
Lights lights;

IRrecv My_Receiver(IR_REMOTE);

IRdecode My_Decoder;
unsigned int Buffer[RAWBUF];

volatile unsigned long speed = 0;

void setup() {
  Serial.begin(115200);
  lights.begin();
  delay(20); while (!Serial); //delay for Leonardo
  My_Receiver.enableIRIn(); // Start the receiver
  My_Decoder.UseExtnBuf(Buffer);

  pinMode(SPEED_SENSE, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SPEED_SENSE), checkSpeed, RISING);
}

#ifdef WIFI
uint8_t serialMessage[2];
boolean messageDone = false;
#endif

void loop() {
#ifdef WIFI
  if (Serial.available() > 0) {
    uint8_t data = Serial.read();
    Serial.print("Serial data:\t0x");
    Serial.println(data & ~(1 << 7), HEX);
    if (data >> 7) {
      serialMessage[0] = data;
      if (data != 0b10000001) { // if it's not a speed message, the packet is only one byte long
        messageDone = true;
      }
    } else if (serialMessage[0] == 0b10000001) {
      serialMessage[1] = data;
      messageDone = true;
    }
  }
  if (messageDone) {
    Serial.println("Message is being processed");
    drive.checkIR(serialMessage[0] & ~(1 << 7));
    lights.checkIR(serialMessage[0] & ~(1 << 7));
    messageDone = false;
  }
#endif
  if (My_Receiver.GetResults(&My_Decoder)) {
    My_Receiver.resume();
    My_Decoder.decode();
#ifdef DEBUG
    Serial.println(My_Decoder.value, HEX);
#endif
    drive.checkIR(My_Decoder.value);
    lights.checkIR(My_Decoder.value);
    My_Decoder.value = 0;
  }
  drive.refresh();
  lights.refresh(drive.getSpeed());
  unsigned long maxSpeed = 0;
  for (int i = 0; i < 100; i++){
    if (speed > maxSpeed) {
      maxSpeed = speed;
    }
  }
  Serial.println(maxSpeed);
}

void checkSpeed() {
  static unsigned long prev = micros();
  unsigned long tmp = micros() - prev;
  if(tmp > 5000){
    speed = tmp;
  }
  prev = micros();
}

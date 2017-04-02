#define DVD
#define WIFI

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

Drive drive(DIRECTION_L, DIRECTION_R, SPEED_L, SPEED_R, LINE_LEFT, LINE_RIGHT, LINE_LED, BUZZER);
Lights lights;

IRrecv My_Receiver(IR_REMOTE);

IRdecode My_Decoder;
unsigned int Buffer[RAWBUF];

void setup()
{
  Serial.begin(115200);
  My_Receiver.enableIRIn(); // Start the receiver
  My_Decoder.UseExtnBuf(Buffer);
}

void loop() {
  if (My_Receiver.GetResults(&My_Decoder)) {
    My_Receiver.resume();
    My_Decoder.decode();
    Serial.println(My_Decoder.value, HEX);
    drive.checkIR(My_Decoder.value);
    lights.checkIR(My_Decoder.value);
    My_Decoder.value = 0;
  }
  drive.refresh();
  lights.refresh(drive.getSpeed());
}




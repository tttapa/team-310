#define DVD

#include "IRLib.h"
#include "Commands.h"
#include "drive.hpp"

const uint8_t IR_REMOTE = 2;
const uint8_t SPEED_SENSE = 3;
const uint8_t LINE_LED = 4;
const uint8_t SPEED_L = 5;
const uint8_t SPEED_R = 6;
const uint8_t DIRECTION_L = 7;
const uint8_t DIRECTION_R = 8;

Drive drive(DIRECTION_L, DIRECTION_R, SPEED_L, SPEED_R);

IRrecv My_Receiver(IR_REMOTE);

IRdecode My_Decoder;
unsigned int Buffer[RAWBUF];

void setup()
{
  Serial.begin(9600);
  delay(20); while (!Serial); //delay for Leonardo
  My_Receiver.enableIRIn(); // Start the receiver
  My_Decoder.UseExtnBuf(Buffer);
}

void loop() {
  if (My_Receiver.GetResults(&My_Decoder)) {
    My_Receiver.resume();
    My_Decoder.decode();
    Serial.println(My_Decoder.value, HEX);
  }
  drive.checkIR(My_Decoder.value);
  My_Decoder.value = 0;
}




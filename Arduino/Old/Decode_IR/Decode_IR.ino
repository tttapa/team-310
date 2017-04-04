#include "IRLib.h"

const uint8_t IR_REMOTE = 2;

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
  }
}

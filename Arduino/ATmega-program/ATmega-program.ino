#define DVD // use the DVD setting on the remote
#define WIFI // use Serial connection to Wi-Fi chip

//#define DEBUG // print commands and other debug messages to the serial port
//#define DEBUG_SPD

const uint8_t IR_REMOTE = 2;
const uint8_t SPEED_SENSE = 3;
const uint8_t LINE_LED = 4;
const uint8_t SPEED_L = 5;
const uint8_t SPEED_R = 6;
const uint8_t DIRECTION_L = 7;
const uint8_t DIRECTION_R = 8;
const uint8_t LIGHTS = 9;

const uint8_t DATA = 11;
const uint8_t LATCH = 10;
const uint8_t CLOCK = 13;

const uint8_t LIGHT_SENSOR = A0;
const uint8_t LINE_LEFT = A1;
const uint8_t LINE_RIGHT = A2;
const uint8_t BUZZER = A3;

#include "IRLib.h"
#include "Commands.h"
#include "Motors.hpp"
#include "Buzzer.h"
#include "drive.hpp"
#include "lights.hpp"

IRrecv My_Receiver(IR_REMOTE);

IRdecode My_Decoder;
unsigned int Buffer[RAWBUF];

volatile unsigned long prev = micros();
volatile unsigned long speed = 0;
unsigned long prevspeed = -1;
bool sentZeroSpeed = false;

void setup() {
  Serial.begin(115200);
  lights.begin();
  delay(20); while (!Serial); //delay for Leonardo
  My_Receiver.enableIRIn(); // Start the receiver
  My_Decoder.UseExtnBuf(Buffer);

  pinMode(SPEED_SENSE, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SPEED_SENSE), checkSpeed, RISING);

  Serial.write((1 << 7) | RESET);
  delay(1000);
}

#ifdef WIFI
uint8_t serialMessage[2];
boolean messageDone = false;
#endif

void loop() {
  checkSerial();
  if (My_Receiver.GetResults(&My_Decoder)) {
    My_Receiver.resume();
    My_Decoder.decode();
#ifdef DEBUG
    Serial.println(My_Decoder.value, HEX);
#elif defined WIFI
    // Serial.write(My_Decoder.value | (1<<7));
#endif
    drive.checkIR(My_Decoder.value);
    lights.checkIR(My_Decoder.value);
    My_Decoder.value = 0;
  }
  drive.refresh();
  lights.refresh(drive.getSpeed());
  Buzzer.refresh();


  if (speed != prevspeed) {
    sentZeroSpeed = false;
    int actualspeed = runningAverage(speed > 0 ? 1000000.0 / speed : 0);
#ifdef DEBUG_SPD
    Serial.print("Speed:\t");
    Serial.println((actualspeed * 127 / 160) & (~(1 << 7)));
#endif
#ifdef WIFI
    Serial.write(ACTUALSPEED | (1 << 7));
    Serial.write((actualspeed * 127 / 160) & (~(1 << 7)));
#endif
    prevspeed = speed;
  }
  if (micros() > prev + 60000 && !sentZeroSpeed) {
    sentZeroSpeed = true;
#ifdef DEBUG_SPD
    Serial.print("Speed:\t");
    Serial.println(0);
#endif
#ifdef WIFI
    Serial.write(ACTUALSPEED | (1 << 7));
    Serial.write(0);
#endif
  }
}

unsigned long speed_interval = 1;
unsigned long prev_speedISR_micros = 0;

void checkSpeed() {
  unsigned long tmp = micros() - prev;
  if (tmp > 6000)
    speed = tmp;
  prev = micros();
}

void checkSerial() {
#ifdef WIFI
  if (Serial.available() > 0) {
    uint8_t data = Serial.read();
#ifdef DEBUG
    Serial.print("Serial data:\t0x");
    Serial.println(data, HEX);
#endif
    if (data >> 7) {
      serialMessage[0] = data;
      if ((data & ~(1 << 7)) >= 0x06) { // if the packet is only one byte long
        messageDone = true;
      }
    } else if ((serialMessage[0] & ~(1 << 7)) <= 0x06) {
      serialMessage[1] = data;
      messageDone = true;
    }
  }
  if (messageDone) {
#ifdef DEBUG
    Serial.println("Message is being processed");
#else
    // Serial.write(serialMessage, 2);
#endif
    drive.checkIR(serialMessage[0] & ~(1 << 7), serialMessage[1]);
    lights.checkIR(serialMessage[0] & ~(1 << 7));
    messageDone = false;
  }
#endif
}

long runningAverage(int M) {
  #define LM_SIZE 8
  static int LM[LM_SIZE];      // LastMeasurements
  static byte index = 0;
  static long sum = 0;
  static byte count = 0;

  // keep sum updated to improve speed.
  sum -= LM[index];
  LM[index] = M;
  sum += LM[index];
  index++;
  index = index % LM_SIZE;
  if (count < LM_SIZE) count++;

  return sum / count;
}

// ShiftPWM settings
//#define SHIFTPWM_USE_TIMER3
const int ShiftPWM_latchPin = 9;
//#define SHIFTPWM_NOSPI
//const int ShiftPWM_dataPin = 11;
//const int ShiftPWM_clockPin = 13;
const bool ShiftPWM_invertOutputs = false;
const bool ShiftPWM_balanceLoad = false;
#include <ShiftPWM.h>
const unsigned char maxBrightness = 255;
const unsigned char pwmFrequency = 75;
const int numRegisters = 3;
const int numRGBleds = numRegisters * 8 / 3;

const uint8_t RED[] =    {0, 0, 255};
const uint8_t GREEN[] =  {0, 190, 0};
const uint8_t BLUE[] =   {200, 0, 0};
const uint8_t YELLOW[] = {0, 127, 255};
const uint8_t PURPLE[] = {200, 0, 255};
const uint8_t CYAN[] =   {200, 190, 0};
const uint8_t WHITE[] =  {200, 190, 255};
const uint8_t BLACK[] =  {0, 0, 0};

//prototypes
void printColor(uint8_t * color);
void printHex(uint8_t val);

const int lightThreshold = 600;

const unsigned long auto_timeout = 600;

class Lights {
  public:
    Lights() {
      /* nothing in the construcor */
    }
    void begin() {
      ShiftPWM.SetAmountOfRegisters(numRegisters);
      ShiftPWM.Start(pwmFrequency, maxBrightness);
      ShiftPWM.SetAll(0);
    }
    void checkIR(uint16_t cmd) {
      cmd &= 0xFF;
      switch (cmd) {
        /*case LIGHTS_ON:
          Serial.println("Lights on");
          _color = W;
          break;*/
        case LIGHTS_OFF:
          Serial.println("Lights off");
          _color = BLACK;
          _rainbow = false;
          _auto = false;
          break;
        case LIGHTS_RED:
          Serial.println("Lights red");
          _color = RED;
          _rainbow = false;
          break;
        case LIGHTS_GREEN:
          Serial.println("Lights green");
          _color = GREEN;
          _rainbow = false;
          break;
        case LIGHTS_YELLOW:
          Serial.println("Lights white");
          _color = YELLOW;
          _rainbow = false;
          break;
        case LIGHTS_BLUE:
          Serial.println("Lights blue");
          _color = BLUE;
          _rainbow = false;
          break;
        case LIGHTS_RAINBOW:
          Serial.println("Lights rainbow");
          _rainbow = true;
          break;
        case LIGHTS_AUTO:
          if (millis() > (_lastAutoCmd + auto_timeout)) {
            _auto = !_auto;
            if (_color == BLACK && _auto)
              _color = WHITE;
            Serial.println(_auto ? "Lights automatic" : "Lights manual");
            // biep(10);
            _lastAutoCmd = millis();
          }
          break;
      }
    }
    void refresh(uint8_t speed) {
      printColor(_color);
      _hueDelay = 20 / (speed + 1);
      if ( millis() > _next) {
        if (_auto) {
          uint16_t light = analogRead(LIGHT_SENSOR);
          if(light > darkThreshold) {
            ShiftPWM.SetAllRGB(WHITE[0], WHITE[1], WHITE[2]);
          } else {
            ShiftPWM.SetAllRGB(BLACK[0], BLACK[1], BLACK[2]);
          }
        } else {
          ShiftPWM.SetAllRGB(_color[0], _color[1], _color[2]);
        }
        if (_rainbow) {
          for(int led = 0; led < numRGBleds; led++) {
            ShiftPWM.SetHSV(led, (_hue+led*15)%360, 255, 255);
          }
          _hue = (_hue + 3) % 360;
          _next = millis() + _hueDelay;
        }
      }
    }

  private:
    unsigned long _next;
    unsigned long _hueDelay = 20;
    unsigned long _lastAutoCmd = 0;
    int _counter = 0;
    int _hue = 0;
    uint8_t *_color = WHITE;
    boolean _auto = true;
    boolean _rainbow = false;
};

void printColor(uint8_t * color) {
  Serial.print('#');
  printHex(color[2]);
  printHex(color[1]);
  printHex(color[0]);
  Serial.println();
}

void printHex(uint8_t val) {
  if (!(val >> 4))
    Serial.print('0');
  Serial.print(val, HEX);
}


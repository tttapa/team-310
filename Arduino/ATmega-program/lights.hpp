// ShiftPWM settings
//#define SHIFTPWM_USE_TIMER3
const int ShiftPWM_latchPin = LATCH;
//#define SHIFTPWM_NOSPI
//const int ShiftPWM_dataPin = 11;
//const int ShiftPWM_clockPin = 13;
const bool ShiftPWM_invertOutputs = false;
const bool ShiftPWM_balanceLoad = false;
#include <ShiftPWM.h>
const unsigned char maxBrightness = 255;
const unsigned char pwmFrequency = 80;
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
const int hysteresis = 60;

const unsigned long auto_timeout = 600;

class Lights {
  public:
    Lights() {
      pinMode(LIGHTS, OUTPUT);
      digitalWrite(LIGHTS, 0);
    }
    ~Lights() {
      pinMode(LIGHTS, INPUT);
      digitalWrite(LIGHTS, 0);
    }
    void begin() {
      ShiftPWM.SetAmountOfRegisters(numRegisters);
      ShiftPWM.Start(pwmFrequency, maxBrightness);
      ShiftPWM.SetAll(0);
    }
    void checkIR(uint16_t cmd) {
      cmd &= 0xFF;
      switch (cmd) {
        case LIGHTS_ON:
#ifdef DEBUG
          Serial.println("Lights on");
#elif defined WIFI
          Serial.write(LIGHTS_ON | (1 << 7));
#endif
          digitalWrite(LIGHTS, 1);
          _color = WHITE;
          _rainbow = false;
          _auto = false;
          break;
        case LIGHTS_OFF:
#ifdef DEBUG
          Serial.println("Lights off");
#elif defined WIFI
          Serial.write(LIGHTS_OFF | (1 << 7));
#endif
          digitalWrite(LIGHTS, 0);
          _color = BLACK;
          _rainbow = false;
          _auto = false;
          break;
        case LIGHTS_RED:
#ifdef DEBUG
          Serial.println("Lights red");
#elif defined WIFI
          Serial.write(LIGHTS_RED | (1 << 7));
#endif
          _color = RED;
          _rainbow = false;
          _auto = false;
          break;
        case LIGHTS_GREEN:
#ifdef DEBUG
          Serial.println("Lights green");
#elif defined WIFI
          Serial.write(LIGHTS_GREEN | (1 << 7));
#endif
          _color = GREEN;
          _rainbow = false;
          _auto = false;
          break;
        case LIGHTS_YELLOW:
#ifdef DEBUG
          Serial.println("Lights yellow");
#elif defined WIFI
          Serial.write(LIGHTS_YELLOW | (1 << 7));
#endif
          _color = YELLOW;
          _rainbow = false;
          _auto = false;
          break;
        case LIGHTS_BLUE:
#ifdef DEBUG
          Serial.println("Lights blue");
#elif defined WIFI
          Serial.write(LIGHTS_BLUE | (1 << 7));
#endif
          _color = BLUE;
          _rainbow = false;
          _auto = false;
          break;
        case LIGHTS_RAINBOW:
#ifdef DEBUG
          Serial.println("Lights rainbow");
#elif defined WIFI
          Serial.write(LIGHTS_RAINBOW | (1 << 7));
#endif
          _rainbow = true;
          _auto = false;
          break;
        case LIGHTS_AUTO:
          if (millis() > (_lastAutoCmd + auto_timeout)) {
            _auto = !_auto;
            if (_color == BLACK && _auto)
              _color = WHITE;
#ifdef DEBUG
            Serial.println(_auto ? "Lights automatic" : "Lights manual");
#elif defined WIFI
            Serial.write(LIGHTS_AUTO | (1 << 7));
#endif
            // biep(10);
            _lastAutoCmd = millis();
          }
          break;
      }
    }
    void refresh(uint8_t speed) {
      //printColor(_color);
      _hueDelay = 20 / (speed + 1);
      if ( millis() > _next) {
        if (_auto) {
          uint16_t light = analogRead(LIGHT_SENSOR);
          if (light > lightThreshold) {
            ShiftPWM.SetAllRGB(_color[0], _color[1], _color[2]);
            digitalWrite(LIGHTS, 1);
          } else if(light < lightThreshold - hysteresis) {
            ShiftPWM.SetAllRGB(BLACK[0], BLACK[1], BLACK[2]);
            digitalWrite(LIGHTS, 0);
          }
        } else {
          ShiftPWM.SetAllRGB(_color[0], _color[1], _color[2]);
        }
        if (_rainbow) {
          for (int led = 0; led < numRGBleds; led++) {
            ShiftPWM.SetHSV(led, (_hue + led * 15) % 360, 255, 255);
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
#ifdef DEBUG
  Serial.print('#');
  printHex(color[2]);
  printHex(color[1]);
  printHex(color[0]);
  Serial.println();
#endif
}

void printHex(uint8_t val) {
#ifdef DEBUG
  if (!(val >> 4))
    Serial.print('0');
  Serial.print(val, HEX);
#endif
}


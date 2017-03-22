#define R 0b001
#define G 0b010
#define B 0b100
#define Y 0b011
#define C 0b110
#define P 0b101
#define W 0b111
#define RAINBOW 0b1000

const uint8_t rainbowColors[] = {R, G, B};

const int lightThreshold = 600;

const unsigned long auto_timeout = 600;

class Lights {
  public:
    Lights() {
      pinMode(DATA, OUTPUT);
      pinMode(LATCH, OUTPUT);
      pinMode(CLOCK, OUTPUT);
      shift(8, 0);
      shift(8, 0);
      shift(8, 0);
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
          _color = 0;
          break;
        case LIGHTS_RED:
          Serial.println("Lights red");
          _color = R;
          break;
        case LIGHTS_GREEN:
          Serial.println("Lights green");
          _color = G;
          break;
        case LIGHTS_YELLOW:
          Serial.println("Lights white");
          _color = W;
          break;
        case LIGHTS_BLUE:
          Serial.println("Lights blue");
          _color = B;
          break;
        case LIGHTS_RAINBOW:
          Serial.println("Lights rainbow");
          _color = RAINBOW;
          break;
        case LIGHTS_AUTO:
          if (millis() > (_lastAutoCmd + auto_timeout)) {
            _auto = !_auto;
            if (!_color && _auto)
              _color = W;
            Serial.println(_auto ? "Lights automatic" : "Lights manual");
            // biep(10);
            _lastAutoCmd = millis();
          }
          break;
      }
    }
    void refresh(uint8_t speed) {
      _delayTime = 150 / (speed + 1);
      if ( millis() > _next) {
        if (_auto) {
          int lightLevel = analogRead(LIGHT_SENSOR);
          if ( lightLevel > lightThreshold) {
            shift(3, _counter >= ( lightLevel - lightThreshold ) * 7 / (1023 - lightThreshold) + 1 ? 0 : _color);
          } else {
            shift(3, _counter >= 1 ? 0 : _color);
          }
          _next = millis() + _delayTime;
          _counter = (_counter + 1) % 8;
        } else {
          if (_color == RAINBOW) {
            shift(3, 1 << (_counter / 8));
            _next = millis() + _delayTime;
            _counter = (_counter + 1) % 24;
          } else {
            shift(3, (_counter >> 3) ? _color : 0);
            _next = millis() + _delayTime;
            _counter = (_counter + 1) % 16;
          }
        }
      }
    }
  private:
    unsigned long _next;
    unsigned long _delayTime = 100;
    unsigned long _lastAutoCmd = 0;
    int _counter = 0;
    uint8_t _color = W;
    boolean _auto = true;
    void shift(int nb, uint8_t data) {
      digitalWrite(LATCH, 0);
      for (int i = 0; i < nb; i++)  {
        digitalWrite(DATA, (data >> i) & 1);
        digitalWrite(CLOCK, HIGH);
        digitalWrite(CLOCK, LOW);
      }
      digitalWrite(LATCH, 1);
    }
};

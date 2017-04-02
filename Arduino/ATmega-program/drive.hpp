#define TEST // gebruik knoppen ipv lijnvolgsensoren

#define GROUND 0
#define TAPE 1
#define LIGHT 0
#define DARK 1

const unsigned long remote_timeout = 200;
const unsigned long speed_timeout = 600;

const unsigned int darkThreshold = 600;

const float ROT_SPEED_FAC = 0.8;

const size_t nb_speed_levels = 3;
const uint8_t SPEED_LEVELS[nb_speed_levels] = {150, 200, 255};

class Drive {
  public:
    Drive() {
      pinMode(DIRECTION_L, OUTPUT);
      pinMode(DIRECTION_R, OUTPUT);
      pinMode(SPEED_L, OUTPUT);
      pinMode(SPEED_R, OUTPUT);
      pinMode(BUZZER, OUTPUT);
      digitalWrite(DIRECTION_L, LOW);
      digitalWrite(DIRECTION_R, LOW);
      digitalWrite(SPEED_L, LOW);
      digitalWrite(SPEED_R, LOW);
      digitalWrite(BUZZER, LOW);
    }
    ~Drive() {
      pinMode(DIRECTION_L, INPUT);
      pinMode(DIRECTION_R, INPUT);
      pinMode(SPEED_L, INPUT);
      pinMode(SPEED_R, INPUT);
      pinMode(BUZZER, INPUT);
      digitalWrite(DIRECTION_L, LOW);
      digitalWrite(DIRECTION_R, LOW);
      digitalWrite(SPEED_L, LOW);
      digitalWrite(SPEED_R, LOW);
      digitalWrite(BUZZER, LOW);
    }
    void checkIR(uint16_t cmd) {
      cmd &= 0xFF;
      switch (cmd) {
        case FORWARD:
          fwd();
          break;
        case BACKWARD:
          bwd();
          break;
        case LEFT:
          lft();
          break;
        case RIGHT:
          rgt();
          break;
        case BRAKE:
          brk();
          break;
        case SPEEDUP:
          speedup();
          break;
        case SPEEDDOWN:
          speeddown();
          break;
        case CHG_STATION:
          _auto = true;
          break;
        case MANUAL:
          _auto = false;
          _foundRight = false;
          _foundLeft = false;
          _foundLine = false;
          break;
        default:
          break;
      }
    }
    void refresh() {
      if (_auto) {
        if (_foundLine) {
          followLine();
        } else {
          findLine();
        }
      } else {
        if (millis() > (_lastDriveCmd + remote_timeout) && ! _braked) {
          brk();
        }
      }

      if (millis() > _biepEnd)
        biepOff();
#ifdef DEBUG
      if(_foundLeft)
        Serial.print("Found Left\t");
      if(_foundRight)
        Serial.print("Found Right\t");
      if(_foundLine)
        Serial.print("Found Line\t");
#endif
    }

    
    uint8_t getSpeed() {
      return _speed;
    }
  private:
    uint8_t _speed, LINE_LEFT, LINE_RIGHT, LINE_LED, BUZZER;
    unsigned long _lastDriveCmd;
    unsigned long _lastSpdUpCmd;
    unsigned long _lastSpdDwnCmd;
    unsigned long _biepEnd;
    boolean _braked = true;
    boolean _auto = false;
    boolean _foundRight = false;
    boolean _foundLeft = false;
    boolean _foundLine = false;
    
    void fwd() {
#ifdef DEBUG
      Serial.println("forward");
#endif
      digitalWrite(DIRECTION_L, LOW); // Set the direction of both motors to forward
      digitalWrite(DIRECTION_R, LOW);
      analogWrite(SPEED_L, SPEED_LEVELS[_speed]);
      analogWrite(SPEED_R, SPEED_LEVELS[_speed]);
      _lastDriveCmd = millis();
      _braked = false;
    }
    void bwd() {
#ifdef DEBUG
      Serial.println("backward");
#endif
      digitalWrite(DIRECTION_L, HIGH); // Set the direction of both motors to backward
      digitalWrite(DIRECTION_R, HIGH);
      analogWrite(SPEED_L, SPEED_LEVELS[_speed]);
      analogWrite(SPEED_R, SPEED_LEVELS[_speed]);
      _lastDriveCmd = millis();
      _braked = false;
    }
    void lft() {
#ifdef DEBUG
      Serial.println("left");
#endif
      digitalWrite(DIRECTION_L, HIGH); // Set the direction of the left motor to backward
      digitalWrite(DIRECTION_R, LOW);
      analogWrite(SPEED_L, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      analogWrite(SPEED_R, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      _lastDriveCmd = millis();
      _braked = false;
    }
    void rgt() {
#ifdef DEBUG
      Serial.println("right");
#endif
      digitalWrite(DIRECTION_L, LOW); // Set the direction of the right motor to backward
      digitalWrite(DIRECTION_R, HIGH);
      analogWrite(SPEED_L, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      analogWrite(SPEED_R, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      _lastDriveCmd = millis();
      _braked = false;
    }
    void brk() {
#ifdef DEBUG
      Serial.println("brake");
#endif
      digitalWrite(SPEED_L, 0);
      digitalWrite(SPEED_R, 0);
      _braked = true;
    }
    void lftFwd() {
#ifdef DEBUG
      Serial.println("left forward");
#endif
      digitalWrite(DIRECTION_L, LOW); // Set the direction of the both motors to forward
      digitalWrite(DIRECTION_R, LOW);
      analogWrite(SPEED_L, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      analogWrite(SPEED_R, SPEED_LEVELS[_speed]);
      _lastDriveCmd = millis();
      _braked = false;
    }
    void rgtFwd() {
#ifdef DEBUG
      Serial.println("right forward");
#endif
      digitalWrite(DIRECTION_L, LOW); // Set the direction of the both motors to forward
      digitalWrite(DIRECTION_R, LOW);
      analogWrite(SPEED_L, SPEED_LEVELS[_speed]);
      analogWrite(SPEED_R, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      _lastDriveCmd = millis();
      _braked = false;
    }
    void speedup() {
      if (millis() > (_lastSpdUpCmd + speed_timeout)) {
        if (++_speed >= nb_speed_levels) {
          _speed = nb_speed_levels - 1;
          biep(100);
        } else {
          biep(10);
        }
        _lastSpdUpCmd = millis();
      }
#ifdef DEBUG
      Serial.print("Speed:\t");
      Serial.println(_speed);
#endif
    }
    void speeddown() {
      if (millis() > (_lastSpdDwnCmd + speed_timeout)) {
        if (_speed-- == 0) {
          biep(100);
          _speed = 0;
        } else {
          biep(10);
        }
        _lastSpdDwnCmd = millis();
      }
#ifdef DEBUG
      Serial.print("Speed:\t");
      Serial.println(_speed);
#endif
    }

    void biep(unsigned long len) {
      if (BUZZER != 255) {
        digitalWrite(BUZZER, HIGH);
        _biepEnd = millis() + len;
      }
    }
    void biepOff() {
      if (BUZZER != 255) {
        digitalWrite(BUZZER, LOW);
      }
    }

    void findLine() {
      _speed = 0;
      if (_foundRight) {
      rgt();
        if (getRightColor() == GROUND) {
          fwd();
          _foundLine = true;
        }
      } else if (_foundLeft) {
        lft();
        if (getRightColor() == TAPE) {
          _foundRight = true;
        }
      } else {
        lft();
        if (getLeftColor() == TAPE) {
          _foundLeft = true;
        }
      }
    }

    void followLine() {
      if (getRightColor() == TAPE && !getLeftColor() == TAPE) {
        rgtFwd();
      } else if (getLeftColor() == TAPE && !getRightColor() == TAPE)  {
        lftFwd();
      } else if (getLeftColor() == TAPE && getRightColor() == TAPE) {
        // _charging = true;
        brk();
        digitalWrite(DIRECTION_L, LOW);
        digitalWrite(DIRECTION_R, LOW);
#ifdef DEBUG
        Serial.println("Reached charging station");
#endif
        digitalWrite(BUZZER, HIGH);
        delay(200);
        digitalWrite(BUZZER, LOW);
        delay(200);
        digitalWrite(BUZZER, HIGH);
        delay(200);
        digitalWrite(BUZZER, LOW);
        delay(200);
        digitalWrite(BUZZER, HIGH);
        delay(200);
        digitalWrite(BUZZER, LOW);
        delay(200);
        digitalWrite(BUZZER, HIGH);
        delay(600);
        digitalWrite(BUZZER, LOW);
        _auto = false;
        _foundRight = false;
        _foundLeft = false;
        _foundLine = false;
      } else {
        fwd();
      }
    }

    boolean getLeftColor() {
      if (ambientReflectionDiff(LINE_LEFT) > darkThreshold) {
        return DARK;
      } else {
        return LIGHT;
      }
    }

    boolean getRightColor() {
      if (ambientReflectionDiff(LINE_RIGHT) > darkThreshold) {
        return DARK;
      } else {
        return LIGHT;
      }
    }
    int ambientReflectionDiff(uint8_t pin) {
#ifdef TEST
      return 1023 - 1023*digitalRead(pin);
#else
      int amb = analogRead(pin);
      digitalWrite(LINE_LED, HIGH);
      delay(1);
      int refl = analogRead(pin);
      digitalWrite(LINE_LED, LOW);
      delay(1);
#endif
    }
};

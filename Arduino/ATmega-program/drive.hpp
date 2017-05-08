//#define TEST // gebruik knoppen ipv lijnvolgsensoren

#define GROUND 0
#define TAPE 1
#define LIGHT 0
#define DARK 1

const unsigned long remote_timeout = 200;
const unsigned long speed_timeout = 600;

const unsigned int darkThreshold = 750;

const float ROT_SPEED_FAC = 0.8;
const float ROT_FWD_SLOW = 0.3;
const float ROT_FWD_FAST = 1.3;

const size_t nb_speed_levels = 3;
const uint8_t SPEED_LEVELS[nb_speed_levels] = {150, 200, 255};

class Drive {
  private:
    uint8_t _speed, _movement, _prevMovement;
    unsigned long _lastDriveCmd;
    unsigned long _lastSpdUpCmd;
    unsigned long _lastSpdDwnCmd;
    unsigned long _biepEnd;
    boolean _auto = false;
    boolean _foundRight = false;
    boolean _foundLeft = false;
    boolean _foundLine = false;

  public:
    Drive() {
      pinMode(DIRECTION_L, OUTPUT);
      pinMode(DIRECTION_R, OUTPUT);
      pinMode(SPEED_L, OUTPUT);
      pinMode(SPEED_R, OUTPUT);
      pinMode(BUZZER, OUTPUT);
      pinMode(LINE_LED, OUTPUT);
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
      pinMode(LINE_LED, INPUT);
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
        case BACKWARD:
        case LEFT:
        case RIGHT:
        case BRAKE:
          _lastDriveCmd = millis();
          _movement = cmd;
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
        if (millis() > (_lastDriveCmd + remote_timeout)) {
          _movement = BRAKE;
        }
      }

      if (_movement != _prevMovement) {
        move();
        _prevMovement = _movement;
      }

      if (millis() > _biepEnd)
        biepOff();
#ifdef DEBUG
      if (_foundLeft)
        Serial.print("Found Left\t");
      if (_foundRight)
        Serial.print("Found Right\t");
      if (_foundLine)
        Serial.print("Found Line\t");
#endif
    }


    uint8_t getSpeed() {
      return _speed;
    }


  private:

    /* _________________________________MOVEMENT_________________________________ */

    void move() {
      switch (_movement) {
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
        default: // case BRAKE:
          brk();
          break;
      }
    }

    void fwd() {
#ifdef DEBUG
      Serial.println("forward");
#elif defined WIFI
      Serial.write(FORWARD | (1 << 7));
#endif
      digitalWrite(DIRECTION_L, LOW); // Set the direction of both motors to forward
      digitalWrite(DIRECTION_R, LOW);
      analogWrite(SPEED_L, SPEED_LEVELS[_speed]);
      analogWrite(SPEED_R, SPEED_LEVELS[_speed]);
    }
    void bwd() {
#ifdef DEBUG
      Serial.println("backward");
#elif defined WIFI
      Serial.write(BACKWARD | (1 << 7));
#endif
      digitalWrite(DIRECTION_L, HIGH); // Set the direction of both motors to backward
      digitalWrite(DIRECTION_R, HIGH);
      analogWrite(SPEED_L, SPEED_LEVELS[_speed]);
      analogWrite(SPEED_R, SPEED_LEVELS[_speed]);
    }
    void lft() {
#ifdef DEBUG
      Serial.println("left");
#elif defined WIFI
      Serial.write(LEFT | (1 << 7));
#endif
      digitalWrite(DIRECTION_L, HIGH); // Set the direction of the left motor to backward
      digitalWrite(DIRECTION_R, LOW);
      analogWrite(SPEED_L, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      analogWrite(SPEED_R, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
    }
    void rgt() {
#ifdef DEBUG
      Serial.println("right");
#elif defined WIFI
      Serial.write(RIGHT | (1 << 7));
#endif
      digitalWrite(DIRECTION_L, LOW); // Set the direction of the right motor to backward
      digitalWrite(DIRECTION_R, HIGH);
      analogWrite(SPEED_L, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      analogWrite(SPEED_R, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
    }
    void brk() {
#ifdef DEBUG
      Serial.println("brake");
#elif defined WIFI
      Serial.write(BRAKE | (1 << 7));
#endif
      digitalWrite(SPEED_L, 0);
      digitalWrite(SPEED_R, 0);
    }
    void lftFwd() {
#ifdef DEBUG
      Serial.println("left forward");
#endif
      digitalWrite(DIRECTION_L, LOW); // Set the direction of the both motors to forward
      digitalWrite(DIRECTION_R, LOW);
      analogWrite(SPEED_L, SPEED_LEVELS[_speed]*ROT_FWD_SLOW);
      analogWrite(SPEED_R, min(SPEED_LEVELS[_speed]*ROT_FWD_FAST, 255));
    }
    void rgtFwd() {
#ifdef DEBUG
      Serial.println("right forward");
#endif
      digitalWrite(DIRECTION_L, LOW); // Set the direction of the both motors to forward
      digitalWrite(DIRECTION_R, LOW);
      analogWrite(SPEED_L, min(SPEED_LEVELS[_speed]*ROT_FWD_FAST, 255));
      analogWrite(SPEED_R, SPEED_LEVELS[_speed]*ROT_FWD_SLOW);
    }

    /* _________________________________SPEED_________________________________ */

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
#elif defined WIFI
      Serial.write(SETSPEED | (1 << 7));
      Serial.write(_speed & ~(1 << 7));
      Serial.write(REALSPEED | (1 << 7));
      Serial.write(SPEED_LEVELS[_speed] >> 1);
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
#elif defined WIFI
      Serial.write(SETSPEED | (1 << 7));
      Serial.write(_speed & ~(1 << 7));
      Serial.write(REALSPEED | (1 << 7));
      Serial.write(SPEED_LEVELS[_speed] >> 1);
#endif
    }

    /* _________________________________BUZZER_________________________________ */

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

    /* _________________________________LINE_FOLLOWER_________________________________ */

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
      //brk();
      //while(true);
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
      if (ambientReflectionDiff(LINE_LEFT) < darkThreshold) {
        return DARK;
      } else {
        return LIGHT;
      }
    }

    boolean getRightColor() {
      if (ambientReflectionDiff(LINE_RIGHT) < darkThreshold) {
        return DARK;
      } else {
        return LIGHT;
      }
    }
    int ambientReflectionDiff(uint8_t pin) {
#ifdef TEST
      return 1023 - 1023 * digitalRead(pin);
#else
      digitalWrite(LINE_LED, LOW);
      delay(1);
      int amb = analogRead(pin);
      digitalWrite(LINE_LED, HIGH);
      delay(1);
      int refl = analogRead(pin);
//#ifdef DEBUG
      Serial.print(pin);
      Serial.print('\t');
      Serial.println(amb - refl);
//#endif
      return amb - refl;
#endif
    }
};

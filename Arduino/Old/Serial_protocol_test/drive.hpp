#define TEST
#define DEBUG

#define GROUND 0
#define TAPE 1
#define LIGHT 0
#define DARK 1

const unsigned long remote_timeout = 200;
const unsigned long speed_timeout = 600;

const unsigned int darkThreshold = 600;

const float ROT_SPEED_FAC = 0.5;

const size_t nb_speed_levels = 3;
const uint8_t SPEED_LEVELS[nb_speed_levels] = {31, 63, 255};

class Drive {
  public:
    Drive(uint8_t dirL, uint8_t dirR, uint8_t en1, uint8_t en2, uint8_t lftSns, uint8_t rgtSns, uint8_t IRfollow, uint8_t buz = 255) : _dir_L(dirL), _dir_R(dirR), _en_L(en1), _en_R(en2), _leftSens(lftSns), _rightSens(rgtSns), _IRfollow(IRfollow), _buzzer(buz) {
      pinMode(_dir_L, OUTPUT);
      pinMode(_dir_R, OUTPUT);
      pinMode(_en_L, OUTPUT);
      pinMode(_en_R, OUTPUT);
      pinMode(_buzzer, OUTPUT);
      digitalWrite(_dir_L, LOW);
      digitalWrite(_dir_R, LOW);
      digitalWrite(_en_L, LOW);
      digitalWrite(_en_R, LOW);
      digitalWrite(_buzzer, LOW);
    }
    ~Drive() {
      pinMode(_dir_L, INPUT);
      pinMode(_dir_R, INPUT);
      pinMode(_en_L, INPUT);
      pinMode(_en_R, INPUT);
      pinMode(_buzzer, INPUT);
      digitalWrite(_dir_L, LOW);
      digitalWrite(_dir_R, LOW);
      digitalWrite(_en_L, LOW);
      digitalWrite(_en_R, LOW);
      digitalWrite(_buzzer, LOW);
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
    uint8_t _dir_L, _dir_R, _en_L, _en_R, _speed, _leftSens, _rightSens, _IRfollow, _buzzer;
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
      Serial.println("forward");
      digitalWrite(_dir_L, LOW); // Set the direction of both motors to forward
      digitalWrite(_dir_R, LOW);
      analogWrite(_en_L, SPEED_LEVELS[_speed]);
      analogWrite(_en_R, SPEED_LEVELS[_speed]);
      _lastDriveCmd = millis();
      _braked = false;
    }
    void bwd() {
      Serial.println("backward");
      digitalWrite(_dir_L, HIGH); // Set the direction of both motors to backward
      digitalWrite(_dir_R, HIGH);
      analogWrite(_en_L, SPEED_LEVELS[_speed]);
      analogWrite(_en_R, SPEED_LEVELS[_speed]);
      _lastDriveCmd = millis();
      _braked = false;
    }
    void lft() {
      Serial.println("left");
      digitalWrite(_dir_L, HIGH); // Set the direction of the left motor to backward
      digitalWrite(_dir_R, LOW);
      analogWrite(_en_L, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      analogWrite(_en_R, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      _lastDriveCmd = millis();
      _braked = false;
    }
    void rgt() {
      Serial.println("right");
      digitalWrite(_dir_L, LOW); // Set the direction of the right motor to backward
      digitalWrite(_dir_R, HIGH);
      analogWrite(_en_L, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      analogWrite(_en_R, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      _lastDriveCmd = millis();
      _braked = false;
    }
    void brk() {
      Serial.println("brake");
      digitalWrite(_en_L, 0);
      digitalWrite(_en_R, 0);
      _braked = true;
    }
    void lftFwd() {
      Serial.println("left forward");
      digitalWrite(_dir_L, LOW); // Set the direction of the both motors to forward
      digitalWrite(_dir_R, LOW);
      analogWrite(_en_L, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      analogWrite(_en_R, SPEED_LEVELS[_speed]);
      _lastDriveCmd = millis();
      _braked = false;
    }
    void rgtFwd() {
      Serial.println("right forward");
      digitalWrite(_dir_L, LOW); // Set the direction of the both motors to forward
      digitalWrite(_dir_R, LOW);
      analogWrite(_en_L, SPEED_LEVELS[_speed]);
      analogWrite(_en_R, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
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
      Serial.print("Speed:\t");
      Serial.println(_speed);
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
      Serial.print("Speed:\t");
      Serial.println(_speed);
    }

    void biep(unsigned long len) {
      if (_buzzer != 255) {
        digitalWrite(_buzzer, HIGH);
        _biepEnd = millis() + len;
      }
    }
    void biepOff() {
      if (_buzzer != 255) {
        digitalWrite(_buzzer, LOW);
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
        digitalWrite(_dir_L, LOW);
        digitalWrite(_dir_R, LOW);
#ifdef DEBUG
        Serial.println("Reached charging station");
#endif
        digitalWrite(_buzzer, HIGH);
        delay(200);
        digitalWrite(_buzzer, LOW);
        delay(200);
        digitalWrite(_buzzer, HIGH);
        delay(200);
        digitalWrite(_buzzer, LOW);
        delay(200);
        digitalWrite(_buzzer, HIGH);
        delay(200);
        digitalWrite(_buzzer, LOW);
        delay(200);
        digitalWrite(_buzzer, HIGH);
        delay(600);
        digitalWrite(_buzzer, LOW);
        _auto = false;
        _foundRight = false;
        _foundLeft = false;
        _foundLine = false;
      } else {
        fwd();
      }
    }

    boolean getLeftColor() {
      if (ambientReflectionDiff(_leftSens) > darkThreshold) {
        return DARK;
      } else {
        return LIGHT;
      }
    }

    boolean getRightColor() {
      if (ambientReflectionDiff(_rightSens) > darkThreshold) {
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
      digitalWrite(_IRfollow, HIGH);
      delay(1);
      int refl = analogRead(pin);
      digitalWrite(_IRfollow, LOW);
      delay(1);
#endif
    }
};

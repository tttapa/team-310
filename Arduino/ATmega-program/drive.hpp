#ifndef DRIVE_HPP
#define DRIVE_HPP

//#define TEST // gebruik knoppen ipv lijnvolgsensoren

#define GROUND 0
#define TAPE 1
#define LIGHT 0
#define DARK 1

#include "Buzzer.h"

const unsigned long remote_timeout = 115;
const unsigned long speed_timeout = 600;

const unsigned int darkThreshold = 750;

const float ROT_SPEED_FAC = 0.8;
const float ROT_FWD_SLOW = 0.2;
const float ROT_FWD_FAST = 1.35;

const size_t nb_speed_levels = 3;

const uint8_t SPEED_LEVELS[nb_speed_levels] = {150, 200, 255};

const uint8_t LEFT_FWD_SPEED_LEVELS[nb_speed_levels] = {150, 200, 245};
const uint8_t LEFT_BWD_SPEED_LEVELS[nb_speed_levels] = {150, 200, 255};
const uint8_t RIGHT_FWD_SPEED_LEVELS[nb_speed_levels] = {150, 200, 255};
const uint8_t RIGHT_BWD_SPEED_LEVELS[nb_speed_levels] = {150, 200, 255};

class Drive {
  private:
    uint8_t _speed, _movement, _prevMovement;
    bool _lastDriveCmdWS = false;
    unsigned long _lastDriveCmdTime;
    unsigned long _lastSpdUpCmd;
    unsigned long _lastSpdDwnCmd;
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
    void checkIR(uint16_t cmd, uint8_t val = 0) {
      cmd &= 0xFF;
      switch (cmd) {
        case FORWARD:
        case BACKWARD:
        case LEFT:
        case RIGHT:
        case BRAKE:
          _lastDriveCmdTime = millis();
          _movement = cmd;
          _lastDriveCmdWS = false;
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
          noAutoPilot();
          break;

        case WS_FWD:
        case WS_BWD:
        case WS_LFT:
        case WS_RGT:
          _movement = cmd - 0x0A;
          _lastDriveCmdWS = true;
          break;
        case PWR_L:          
        case PWR_R:

          break;

        
        default:
          break;
      }
    }
    void refresh() {
      if (_auto) {
        if (reachedChargingStation()) {
          // _charging = true;
#ifdef DEBUG
          Serial.println("Reached charging station");
#endif
          Buzzer.biep(200, 200);
          Buzzer.biep(200, 200);
          Buzzer.biep(200, 200);
          Buzzer.biep(600, 200);
          noAutoPilot();
        } else if (_foundLine) {
          followLine();
        } else {
          findLine();
        }
      } else {
        if (! _lastDriveCmdWS && millis() > (_lastDriveCmdTime + remote_timeout)) {
          _movement = BRAKE;
        }
      }

      if (_movement != _prevMovement) {
        move();
        _prevMovement = _movement;
      }

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
      digitalWrite(DIRECTION_L, LOW);
      digitalWrite(DIRECTION_R, LOW);
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
          Buzzer.biep(100, 50);
        } else {
          Buzzer.biep(10, 50);
        }
        _lastSpdUpCmd = millis();
      }
#ifdef DEBUG
      Serial.print("Speed:\t");
      Serial.println(_speed);
#elif defined WIFI
      Serial.write(SETSPEED | (1 << 7));
      Serial.write(_speed & ~(1 << 7));
      Serial.write(ACTUALSPEED | (1 << 7));
      Serial.write(SPEED_LEVELS[_speed] >> 1);
#endif
    }
    void speeddown() {
      if (millis() > (_lastSpdDwnCmd + speed_timeout)) {
        if (_speed-- == 0) {
          Buzzer.biep(100, 50);
          _speed = 0;
        } else {
          Buzzer.biep(10, 50);
        }
        _lastSpdDwnCmd = millis();
      }
#ifdef DEBUG
      Serial.print("Speed:\t");
      Serial.println(_speed);
#elif defined WIFI
      Serial.write(SETSPEED | (1 << 7));
      Serial.write(_speed & ~(1 << 7));
      Serial.write(ACTUALSPEED | (1 << 7));
      Serial.write(SPEED_LEVELS[_speed] >> 1);
#endif
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

    boolean reachedChargingStation() {
      return getLeftColor() == TAPE && getRightColor() == TAPE;
    }

    void noAutoPilot() {
      _auto = false;
      _foundRight = false;
      _foundLeft = false;
      _foundLine = false;
      _movement = BRAKE;
      brk();
    }
};

#endif // DRIVE_HPP

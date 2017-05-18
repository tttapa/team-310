#ifndef DRIVE_HPP
#define DRIVE_HPP

#include "lights.hpp"
//#define TEST // gebruik knoppen ipv lijnvolgsensoren

#define GROUND 0
#define TAPE 1
#define LIGHT 0
#define DARK 1

#define SPEED_LEVELS 3 // the number of speed level settings

#include "Motors.hpp"
#include "Buzzer.h"

const unsigned long remote_timeout = 115;
const unsigned long speed_timeout = 600;

const unsigned int darkThreshold = 750;

const float ROT_SPEED_FAC = 0.8;
const float ROT_FWD_SLOW = 0.2;
const float ROT_FWD_FAST = 1.35;

const uint8_t LEFT_FWD_SPEED_LEVELS[SPEED_LEVELS] = {150, 200, 245};
const uint8_t LEFT_BWD_SPEED_LEVELS[SPEED_LEVELS] = {150, 200, 255};
const uint8_t RIGHT_FWD_SPEED_LEVELS[SPEED_LEVELS] = {150, 200, 255};
const uint8_t RIGHT_BWD_SPEED_LEVELS[SPEED_LEVELS] = {150, 200, 255};

class Drive {
  private:
    uint8_t _speed = 0, _movement, _prevMovement;
    bool _lastDriveCmdWS = false;
    unsigned long _lastDriveCmdTime;
    unsigned long _lastSpdUpCmd;
    unsigned long _lastSpdDwnCmd;
    boolean _auto = false;
    boolean _foundRight = false;
    boolean _foundLeft = false;
    boolean _foundLine = false;
    Motor L_Motor;
    Motor R_Motor;

  public:
    Drive() :
      L_Motor(SPEED_L, DIRECTION_L, &LEFT_FWD_SPEED_LEVELS, &LEFT_BWD_SPEED_LEVELS),
      R_Motor(SPEED_R, DIRECTION_R, &RIGHT_FWD_SPEED_LEVELS, &RIGHT_BWD_SPEED_LEVELS)
    {
      pinMode(LINE_LED, OUTPUT);
      L_Motor.setSpeed(_speed);
      R_Motor.setSpeed(_speed);
    }
    ~Drive() {
      delete &L_Motor;
      delete &R_Motor;
      pinMode(LINE_LED, INPUT);
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
          sendSpeed();
          break;
        case SPEEDDOWN:
          speeddown();
          sendSpeed();
          break;
        case SETSPEED:
          _speed = val % SPEED_LEVELS;
          sendSpeed();
          break;
        case CHG_STATION:
          _auto = true;
#ifdef WIFI
          Serial.write(cmd | (1 << 7));
#endif
          break;
        case PREV:
        case NEXT:
#ifdef WIFI
          Serial.write(cmd | (1 << 7));
#endif
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
          {
            float speed = min((float)abs(val - 63) / 63, 1.0f);
#ifdef DEBUG
            Serial.print("Speed L: \t");
            Serial.print(speed);
            Serial.print("raw: \t");
            Serial.println(val);
#endif
            L_Motor.setSpeed(SPEED_LEVELS - 1, speed);
            if (val > 63)
              L_Motor.forwards();
            else if (val < 63)
              L_Motor.backwards();
            else
              L_Motor.stop();
          }
          _lastDriveCmdWS = true;
          break;
        case PWR_R:
          {
            float speed = min((float)abs(val - 63) / 63, 1.0f);
#ifdef DEBUG
            Serial.print("Speed R: \t");
            Serial.print(speed);
            Serial.print("raw: \t");
            Serial.println(val);
#endif
            R_Motor.setSpeed(SPEED_LEVELS - 1, speed);
            if (val > 63)
              R_Motor.forwards();
            else if (val < 63)
              R_Motor.backwards();
            else
              R_Motor.stop();
          }
          _lastDriveCmdWS = true;
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
          lights.checkIR(LIGHTS_OFF);
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
#endif
#ifdef WIFI
      Serial.write(FORWARD | (1 << 7));
#endif
      L_Motor.setSpeed(_speed);
      R_Motor.setSpeed(_speed);
      L_Motor.forwards();
      R_Motor.forwards();
    }

    void bwd() {
#ifdef DEBUG
      Serial.println("backward");
#endif
#ifdef WIFI
      Serial.write(BACKWARD | (1 << 7));
#endif
      L_Motor.setSpeed(_speed);
      R_Motor.setSpeed(_speed);
      L_Motor.backwards();
      R_Motor.backwards();
    }

    void lft() {
#ifdef DEBUG
      Serial.println("left");
#endif
#ifdef WIFI
      Serial.write(LEFT | (1 << 7));
#endif
      L_Motor.setSpeed(_speed, ROT_SPEED_FAC);
      R_Motor.setSpeed(_speed, ROT_SPEED_FAC);
      L_Motor.backwards();
      R_Motor.forwards();
    }

    void rgt() {
#ifdef DEBUG
      Serial.println("right");
#endif
#ifdef WIFI
      Serial.write(RIGHT | (1 << 7));
#endif
      L_Motor.setSpeed(_speed, ROT_SPEED_FAC);
      R_Motor.setSpeed(_speed, ROT_SPEED_FAC);
      L_Motor.forwards();
      R_Motor.backwards();
    }
    void brk() {
#ifdef DEBUG
      Serial.println("brake");
#endif
#ifdef WIFI
      Serial.write(BRAKE | (1 << 7));
#endif
      L_Motor.stop();
      R_Motor.stop();
    }
    void lftFwd() {
#ifdef DEBUG
      Serial.println("left forward");
#endif
      L_Motor.setSpeed(_speed, ROT_FWD_SLOW);
      R_Motor.setSpeed(_speed, ROT_FWD_FAST);
      L_Motor.forwards();
      R_Motor.forwards();
    }

    void rgtFwd() {
#ifdef DEBUG
      Serial.println("right forward");
#endif
      R_Motor.setSpeed(_speed, ROT_FWD_SLOW);
      L_Motor.setSpeed(_speed, ROT_FWD_FAST);
      L_Motor.forwards();
      R_Motor.forwards();
    }

    /* _________________________________SPEED_________________________________ */

    void speedup() {
      if (millis() > (_lastSpdUpCmd + speed_timeout)) {
        if (++_speed >= SPEED_LEVELS) {
          _speed = SPEED_LEVELS - 1;
          Buzzer.biep(120, 50);
        } else {
          Buzzer.biep(10, 50);
        }
        _lastSpdUpCmd = millis();
      }
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
    }

    void sendSpeed() {
#ifdef DEBUG
      Serial.print("Speed:\t");
      Serial.println(_speed);
#endif
#if defined WIFI
      Serial.write(SETSPEED | (1 << 7));
      Serial.write(_speed & ~(1 << 7));
      //Serial.write(ACTUALSPEED | (1 << 7));
      //Serial.write(SPEED_LEVELS[_speed] >> 1);
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
#ifdef DEBUG
      Serial.print(pin);
      Serial.print('\t');
      Serial.println(amb - refl);
#endif
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
      brk();
      _movement = BRAKE;
#ifdef WIFI
      Serial.write(MANUAL | (1 << 7));
#endif
    }
};

Drive drive;

#endif // DRIVE_HPP

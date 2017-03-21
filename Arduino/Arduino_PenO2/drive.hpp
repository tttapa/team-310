#define DARK 0
#define LIGHT 1

const unsigned long remote_timeout = 200;

const float ROT_SPEED_FAC = 0.5;

const size_t nb_speed_levels = 2;
const uint8_t SPEED_LEVELS[nb_speed_levels] = {63, 255};

class Drive {
  public:
    Drive(uint8_t dirL, uint8_t dirR, uint8_t en1, uint8_t en2, uint8_t buz = 255) : _dir_L(dirL), _dir_R(dirR), _en_L(en1), _en_R(en2), _buzzer(buz) {
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
      pinMode(_buzzer, OUTPUT);
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
          if(++_speed >= nb_speed_levels) {
            _speed = nb_speed_levels - 1;
            biep(20);
          }
          break;
        case SPEEDDOWN:
          if(_speed-- == 0) {
            biep(20);
            _speed = 0;
          }
          break;
        default:
          break;
      }
      if(millis() > (_lastDriveCmd + remote_timeout)) {
        brk();
      }
      if(millis() > _biepEnd)
        biepOff();
      Serial.print("Speed:\t");
      Serial.println(_speed);
    }
  private:
    uint8_t _dir_L, _dir_R, _en_L, _en_R, _speed, _buzzer;
    unsigned long _lastDriveCmd;
    unsigned long _biepEnd;
    void fwd() {
      Serial.println("forward");
      digitalWrite(_dir_L, LOW); // Set the direction of both motors to forward
      digitalWrite(_dir_R, LOW); 
      analogWrite(_en_L, SPEED_LEVELS[_speed]);
      analogWrite(_en_R, SPEED_LEVELS[_speed]);
      _lastDriveCmd = millis();
    }
    void bwd() {
      Serial.println("backward");
      digitalWrite(_dir_L, HIGH); // Set the direction of both motors to backward
      digitalWrite(_dir_R, HIGH); 
      analogWrite(_en_L, SPEED_LEVELS[_speed]);
      analogWrite(_en_R, SPEED_LEVELS[_speed]);
      _lastDriveCmd = millis();
    }
    void lft() {
      Serial.println("left");
      digitalWrite(_dir_L, HIGH); // Set the direction of the left motor to backward
      digitalWrite(_dir_R, LOW); 
      analogWrite(_en_L, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      analogWrite(_en_R, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      _lastDriveCmd = millis();
    }
    void rgt() {
      Serial.println("left");
      digitalWrite(_dir_L, LOW); // Set the direction of the right motor to backward
      digitalWrite(_dir_R, HIGH); 
      analogWrite(_en_L, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      analogWrite(_en_R, SPEED_LEVELS[_speed]*ROT_SPEED_FAC);
      _lastDriveCmd = millis();
    }
    void brk() {
      Serial.println("brake");
      digitalWrite(_en_L, 0);
      digitalWrite(_en_R, 0);
    }

    void biep(unsigned long len) {
      if(_buzzer != 255){
        digitalWrite(_buzzer, HIGH);
        _biepEnd = millis() + len;
      }
    }
    void biepOff() {
      if(_buzzer != 255){
        digitalWrite(_buzzer, LOW);
      }
    }
/*
    void findLine() {
      _speed = 0;
      if(_foundLine) {

      } else if(_foundLeft) {
        
      } else { 
        lft();
        if(getLeftColor()) {
          _foundLeft
        }
      }
    }*/
};

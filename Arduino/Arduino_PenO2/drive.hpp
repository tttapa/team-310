#define DARK 0
#define LIGHT 1

const unsigned long remote_timeout = 200;
const unsigned long speed_timeout = 600;

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
          speedup();
          break;
        case SPEEDDOWN:
          speeddown();
          break;
        default:
          break;
      }
    }
    void refresh() {
      if(millis() > (_lastDriveCmd + remote_timeout) && ! _braked) {
        brk();
      }
      if(millis() > _biepEnd)
        biepOff();
    }
  private:
    uint8_t _dir_L, _dir_R, _en_L, _en_R, _speed, _buzzer;
    unsigned long _lastDriveCmd;
    unsigned long _lastSpdUpCmd;
    unsigned long _lastSpdDwnCmd;
    unsigned long _biepEnd;
    boolean _braked = true;
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
      Serial.println("left");
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
    void speedup() {
      if(millis() > (_lastSpdUpCmd + speed_timeout)) {
        if(++_speed >= nb_speed_levels) {
          _speed = nb_speed_levels - 1;
          biep(60);
        }
        _lastSpdUpCmd = millis();
      }
      Serial.print("Speed:\t");
      Serial.println(_speed);
    }
    void speeddown() {
      if(millis() > (_lastSpdDwnCmd + speed_timeout)) {
        if(_speed-- == 0) {
          biep(10);
          _speed = 0;
        }
        _lastSpdDwnCmd = millis();
      }
      Serial.print("Speed:\t");
      Serial.println(_speed);
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

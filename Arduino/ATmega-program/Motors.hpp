#ifndef MOTORS_H
#define MOTORS_H

#ifndef SPEED_LEVELS
#define SPEED_LEVELS 3 // the number of speed level settings
#endif

class Motor {
  public:
    Motor(int enpin, int dirpin, uint8_t (*fwspds)[SPEED_LEVELS], uint8_t (*bwspds)[SPEED_LEVELS]) :
      _enpin{enpin},
      _dirpin{dirpin},
      fwspeeds(*fwspds),
      bwspeeds(*bwspds)
    {
      pinMode(_enpin, OUTPUT);
      pinMode(_dirpin, OUTPUT);
      stop();
    }
    ~Motor() {
      stop();
      pinMode(_enpin, INPUT);
      pinMode(_dirpin, INPUT);
    }
    void setSpeed(int speedlevel, float factor = 1.0) {
      _speedlevel = speedlevel;
      _speedfac = factor;
    }
    void stop() {
      digitalWrite(_enpin, LOW);
      digitalWrite(_dirpin, LOW);
    }
    void backwards() {
      digitalWrite(_dirpin, HIGH);
      analogWrite(_enpin, min(255, bwspeeds[_speedlevel % SPEED_LEVELS]*_speedfac));
    }
    void forwards() {
      digitalWrite(_dirpin, LOW);
      analogWrite(_enpin, min(255, fwspeeds[_speedlevel % SPEED_LEVELS]*_speedfac));
    }
  private:
    int _enpin;
    int _dirpin;
    uint8_t (&fwspeeds)[SPEED_LEVELS];
    uint8_t (&bwspeeds)[SPEED_LEVELS];
    uint8_t _speedlevel = 0;
    float _speedfac = 1.0;
};

#endif // MOTORS_H

#ifndef BUZZER_H
#define BUZZER_H

class BuzzerClass {
  private:
    const static size_t _bufferLen = 4;
    unsigned int _FIFO[_bufferLen][2]; 
    unsigned long _nextOffTime = 0;
    unsigned long _nextOnTime = 0;
    int _index = 0;
    int _writeIndex = 0;
    int _biepsInQueue = 0;
    bool _bieping = false;
    void biepEn() {
      if (_biepsInQueue == 0)
        return;
      _bieping = true;
      digitalWrite(BUZZER, HIGH);
      _nextOffTime = millis() + _FIFO[_index][0];
    }
    void biepDis() {
      _bieping = false;
      digitalWrite(BUZZER, LOW);
      _nextOnTime = millis() + _FIFO[_index][1];
      _biepsInQueue--;
      _index = (_index + 1) % _bufferLen;
    }

  public:
    void refresh() {
      if(!_bieping && millis() > _nextOnTime) {
        biepEn();
      }
      if(_bieping && millis() > _nextOffTime) {
        biepDis();
      }
    }

    void biep(unsigned int on_time, unsigned int off_time=250) {
      if(_biepsInQueue >= _bufferLen)
        return;
      _FIFO[_writeIndex][0] = on_time;
      _FIFO[_writeIndex][1] = off_time;
      _biepsInQueue++;
      _writeIndex = (_writeIndex + 1) % _bufferLen;
    }
    BuzzerClass() {
      pinMode(BUZZER, OUTPUT);
    }
};
BuzzerClass Buzzer;

#endif // BUZZER_H

#include "SimpleIR.h"

SimpleIR::SimpleIR(int pin) {
  _pin = pin;
  _irState = LOW;
  _irStateLast = LOW;
  _timeLast = 0;
  _isIdle = true;
  _signalIndex = 0;
  _signalReceived = false;
}

void SimpleIR::begin() {
  pinMode(_pin, INPUT);
  _irState = digitalRead(_pin);
}

void SimpleIR::check() {
  if (_signalReceived) return; // 如果已經收到訊號還沒處理，就先暫停偵測

  _irState = digitalRead(_pin);

  if (_irState != _irStateLast) {
    long int timeNow = micros();
    long int dT = timeNow - _timeLast;

    if (dT >= durationMax && !_isIdle) {
      _isIdle = true;
      _signalIndex = 0;
    } else if (dT < durationMax && dT > durationMin) {
      _isIdle = false;
      if (_signalIndex < maxSignalLength) {
        _signalBuffer[_signalIndex++] = (_irState == HIGH ? dT : -dT);
      }
    }
    _timeLast = timeNow;
  }
  _irStateLast = _irState;

  // 判斷是否接收完畢 (簡單判斷 Buffer 長度)
  if (_signalIndex >= maxSignalLength) {
    _signalReceived = true;
  }
}

bool SimpleIR::hasSignal() {
  return _signalReceived;
}

unsigned long SimpleIR::getResult() {
  if (!_signalReceived) return 0;
  return decodeSignal();
}

void SimpleIR::resume() {
  _signalReceived = false;
  _signalIndex = 0;
  _isIdle = true;
}

unsigned long SimpleIR::decodeSignal() {
  unsigned long result = 0;
  for (int i = 3; i < 66; i += 2) { 
    int positiveDuration = abs(_signalBuffer[i - 1]);
    int negativeDuration = abs(_signalBuffer[i]);
    
    if (negativeDuration > positiveDuration * 1.5) { 
      result = (result << 1) | 1;
    } else { 
      result = (result << 1); 
    }
  }
  return result;
}

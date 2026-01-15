#ifndef SIMPLE_IR_H
#define SIMPLE_IR_H

#include <Arduino.h>

class SimpleIR {
  public:
    // 建構子：設定接收腳位
    SimpleIR(int pin);
    
    // 初始化 (在 setup 中呼叫)
    void begin();
    
    // 持續偵測訊號 (在 loop 中呼叫)
    void check();
    
    // 檢查是否有完整訊號解碼完成
    bool hasSignal();
    
    // 取得解碼後的數值
    unsigned long getResult();
    
    // 重置狀態以接收下一筆
    void resume();

  private:
    int _pin;
    int _irState;
    int _irStateLast;
    long int _timeLast;
    boolean _isIdle;
    
    static const long int durationMax = 10000;
    static const long int durationMin = 400;
    static const int maxSignalLength = 66;
    
    int _signalBuffer[66]; // 陣列大小需與 maxSignalLength 一致
    int _signalIndex;
    boolean _signalReceived;
    
    // 私有解碼函式
    unsigned long decodeSignal();
};

#endif

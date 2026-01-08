/*
 * 基礎測試 (Modular Version)
 * 使用方法：
 * 確保 SimpleIR.h 和 SimpleIR.cpp 都在同一個資料夾內。
 */

#include "SimpleIR.h" // 引入我們自己寫的函式庫

// 建立紅外線物件 (接腳為 10)
SimpleIR ir(10);

// --- 硬體接腳定義 ---
#define MOTOR_L_B_IB 2
#define MOTOR_L_B_IA 3
#define MOTOR_R_A_IB 4
#define MOTOR_R_A_IA 5

void setup() {
  Serial.begin(115200);
  
  // 初始化紅外線物件
  ir.begin();
  
  // 初始化馬達
  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  
  Serial.println("Modular IR Car Initialized.");
}

void loop() {
  // 1. 持續偵測 (這行取代了原本幾十行的 IR_rec_Check)
  ir.check();

  // 2. 如果收到訊號
  if (ir.hasSignal()) {
    unsigned long result = ir.getResult();
    
    // 3. 處理按鍵
    handleControl(result);
    
    // 4. 重置 (取代 signalReceived = false...)
    ir.resume();
  }
  
  delayMicroseconds(20);
}

// --- 控制邏輯 ---
void handleControl(unsigned long result) {
  switch (result) {
    case 16718055: // Forward
      Serial.println("Forward");
      forward(200); delay(200); brake();
      break;
      
    case 16730805: // Backward
      Serial.println("Backward");
      backward(200); delay(200); brake();
      break;
      
    case 16716015: // Turn Left
      Serial.println("Turn Left");
      turnLeft(200); delay(200); brake();
      break;
      
    case 16734885: // Turn Right
      Serial.println("Turn Right");
      turnRight(200); delay(200); brake();
      break;
      
    case 16726215: // Stop
      Serial.println("Stop");
      brake();
      break;
      
    default:
      Serial.print("Unknown key: "); Serial.println(result);
      break;
  }
}

// --- 馬達控制 ---
void forward(int v) {
  digitalWrite(MOTOR_L_B_IB, HIGH); analogWrite(MOTOR_L_B_IA, 255 - v);
  digitalWrite(MOTOR_R_A_IB, HIGH); analogWrite(MOTOR_R_A_IA, 255 - v);
}
void backward(int v) {
  digitalWrite(MOTOR_L_B_IB, LOW); analogWrite(MOTOR_L_B_IA, v);
  digitalWrite(MOTOR_R_A_IB, LOW); analogWrite(MOTOR_R_A_IA, v);
}
void turnLeft(int v) {
  digitalWrite(MOTOR_L_B_IB, LOW); digitalWrite(MOTOR_L_B_IA, LOW);
  digitalWrite(MOTOR_R_A_IB, HIGH); analogWrite(MOTOR_R_A_IA, 255 - v);
}
void turnRight(int v) {
  digitalWrite(MOTOR_L_B_IB, HIGH); analogWrite(MOTOR_L_B_IA, 255 - v);
  digitalWrite(MOTOR_R_A_IB, LOW); digitalWrite(MOTOR_R_A_IA, LOW);
}
void brake() {
  digitalWrite(MOTOR_L_B_IB, LOW); digitalWrite(MOTOR_L_B_IA, LOW);
  digitalWrite(MOTOR_R_A_IB, LOW); digitalWrite(MOTOR_R_A_IA, LOW);
}

/*
 * 模式切換概念 (Mode Switching Concept)
 * 
 * 修改說明：
 * - 改用 "SimpleIR.h" 函式庫。
 * - 簡化音效邏輯，專注於模式切換架構。
 */

#include "SimpleIR.h"

#define MOTOR_L_B_IB 2
#define MOTOR_L_B_IA 3
#define MOTOR_R_A_IB 4
#define MOTOR_R_A_IA 5
#define BUZZER_PIN 11
#define IR_RECEIVER_PIN 10

SimpleIR ir(IR_RECEIVER_PIN);

enum Mode { MANUAL, AUTO };
Mode currentMode = MANUAL;

void setup() {
  Serial.begin(115200);
  
  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  ir.begin(); // 初始化紅外線
  
  Serial.println("Mode Switch Concept (SimpleIR)");
  Serial.println("Current Mode: MANUAL");
}

void loop() {
  // 1. 持續偵測
  ir.check();
  
  // 2. 處理訊號
  if (ir.hasSignal()) {
    unsigned long key = ir.getResult();
    
    // 按鈕 EQ (切換模式)
    if (key == 4127850240) { 
      toggleMode();
    } 
    // 手動模式控制
    else if (currentMode == MANUAL) {
      handleManualControl(key);
    }
    
    // 重要：重置紅外線狀態
    ir.resume();
  }

  // 3. 自動模式行為 (持續執行)
  if (currentMode == AUTO) {
    runSimpleAutoBehavior();
  }
}

void toggleMode() {
  if (currentMode == MANUAL) {
    currentMode = AUTO;
    Serial.println("Switching to AUTO...");
    tone(BUZZER_PIN, 1000, 100); 
  } else {
    currentMode = MANUAL;
    Serial.println("Switching to MANUAL...");
    brake();
    tone(BUZZER_PIN, 2000, 100);
  }
}

void handleManualControl(unsigned long key) {
  switch (key) {
    case 16718055: // Forward
      Serial.println("Manual: Forward");
      forward(180);
      delay(200); // 這裡保留簡單的 delay 為了教學演示 "點動" 效果
      brake();
      break;
    case 16730805: // Backward
      Serial.println("Manual: Backward");
      backward(180);
      delay(200);
      brake();
      break;
  }
}

void runSimpleAutoBehavior() {
  // 使用 millis() 做簡單的時間控制
  static unsigned long lastAutoMove = 0;
  static bool isMoving = false;
  
  if (millis() - lastAutoMove > 1000) {
    lastAutoMove = millis();
    isMoving = !isMoving;
    
    if (isMoving) {
      Serial.println("Auto: Spinning...");
      spinRight(150);
    } else {
      Serial.println("Auto: Pause...");
      brake();
    }
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
void spinRight(int v) {
  digitalWrite(MOTOR_L_B_IB, HIGH); analogWrite(MOTOR_L_B_IA, 255 - v);
  digitalWrite(MOTOR_R_A_IB, LOW); analogWrite(MOTOR_R_A_IA, v);
}
void brake() {
  digitalWrite(MOTOR_L_B_IB, LOW); digitalWrite(MOTOR_L_B_IA, LOW);
  digitalWrite(MOTOR_R_A_IB, LOW); digitalWrite(MOTOR_R_A_IA, LOW);
}

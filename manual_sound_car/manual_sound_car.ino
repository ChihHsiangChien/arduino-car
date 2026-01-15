/*
 * 手動操控與音效車 (Manual Control Car with Sound)
 * 
 * 修改說明：
 * 1. 改用 "SimpleIR.h" 函式庫。
 * 2. 使用標準 tone() 函式取代手寫 newtone，減少阻塞。
 * 3. 為了讓 SimpleIR 運作順暢，盡量減少 delay() 的使用。
 */

#include "SimpleIR.h"

// --- 硬體接腳定義 ---
#define MOTOR_L_B_IB 2
#define MOTOR_L_B_IA 3
#define MOTOR_R_A_IB 4
#define MOTOR_R_A_IA 5
#define BUZZER_PIN 11
#define IR_RECEIVER_PIN 10

// 建立紅外線物件
SimpleIR ir(IR_RECEIVER_PIN);

// --- 全域變數 ---
int carSpeed = 220; 

void setup() {
  Serial.begin(115200);

  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // 初始化紅外線
  ir.begin();

  Serial.println("Smart Car Manual Mode (SimpleIR)");
  
  brake();
  playStartupSound();
}

void loop() {
  // 1. 持續偵測紅外線
  ir.check();

  // 2. 處理訊號
  if (ir.hasSignal()) {
    unsigned long key = ir.getResult();
    if (key != 0) {
      handleIrInput(key);
    }
    ir.resume();
  }
}

// --- 遙控訊號處理 ---
void handleIrInput(unsigned long key) {
  switch (key) {
    case 16738455: // Button 0
      Serial.println("button 0");
      break;

    case 16724175: // Button 1
      Serial.println("Spin Left");
      playTurnSound();
      spinLeft(carSpeed); 
      break;

    case 16718055: // Button 2
      Serial.println("Forward");
      playMoveSound();
      forward(carSpeed); 
      break;

    case 16743045: // Button 3
      Serial.println("Spin Right");
      playTurnSound();
      spinRight(carSpeed); 
      break;

    case 16716015: // Button 4
      Serial.println("Left");
      playTurnSound();
      turnLeft(carSpeed); 
      break;

    case 16726215: // Button 5 (Stop)
      Serial.println("Stop");
      playStopSound();
      brake(); 
      break;

    case 16734885: // Button 6
      Serial.println("Right");
      playTurnSound();
      turnRight(carSpeed); 
      break;

    case 16730805: // Button 8
      Serial.println("Backward");
      playBackSound();
      backward(carSpeed); 
      break;

    case 16769055: // Button - (Speed Down)
      carSpeed = max(0, carSpeed - 20);
      Serial.print("Speed: "); Serial.println(carSpeed);
      break;

    case 16754775: // Button + (Speed Up)
      carSpeed = min(255, carSpeed + 20);
      Serial.print("Speed: "); Serial.println(carSpeed);
      break;

    case 16748655: // EQ (Horn)
      Serial.println("Horn");
      playHornSound();
      break;

    case 16769565: // Play/Pause (Emergency Stop)
      Serial.println("Pause");
      brake();
      break;
      
    default:
      break;
  }
}

// --- 音效函式 (使用標準 tone) ---
void playStartupSound() {
  // 啟動音效允許使用 delay，因為還沒進入 loop
  tone(BUZZER_PIN, 523, 150); delay(150);
  tone(BUZZER_PIN, 659, 150); delay(150);
  tone(BUZZER_PIN, 784, 200); delay(200);
}

void playStopSound() {
  tone(BUZZER_PIN, 200, 150); 
}

void playMoveSound() {
  tone(BUZZER_PIN, 1500, 100); 
}

void playBackSound() {
  // 簡化為單聲，避免 delay 卡住紅外線
  tone(BUZZER_PIN, 1000, 200); 
}

void playTurnSound() {
  tone(BUZZER_PIN, 800, 100);
}

void playHornSound() {
  // 喇叭聲較長，但 tone 是非阻塞的，所以不會卡住程式
  tone(BUZZER_PIN, 300, 500); 
}

// --- 馬達控制 ---
void brake() {
  digitalWrite(MOTOR_L_B_IB, LOW); digitalWrite(MOTOR_L_B_IA, LOW);
  digitalWrite(MOTOR_R_A_IB, LOW); digitalWrite(MOTOR_R_A_IA, LOW);
}
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
void spinLeft(int v) {
  digitalWrite(MOTOR_L_B_IB, LOW); analogWrite(MOTOR_L_B_IA, v);
  digitalWrite(MOTOR_R_A_IB, HIGH); analogWrite(MOTOR_R_A_IA, 255 - v);
}
void spinRight(int v) {
  digitalWrite(MOTOR_L_B_IB, HIGH); analogWrite(MOTOR_L_B_IA, 255 - v);
  digitalWrite(MOTOR_R_A_IB, LOW); analogWrite(MOTOR_R_A_IA, v);
}

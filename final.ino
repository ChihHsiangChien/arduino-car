// 引入紅外線遙控函式庫
#include <IRremote.hpp>

// --- 硬體接腳定義 (Pin Definitions) ---
#define MOTOR_L_B_IB 2    // 左輪馬達控制腳位 B-IB
#define MOTOR_L_B_IA 3    // 左輪馬達控制腳位 B-IA (PWM)
#define MOTOR_R_A_IB 4    // 右輪馬達控制腳位 A-IB
#define MOTOR_R_A_IA 5    // 右輪馬達控制腳位 A-IA (PWM)
#define IR_AVOID_L 6      // 左側紅外線避障感測器
#define IR_AVOID_R 7      // 右側紅外線避障感測器
#define ULTRASONIC_TRIG 8 // 超音波感測器 Trig
#define ULTRASONIC_ECHO 9 // 超音波感測器 Echo
#define IR_RECEIVER_PIN 10// 紅外線遙控接收器
#define BUZZER_PIN 11     // 警報蜂鳴器

// --- 全域變數與常數設定 ---
const int DEFAULT_SPEED = 255; // 建議將預設速度從255調低，避免馬達瞬間滿載啟動

enum Mode { MANUAL, AUTO };
Mode controlMode = MANUAL;

// 動作類型
enum ActionType { 
  ACTION_NONE, 
  ACTION_AVOID,         // 超音波前方避障
  ACTION_AUTO_SPIN_L,   // 紅外線右側避障 (向左旋轉)
  ACTION_AUTO_SPIN_R    // 紅外線左側避障 (向右旋轉)
};
ActionType currentAction = ACTION_NONE; // 記錄當前執行的動作類型
bool isActionRunning = false;           // 標記是否有非同步動作正在執行
bool isAutoModePaused = false;          // 標記自動模式是否被手動暫停
unsigned long actionStartTime = 0;      // 用於計時的開始時間
int actionStep = 0;                     // 記錄動作執行到第幾步

// --- Arduino 程式入口：setup() ---
void setup() {
  Serial.begin(115200);

  // 初始化所有硬體接腳
  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  pinMode(IR_AVOID_L, INPUT);
  pinMode(IR_AVOID_R, INPUT);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // 啟動紅外線接收器
  IrReceiver.begin(IR_RECEIVER_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("Smart Car Initialized (Simplified Version)");
  Serial.println("Current Mode: MANUAL");
  
  brake(); // 確保啟動時車子是靜止的
}

// --- Arduino 程式主體：loop() ---
void loop() {
  // 1. 優先處理遙控輸入
  if (IrReceiver.decode()) {
    handleIrInput();
    IrReceiver.resume();
  }

  // 2. 如果有非同步動作正在執行，更新其狀態
  if (isActionRunning) {
    updateAction();
  }

  // 3. 在自動模式下，如果沒有其他動作干擾，則執行自動巡航
  if (controlMode == AUTO && !isActionRunning && !isAutoModePaused) {
    runAutonomousMode();
  }
}

// --- 遙控訊號處理函式 ---
void handleIrInput() {
  unsigned long irData = IrReceiver.decodedIRData.decodedRawData;

  // --- 最高優先級：PLAY/PAUSE 鍵 (通用停止/暫停/恢復) ---
  if (irData == 3158572800) {
    if (controlMode == AUTO) {
      isAutoModePaused = !isAutoModePaused; // 切換自動模式的暫停狀態
      if (isAutoModePaused) {
        brake();
        Serial.println("AUTO Mode Paused.");
      } else {
        Serial.println("AUTO Mode Resumed.");
      }
    } else { // 在手動模式下，此鍵是「緊急停止」
      stopAllActions();
      Serial.println("MANUAL: Emergency Stop.");
    }
    return;
  }

  // --- 次高優先級：模式切換鍵 (EQ) ---
  if (irData == 4127850240) {
    controlMode = (controlMode == MANUAL) ? AUTO : MANUAL;
    stopAllActions(); // 切換模式前，先停止所有動作確保安全
    Serial.print("Mode Switched -> Current Mode: ");
    Serial.println(controlMode == MANUAL ? "MANUAL" : "AUTO");
    return;
  }
  
  // 如果處於自動模式，則忽略所有其他按鍵
  if (controlMode == AUTO) {
    return;
  }

  // --- 以下為手動模式下的指令 ---
  switch (irData) {
    // 移除 delay() 以實現流暢的連續操作
    case 4077715200: 
       Serial.println("MANUAL: Forward");
       delay(100);
       forward(DEFAULT_SPEED);
       break;
   
    case 3877175040: Serial.println("MANUAL: Backward");delay(100); backward(DEFAULT_SPEED); break;
    case 3141861120: Serial.println("MANUAL: Turn Left");delay(100); turnLeft(DEFAULT_SPEED); break;
    case 3208707840: Serial.println("MANUAL: Turn Right");delay(100); turnRight(DEFAULT_SPEED); break;
    case 3125149440: Serial.println("MANUAL: Spin Left");delay(100); spinLeft(DEFAULT_SPEED); break;
    case 3108437760: Serial.println("MANUAL: Spin Right");delay(100); spinRight(DEFAULT_SPEED); break;        
    

    // 對於未定義的按鍵，不執行任何操作，避免誤觸煞車
    default: break;
  }
}

// --- 自動模式主函式 ---
void runAutonomousMode() {
  // 檢查所有感測器
  float distance = ultraSonic();
  bool leftBlocked = !digitalRead(IR_AVOID_L);
  bool rightBlocked = !digitalRead(IR_AVOID_R);
  bool frontBlocked = (distance > 1 && distance < 15); // 增加距離下限，過濾無效讀值

  // 根據感測器狀態決定動作 (有優先級)
  if (frontBlocked) {
    startAvoidObstacle();   // 前方避障優先級最高
  } else if (leftBlocked) {
    startAutoSpinRight();   // 其次處理左側障礙
  } else if (rightBlocked) {
    startAutoSpinLeft();    // 再處理右側障礙
  } else {
    forward(DEFAULT_SPEED); // 暢行無阻
  }
}

// --- 動作更新主函式 (狀態機) ---
void updateAction() {
  switch (currentAction) {
    case ACTION_AVOID:       updateAvoidObstacle(); break; 
    case ACTION_AUTO_SPIN_L: // FALLTHROUGH - 左右微調共用一個更新邏輯
    case ACTION_AUTO_SPIN_R: updateAutoSpin();      break;        
    case ACTION_NONE:        break;
  }
}

// --- 智慧型前方避障邏輯 ---
void startAvoidObstacle() {
  if (isActionRunning) return; 
  isActionRunning = true;
  currentAction = ACTION_AVOID;
  actionStep = 0;
  actionStartTime = millis();
}

void updateAvoidObstacle() {
  // 步驟 0: 後退一段固定時間
  if (actionStep == 0) {
    const unsigned long backupDuration = 400; // 後退時間
    backward(DEFAULT_SPEED); 
    
    if (millis() - actionStartTime >= backupDuration) {
      actionStep = 1;                 // 進入下一步：旋轉尋路
      actionStartTime = millis();       // 重置計時器用於旋轉超時
      spinRight(DEFAULT_SPEED);       // 開始向右旋轉
    }
    return;
  }

  // 步驟 1: 旋轉直到找到出口
  if (actionStep == 1) {
    float distance = ultraSonic();
    
    // 條件1: 找到出口 (前方大於30cm)
    if (distance > 30 && distance < 400) { // 增加上限避免無效超長距離
      stopAllActions();
      return;
    }
    
    // 條件2: 安全超時 (避免在死角無限旋轉)
    const unsigned long spinTimeout = 3000; // 最多轉3秒
    if (millis() - actionStartTime >= spinTimeout) {
      stopAllActions();
      return;
    }
    // 若無滿足條件，則保持旋轉
  }
}

// --- 紅外線微調轉向邏輯 ---
void startAutoSpinLeft() {
  if (isActionRunning) return;
  isActionRunning = true;
  currentAction = ACTION_AUTO_SPIN_L;
  actionStartTime = millis();
  spinLeft(DEFAULT_SPEED);
}

void startAutoSpinRight() {
  if (isActionRunning) return;
  isActionRunning = true;
  currentAction = ACTION_AUTO_SPIN_R;
  actionStartTime = millis();
  spinRight(DEFAULT_SPEED);
}

void updateAutoSpin() {
  const unsigned long spinDuration = 200; // 微調轉向持續時間
  if (millis() - actionStartTime >= spinDuration) {
    stopAllActions(); // 時間到就結束動作
  }
}

// --- 核心輔助函式 ---
void stopAllActions() {
  isActionRunning = false;
  currentAction = ACTION_NONE;
  isAutoModePaused = false; // 任何停止指令都應解除暫停狀態
  brake();
}

// --- 感測器與馬達基礎控制函式 ---
float ultraSonic() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  return pulseIn(ULTRASONIC_ECHO, HIGH, 30000) * 0.034 / 2;
}

void brake() {
  digitalWrite(MOTOR_L_B_IB, LOW);
  digitalWrite(MOTOR_L_B_IA, LOW);
  digitalWrite(MOTOR_R_A_IB, LOW);
  digitalWrite(MOTOR_R_A_IA, LOW);
}

void forward(int v) {
  digitalWrite(MOTOR_L_B_IB, HIGH);
  analogWrite(MOTOR_L_B_IA, 255 - v);
  digitalWrite(MOTOR_R_A_IB, HIGH);
  analogWrite(MOTOR_R_A_IA, 255 - v);
}

void backward(int v) {
  digitalWrite(MOTOR_L_B_IB, LOW);
  analogWrite(MOTOR_L_B_IA, v);
  digitalWrite(MOTOR_R_A_IB, LOW);
  analogWrite(MOTOR_R_A_IA, v);
}

void turnLeft(int v) {
  digitalWrite(MOTOR_L_B_IB, LOW);
  digitalWrite(MOTOR_L_B_IA, LOW);
  digitalWrite(MOTOR_R_A_IB, HIGH);
  analogWrite(MOTOR_R_A_IA, 255 - v);
}

void turnRight(int v) {
  digitalWrite(MOTOR_L_B_IB, HIGH);
  analogWrite(MOTOR_L_B_IA, 255 - v);
  digitalWrite(MOTOR_R_A_IB, LOW);
  digitalWrite(MOTOR_R_A_IA, LOW);
}

void spinLeft(int v) {
  digitalWrite(MOTOR_L_B_IB, LOW);
  analogWrite(MOTOR_L_B_IA, v);
  digitalWrite(MOTOR_R_A_IB, HIGH);
  analogWrite(MOTOR_R_A_IA, 255 - v);
}

void spinRight(int v) {
  digitalWrite(MOTOR_L_B_IB, HIGH);
  analogWrite(MOTOR_L_B_IA, 255 - v);
  digitalWrite(MOTOR_R_A_IB, LOW);
  analogWrite(MOTOR_R_A_IA, v);
}

void newtone(int t, int f, int d) {
  int p = 1000000L / f;
  int pulse = p / 2;
  for (long i = 0; i < d * 1000L; i += p) {
    digitalWrite(t, HIGH);
    delayMicroseconds(pulse);
    digitalWrite(t, LOW);
    delayMicroseconds(pulse);
  }
}

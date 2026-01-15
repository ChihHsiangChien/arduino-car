/*
 * 完整版智慧車 (SimpleIR Version)
 * 
 * 修改說明：
 * 1. 改用自製的 "SimpleIR" 函式庫，不再依賴外部的 IRremote。
 * 2. 為了配合 SimpleIR (軟體接收)，移除了所有 delay() 阻塞，確保 loop() 跑得夠快。
 * 3. 超音波偵測的 pulseIn 超時設定縮短，避免卡住紅外線接收。
 */

#include "SimpleIR.h" // 引入自製紅外線庫

// --- 硬體接腳定義 ---
#define MOTOR_L_B_IB 2
#define MOTOR_L_B_IA 3
#define MOTOR_R_A_IB 4
#define MOTOR_R_A_IA 5
#define IR_AVOID_L 6
#define IR_AVOID_R 7
#define ULTRASONIC_TRIG 8
#define ULTRASONIC_ECHO 9
#define IR_RECEIVER_PIN 10
#define BUZZER_PIN 11

SimpleIR ir(IR_RECEIVER_PIN); // 建立紅外線物件

// --- 參數設定 ---
const int DEFAULT_SPEED = 200;

enum Mode { MANUAL, AUTO };
Mode controlMode = MANUAL;

enum ActionType { 
  ACTION_NONE, 
  ACTION_AVOID, 
  ACTION_AUTO_SPIN_L, 
  ACTION_AUTO_SPIN_R 
};

ActionType currentAction = ACTION_NONE;
bool isActionRunning = false;
bool isAutoModePaused = false;
unsigned long actionStartTime = 0;
int actionStep = 0;

/*
 * setup(): Arduino 的初始化函式
 * - 設定所有硬體腳位的輸出入模式
 * - 啟動 Serial 序列埠通訊 (除錯用)
 * - 啟動紅外線接收
 */
void setup() {
  Serial.begin(115200);

  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  pinMode(IR_AVOID_L, INPUT);
  pinMode(IR_AVOID_R, INPUT);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // 初始化自製紅外線
  ir.begin();

  Serial.println("Smart Car (SimpleIR) Initialized");
  Serial.println("Current Mode: MANUAL");
  
  brake();
}

/*
 * loop(): 主迴圈
 * 採用「非阻塞 (Non-blocking)」架構，確保紅外線與自動避障能同時運作。
 * 1. 持續檢查 ir.check() 避免漏接訊號。
 * 2. 若收到遙控訊號 -> handleIrInput()。
 * 3. 若有正在進行的自動動作 (如避障轉彎) -> updateAction()。
 * 4. 若處於自動模式且閒置 -> runAutonomousMode()。
 */
void loop() {
  // 1. 持續偵測紅外線 (一定要放在 loop 最上層且不能被 delay 卡住)
  ir.check();

  // 2. 處理接收到的訊號
  if (ir.hasSignal()) {
    unsigned long key = ir.getResult();
    handleIrInput(key);
    ir.resume(); // 準備接收下一筆
  }

  // 3. 更新非阻塞動作狀態
  if (isActionRunning) {
    updateAction();
  }

  // 4. 自動模式邏輯 (僅在無動作且未暫停時執行)
  // 為了避免影響紅外線接收，超音波不應每回合都測，可以使用簡單計時分時處理
  if (controlMode == AUTO && !isActionRunning && !isAutoModePaused) {
    runAutonomousMode();
  }
}

/*
 * handleIrInput(): 處理遙控器輸入
 * @param irData: 接收到的紅外線編碼
 * 
 * 功能：
 * - 模式切換 (EQ鍵): 切換手動/自動。
 * - 暫停/恢復 (Play鍵): 在自動模式下暫停。
 * - 手動控制: 前後左右與停止 (僅在手動模式有效)。
 */
void handleIrInput(unsigned long irData) {
  // 最高優先級：Play/Pause (緊急停止/暫停)
  if (irData == 3158572800) { // 假設這是你的 Play 鍵碼
    if (controlMode == AUTO) {
      isAutoModePaused = !isAutoModePaused;
      if (isAutoModePaused) {
        brake();
        Serial.println("AUTO Paused");
      } else {
        Serial.println("AUTO Resumed");
      }
    } else {
      stopAllActions();
      Serial.println("Emergency Stop");
    }
    return;
  }

  // 模式切換 (EQ)
  if (irData == 4127850240) {
    controlMode = (controlMode == MANUAL) ? AUTO : MANUAL;
    stopAllActions();
    Serial.print("Mode: ");
    Serial.println(controlMode == MANUAL ? "MANUAL" : "AUTO");
    return;
  }
  
  if (controlMode == AUTO) return;

  // 手動控制指令 (移除 delay，改用點動或短觸發)
  switch (irData) {
    case 16718055: // Button 2
      Serial.println("MANUAL: Forward");
      forward(DEFAULT_SPEED);
      // 這裡沒有 delay，所以手放開時需要依賴重複訊號或手動按停，
      // 或是可以設計一個 Timer 自動煞車，這裡簡化為動作後不自動停，需按停止鍵
      break;
   
    case 16730805: // Button 8
      Serial.println("MANUAL: Backward");
      backward(DEFAULT_SPEED); 
      break;

    case 16716015: // Button 4
      Serial.println("MANUAL: Left");
      turnLeft(DEFAULT_SPEED); 
      break;

    case 16734885: // Button 6
      Serial.println("MANUAL: Right");
      turnRight(DEFAULT_SPEED); 
      break;
      
    case 16726215: // Button 5 (Stop)
      Serial.println("MANUAL: Stop");
      brake();
      break;

    default: break;
  }
}

/*
 * runAutonomousMode(): 自動導航核心邏輯
 * 
 * 策略：
 * 1. 讀取超音波 (每 50ms 一次，避免在此卡住太久)。
 * 2. 根據障礙物位置決定動作優先級：前方 > 左側 > 右側。
 * 3. 觸發對應的避障流程 (startAvoidObstacle 等)。
 */
void runAutonomousMode() {
  // 為了保持 SimpleIR 的靈敏度，我們不能頻繁呼叫 pulseIn
  static unsigned long lastCheckTime = 0;
  if (millis() - lastCheckTime < 50) return; // 每 50ms 才做一次判斷
  lastCheckTime = millis();

  float distance = ultraSonic();
  bool leftBlocked = !digitalRead(IR_AVOID_L);
  bool rightBlocked = !digitalRead(IR_AVOID_R);
  bool frontBlocked = (distance > 1 && distance < 20);

  if (frontBlocked) {
    startAvoidObstacle();
  } else if (leftBlocked) {
    startAutoSpinRight();
  } else if (rightBlocked) {
    startAutoSpinLeft();
  } else {
    forward(150); // 自動模式速度稍慢一點
  }
}

/*
 * updateAction(): 狀態機更新函式
 * 
 * 功能：
 * 當某個耗時動作 (如: 後退0.4秒 -> 轉彎) 正在進行時，這個函式會負責檢查
 * 「時間到了沒？」並執行下一個步驟。
 * 取代了 delay() 寫法。
 */
void updateAction() {
  switch (currentAction) {
    case ACTION_AVOID: updateAvoidObstacle(); break; 
    case ACTION_AUTO_SPIN_L: 
    case ACTION_AUTO_SPIN_R: updateAutoSpin(); break;        
    case ACTION_NONE: break;
  }
}

/*
 * startAvoidObstacle(): 啟動「前方避障」流程
 * 流程：先後退 -> 再右轉。
 */
void startAvoidObstacle() {
  if (isActionRunning) return; 
  isActionRunning = true;
  currentAction = ACTION_AVOID;
  actionStep = 0;
  actionStartTime = millis();
}

/*
 * updateAvoidObstacle(): 執行「前方避障」的每一步驟
 * 
 * 步驟 0: 後退 400ms。
 * 步驟 1: 右轉，直到超音波看到前方淨空 (>30cm) 或超時 (2秒)。
 */
void updateAvoidObstacle() {
  if (actionStep == 0) { // 後退
    backward(DEFAULT_SPEED); 
    if (millis() - actionStartTime >= 400) {
      actionStep = 1;
      actionStartTime = millis();
      spinRight(DEFAULT_SPEED);
    }
    return;
  }
  if (actionStep == 1) { // 旋轉找路
    // 這裡也要限制超音波頻率
    static unsigned long lastScan = 0;
    if (millis() - lastScan > 50) {
      lastScan = millis();
      float distance = ultraSonic();
      if (distance > 30) {
        stopAllActions();
        return;
      }
    }
    // 超時強制停止
    if (millis() - actionStartTime >= 2000) {
      stopAllActions();
    }
  }
}

/*
 * startAutoSpinLeft/Right(): 啟動「紅外線避障」流程
 * 單純的轉向動作，用於閃避側邊障礙。
 */
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

/*
 * updateAutoSpin(): 紅外線避障的計時器
 * 轉動 200ms 後自動停止。
 */
void updateAutoSpin() {
  if (millis() - actionStartTime >= 200) {
    stopAllActions();
  }
}

/*
 * stopAllActions(): 強制停止當前自動動作
 * 重置所有狀態旗標並煞車。
 */
void stopAllActions() {
  isActionRunning = false;
  currentAction = ACTION_NONE;
  brake();
}

/*
 * ultraSonic(): 讀取超音波距離
 * 注意：pulseIn 的 timeout 設為 5000us (約 85cm)，
 * 這是為了避免在等待回波時卡住 CPU 太久，影響紅外線接收。
 */
float ultraSonic() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  // 重要：縮短超時時間至 5ms (約 85cm)，避免卡住紅外線接收
  return pulseIn(ULTRASONIC_ECHO, HIGH, 5000) * 0.034 / 2;
}

// --- 馬達控制函式 (H-Bridge Logic) ---
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
void brake() {
  digitalWrite(MOTOR_L_B_IB, LOW); digitalWrite(MOTOR_L_B_IA, LOW);
  digitalWrite(MOTOR_R_A_IB, LOW); digitalWrite(MOTOR_R_A_IA, LOW);
}

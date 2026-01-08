/*
 * 路徑記憶與重播 (Memory Path Recorder)
 * 
 * 教學目標：
 * 1. 資料結構 (Data Structure)：學習宣告 `struct` 來定義一個「動作 (Action)」，包含「做什麼 (Command)」與「做多久 (Duration)」。
 * 2. 陣列 (Array)：使用陣列作為「記憶體」，儲存一連串的動作。
 * 3. 機器人教導模式 (Teaching Mode)：
 *    - [錄製階段]：你用遙控器開車，程式會自動記錄你按了什麼鍵、按了多久。
 *    - [重播階段]：車子會讀取陣列，自動重現剛才的所有動作。
 * 
 * 操作說明：
 * 1. 剛開機為 [待機模式]。
 * 2. 按遙控器任意方向鍵，進入 [錄製模式] 並開始移動。
 * 3. 駕駛一段路徑後，按 EQ 鍵 (或其他指定鍵) 結束錄製，進入 [重播模式]。
 * 4. 車子將無限循環重播剛才的路徑 (直到斷電或重置)。
 */

#include "SimpleIR.h"

// --- 硬體接腳 ---
#define MOTOR_L_B_IB 2
#define MOTOR_L_B_IA 3
#define MOTOR_R_A_IB 4
#define MOTOR_R_A_IA 5
#define IR_RECEIVER_PIN 10
#define BUZZER_PIN 11

SimpleIR ir(IR_RECEIVER_PIN);

// --- 記憶體設定 ---
// 定義一個動作的結構
struct Action {
  unsigned long command; // 按鍵指令 (做什麼)
  unsigned long duration; // 持續時間 (做多久)
};

const int MAX_ACTIONS = 50; // 最多記憶 50 個動作 (受限於 Arduino RAM)
Action actionMemory[MAX_ACTIONS]; // 宣告動作陣列
int actionCount = 0; // 目前記錄了幾個動作

// --- 系統狀態 ---
enum SystemState { IDLE, RECORDING, REPLAY };
SystemState state = IDLE;

// 錄製用變數
unsigned long lastCommand = 0; // 上一個指令
unsigned long lastChangeTime = 0; // 上次改變動作的時間點

// 重播用變數
int replayIndex = 0;
unsigned long replayStartTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  ir.begin();
  
  Serial.println("Memory Path Recorder Initialized.");
  Serial.println("Waiting for input...");
  
  // 提示音
  tone(BUZZER_PIN, 1000, 100);
}

void loop() {
  ir.check();
  
  switch (state) {
    // --- 1. 待機模式 ---
    case IDLE:
      if (ir.hasSignal()) {
        unsigned long key = ir.getResult();
        if (isValidMoveKey(key)) {
          // 收到第一個移動指令，開始錄製
          state = RECORDING;
          Serial.println("--- Recording Started ---");
          
          // 記錄初始狀態
          lastCommand = key;
          lastChangeTime = millis();
          executeCommand(key); // 立即執行
          
          tone(BUZZER_PIN, 500, 200); // 錄製開始提示音
        }
        ir.resume();
      }
      break;

    // --- 2. 錄製模式 ---
    case RECORDING:
      if (ir.hasSignal()) {
        unsigned long key = ir.getResult();
        
        // 如果按了 EQ 鍵 (結束錄製)
        if (key == 4127850240) { 
          saveCurrentAction(); // 儲存最後一個動作
          finishRecording();   // 結束並切換到重播
        } 
        // 如果按了跟剛才不一樣的鍵 (動作改變)
        else if (key != lastCommand && isValidMoveKey(key)) {
          saveCurrentAction();       // 1. 結算上一個動作的時間並存檔
          lastCommand = key;         // 2. 更新當前指令
          lastChangeTime = millis(); // 3. 重置計時器
          executeCommand(key);       // 4. 執行新動作
          
          Serial.print("New Action: "); Serial.println(key);
        }
        ir.resume();
      }
      break;

    // --- 3. 重播模式 ---
    case REPLAY:
      runReplayLogic();
      break;
  }
}

// 判斷是否為有效移動鍵 (上下左右停)
bool isValidMoveKey(unsigned long key) {
  return (key == 16718055 || key == 16730805 || key == 16716015 || key == 16734885 || key == 16726215);
}

// 將當前累積的時間與動作存入陣列
void saveCurrentAction() {
  if (actionCount >= MAX_ACTIONS) {
    Serial.println("Memory Full!");
    return;
  }
  
  unsigned long duration = millis() - lastChangeTime;
  
  // 存入記憶體
  actionMemory[actionCount].command = lastCommand;
  actionMemory[actionCount].duration = duration;
  
  Serial.print("Saved #"); Serial.print(actionCount);
  Serial.print(": Cmd="); Serial.print(lastCommand);
  Serial.print(", Time="); Serial.println(duration);
  
  actionCount++;
}

void finishRecording() {
  brake();
  state = REPLAY;
  replayIndex = 0;
  replayStartTime = millis();
  
  Serial.println("--- Recording Finished ---");
  Serial.print("Total Actions: "); Serial.println(actionCount);
  Serial.println("--- Replay Starting in 2s ---");
  
  tone(BUZZER_PIN, 1000, 200); delay(200);
  tone(BUZZER_PIN, 1000, 200); delay(1000); // 雙嗶聲
  tone(BUZZER_PIN, 2000, 500);
  delay(1000);
}

void runReplayLogic() {
  if (actionCount == 0) return;

  // 檢查當前動作是否做完了
  if (millis() - replayStartTime >= actionMemory[replayIndex].duration) {
    // 換下一個動作
    replayIndex++;
    
    // 如果全部播完了 -> 重頭開始 (Loop)
    if (replayIndex >= actionCount) {
      replayIndex = 0;
      Serial.println("Looping...");
      tone(BUZZER_PIN, 2000, 100); // 循環提示音
      delay(1000); // 休息一下
    }
    
    // 執行新動作
    unsigned long cmd = actionMemory[replayIndex].command;
    executeCommand(cmd);
    replayStartTime = millis(); // 重置計時
    
    Serial.print("Replay #"); Serial.print(replayIndex);
    Serial.print(": Cmd="); Serial.println(cmd);
  }
}

// 執行動作
void executeCommand(unsigned long key) {
  switch (key) {
    case 16718055: forward(200); break;  // Forward
    case 16730805: backward(200); break; // Backward
    case 16716015: turnLeft(200); break; // Left
    case 16734885: turnRight(200); break;// Right
    case 16726215: brake(); break;       // Stop
  }
}

// 馬達控制
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

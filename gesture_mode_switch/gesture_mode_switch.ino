// 1. 定義 4 種狀態（數字代表 0, 1, 2, 3）
#define STATE_RUN   0   // 全速前進
#define STATE_RIGHT 1   // 全速右轉
#define STATE_LEFT  2   // 全速左轉
#define STATE_STOP  3   // 完全煞車

int currentState = STATE_STOP; // 開機預設為：煞車待命狀態
bool isDark = false;           // 狀態機專用標記：記錄手掌「目前是不是蓋著」

void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
  brake(); // 開機先靜止
  Serial.begin(9600);
}

void loop() {
  int light = analogRead(A0);

  // ==================================================
  // 核心邏輯 A：手勢偵測（手蓋上再拿開，狀態才會跳一格）
  // ==================================================
  if (light < 150 && isDark == false) {
    // 手掌剛好蓋上（變暗）
    isDark = true; 
  } 
  else if (light >= 150 && isDark == true) {
    // 手掌剛好拿開（變亮）-> 觸發切換狀態！
    isDark = false; 
    
    // 狀態依序往後跳一檔：0 -> 1 -> 2 -> 3 -> 0
    currentState = currentState + 1;
    if (currentState > 3) {
      currentState = STATE_RUN; // 超過 3 檔就回到 0 檔
    }
    
    Serial.println("\n--- 偵測到手勢！切換下一檔 ---");
  }

  // ==================================================
  // 核心邏輯 B：狀態機（根據目前狀態，決定執行什麼動作）
  // ==================================================
  switch (currentState) {
    
    case STATE_RUN:
      forward(); // 行為：全速前進
      Serial.println("【目前狀態：0 - 全速前進】");
      break;

    case STATE_RIGHT:
      turnRight(); // 行為：全速右轉
      Serial.println("【目前狀態：1 - 全速右轉】");
      break;

    case STATE_LEFT:
      turnLeft(); // 行為：全速左轉
      Serial.println("【目前狀態：2 - 全速左轉】");
      break;

    case STATE_STOP:
      brake(); // 行為：完全煞車
      Serial.println("【目前狀態：3 - 完全煞車】...請揮手切換到前進");
      break;
  }

  delay(30); // 輕微延遲，穩定運行
}

// ==================================================
// 基本 4 大動作函式
// ==================================================
void forward() {
  digitalWrite(2, 1); digitalWrite(3, 0);
  digitalWrite(4, 1); digitalWrite(5, 0);
}

void turnRight() {
  digitalWrite(2, 1); digitalWrite(3, 0);
  digitalWrite(4, 0); digitalWrite(5, 0); // 右輪不動，左輪動
}

void turnLeft() {
  digitalWrite(2, 0); digitalWrite(3, 0); // 左輪不動，右輪動
  digitalWrite(4, 1); digitalWrite(5, 0);
}

void brake() {
  digitalWrite(2, 0); digitalWrite(3, 0);
  digitalWrite(4, 0); digitalWrite(5, 0);
}

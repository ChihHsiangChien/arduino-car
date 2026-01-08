// --- 紅外線避障自動導航車 (IR Audio Avoiding Car) ---
// 功能：使用左右紅外線感測器偵測障礙物，並根據行為同時發出不同音效。
// 所有音效皆為「非阻塞 (Non-blocking)」，不會讓車子動作停頓。

// --- 硬體接腳定義 ---
#define MOTOR_L_B_IB 2    // 左輪馬達控制腳位 B-IB
#define MOTOR_L_B_IA 3    // 左輪馬達控制腳位 B-IA (PWM)
#define MOTOR_R_A_IB 4    // 右輪馬達控制腳位 A-IB
#define MOTOR_R_A_IA 5    // 右輪馬達控制腳位 A-IA (PWM)

#define IR_AVOID_L 6      // 左側紅外線感測器 (LOW代表有障礙物)
#define IR_AVOID_R 7      // 右側紅外線感測器 (LOW代表有障礙物)
#define BUZZER_PIN 11     // 蜂鳴器

// --- 參數設定 ---
const int DRIVE_SPEED = 180; // 前進速度
const int TURN_SPEED = 200;  // 轉向速度

// --- 狀態定義 ---
enum CarState {
  ST_FORWARD,     // 前進中
  ST_AVOID_LEFT,  // 左邊有障礙 (需右轉)
  ST_AVOID_RIGHT, // 右邊有障礙 (需左轉)
  ST_BACKING      // 雙邊都有障礙 (需後退)
};

CarState currentState = ST_FORWARD;

// --- 音效計時器變數 ---
unsigned long lastSoundTime = 0;
bool soundToggle = false; // 用於切換高低音或開關

void setup() {
  Serial.begin(115200);

  // 初始化腳位
  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  
  pinMode(IR_AVOID_L, INPUT);
  pinMode(IR_AVOID_R, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);



  Serial.println("IR Audio Car Initialized.");
  // 啟動音效
  tone(BUZZER_PIN, 1000, 200);
  delay(200);
  tone(BUZZER_PIN, 2000, 200);
  delay(1000); 
}

void loop() {
  // 1. 讀取感測器
  bool leftBlocked = digitalRead(IR_AVOID_L) ;
  bool rightBlocked = digitalRead(IR_AVOID_R);

  // 2. 決定狀態
  if (leftBlocked && rightBlocked) {
    currentState = ST_BACKING;
  } else if (leftBlocked) {
    currentState = ST_AVOID_LEFT;
  } else if (rightBlocked) {
    currentState = ST_AVOID_RIGHT;
  } else {
    currentState = ST_FORWARD;
  }

  // 3. 執行動作
  moveCar(currentState);

  // 4. 播放音效
  playSound(currentState);
}

// --- 動作控制函式 ---
void moveCar(CarState state) {
  switch (state) {
    case ST_FORWARD:
      forward(DRIVE_SPEED);
      break;
      
    case ST_AVOID_LEFT:
      // 左邊有擋，向右原地旋轉
      spinRight(TURN_SPEED);
      Serial.println("Action: Spin Right");
      break;
      
    case ST_AVOID_RIGHT:
      // 右邊有擋，向左原地旋轉
      spinLeft(TURN_SPEED);
      Serial.println("Action: Spin Left");
      break;
      
    case ST_BACKING:
      // 雙邊有擋，先後退
      backward(DRIVE_SPEED);
      Serial.println("Action: Backward");
      break;
  }
}

// --- 非阻塞音效控制器 ---
// 根據不同狀態播放不同頻率和節奏的聲音
void playSound(CarState state) {
  unsigned long currentMillis = millis();
  int interval = 0;
  int freq1 = 0;
  int freq2 = 0;

  // 定義各狀態的聲音特徵
  switch (state) {
    case ST_FORWARD:
      // 前進時：每 1000ms 發出極短的 500Hz 聲
      interval = 1000;
      freq1 = 500;
      freq2 = 0; // 0 代表靜音
      break;

    case ST_AVOID_LEFT:
      // 右轉避障中：急促的單音
      interval = 100;
      freq1 = 1500;
      freq2 = 0; 
      break;

    case ST_AVOID_RIGHT:
      // 左轉避障中：急促的單音
      interval = 100;
      freq1 = 1800;
      freq2 = 0;
      break;

    case ST_BACKING:
      // 倒車中：雙音警報
      interval = 200;
      freq1 = 800;
      freq2 = 1200;
      break;
  }

  // 檢查時間是否到了
  if (currentMillis - lastSoundTime >= interval) {
    lastSoundTime = currentMillis;
    soundToggle = !soundToggle; // 切換狀態

    if (soundToggle) {
      if (freq1 > 0) tone(BUZZER_PIN, freq1);
      else noTone(BUZZER_PIN);
    } else {
      if (freq2 > 0) tone(BUZZER_PIN, freq2);
      else noTone(BUZZER_PIN);
    }
  }
}

// --- 馬達底層函式 ---
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

void brake() {
  digitalWrite(MOTOR_L_B_IB, LOW);
  digitalWrite(MOTOR_L_B_IA, LOW);
  digitalWrite(MOTOR_R_A_IB, LOW);
  digitalWrite(MOTOR_R_A_IA, LOW);
}

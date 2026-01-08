// --- 倒車雷達自動導航車 (Radar Autonomous Car) ---
// 功能：車子會自動前進，隨著離障礙物越近，蜂鳴器會發出越急促的聲音。
// 不需要紅外線遙控器。

// --- 硬體接腳定義 ---
#define MOTOR_L_B_IB 2    // 左輪馬達控制腳位 B-IB
#define MOTOR_L_B_IA 3    // 左輪馬達控制腳位 B-IA (PWM)
#define MOTOR_R_A_IB 4    // 右輪馬達控制腳位 A-IB
#define MOTOR_R_A_IA 5    // 右輪馬達控制腳位 A-IA (PWM)

#define ULTRASONIC_TRIG 8 // 超音波感測器 Trig
#define ULTRASONIC_ECHO 9 // 超音波感測器 Echo
#define BUZZER_PIN 11     // 蜂鳴器

// --- 參數設定 ---
const int DRIVE_SPEED = 160;  // 自動行駛速度 (不要太快，以免反應不及)
const int SAFE_DIST = 50;     // 開始發出警告音的距離 (公分)
const int DANGER_DIST = 20;   // 危險距離 (公分)，低於此距離會煞車轉向
const int STOP_DIST = 10;     // 極限停止距離

unsigned long lastBeepTime = 0; // 記錄上次嗶聲的時間
bool buzzerState = false;       // 記錄目前蜂鳴器是響還是停

void setup() {
  Serial.begin(115200);

  // 初始化腳位
  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // 初始化隨機數種子 (利用空接腳 A5 的雜訊)
  randomSeed(analogRead(A5));

  Serial.println("Radar Car Initialized.");
  delay(1000); // 等待1秒準備出發
}

void loop() {
  // 1. 測量距離
  float distance = getDistance();
  Serial.print("Dist: "); Serial.println(distance);

  // 2. 處理駕駛邏輯 (Drive) & 3. 處理雷達音效 (Radar Sound)
  
  // 分為三個區間：
  // A. 安全區 (> SAFE_DIST)：安靜，直行
  // B. 警戒區 (DANGER_DIST ~ SAFE_DIST)：依距離發出緩->急的聲音，直行
  // C. 危險區 (< DANGER_DIST)：長鳴或超急促音，執行避障(後退轉彎)

  if (distance > SAFE_DIST) {
    // --- 安全區 ---
    noTone(BUZZER_PIN); // 不發聲
    forward(DRIVE_SPEED);
  } 
  else if (distance > DANGER_DIST) {
    // --- 警戒區 (雷達音效) ---
    forward(DRIVE_SPEED); // 繼續前進
    
    // 計算嗶聲間隔：距離越近，間隔越短 (50cm->500ms, 20cm->50ms)
    // map(value, fromLow, fromHigh, toLow, toHigh)
    int interval = map(distance, DANGER_DIST, SAFE_DIST, 50, 500);
    playRadarSound(interval);
  } 
  else {
    // --- 危險區 (避障) ---
    // 1. 煞車並長鳴
    brake();
    tone(BUZZER_PIN, 1000); // 持續長音
    delay(300); // 停頓一下讓慣性消失

    // 2. 後退
    tone(BUZZER_PIN, 800); // 倒車音調
    backward(DRIVE_SPEED);
    delay(500); 

    // 3. 轉彎 (隨機左或右)
    tone(BUZZER_PIN, 1200); // 轉向音調
    if (random(2) == 0) {
      Serial.println("Avoid: Spin Left");
      spinLeft(DRIVE_SPEED);
    } else {
      Serial.println("Avoid: Spin Right");
      spinRight(DRIVE_SPEED);
    }
    delay(400);

    // 4. 回復
    noTone(BUZZER_PIN); // 關閉聲音
    brake();
    delay(200);
  }
}

// --- 非阻塞式雷達音效函式 (Non-blocking Sound) ---
// 
// tone() 和 noTone() 是Arduino 官方內建函式。
//     - tone(pin, frequency): 會啟動硬體計時器 (Timer) 在背景產生方波，CPU 設定完後就不用管它，所以發聲本身不佔用 CPU。
//     - noTone(pin): 停止該腳位的訊號輸出。
//
// 是非阻塞 (Non-blocking)，因為沒有使用 delay() 來控制聲音的「長度」或「間隔」。
//     - 阻塞式寫法： tone(); delay(100); noTone(); delay(100); -> 這會讓整個程式停住 0.2 秒，車子會頓挫。
//     - 非阻塞寫法： 如下方程式碼，我們只是「看手錶 (millis)」。
//
// 利用「時間差」的概念 (Time Slicing)：
//     1. 每次進來這個函式，先檢查現在時間 (millis()) 跟上次響的時間 (lastBeepTime) 差了多久。
//     2. 如果時間還沒到 (差值 < interval)，直接離開，讓 CPU 去做別的事 (如控制馬達)。
//     3. 如果時間到了，就切換蜂鳴器的狀態 (開變關、關變開)，並更新 lastBeepTime。
//     這樣 CPU 可以在兩次嗶聲之間的空檔去處理超音波和馬達，達成「多工處理」的效果。
void playRadarSound(int interval) {
  unsigned long currentMillis = millis(); // 取得開機到現在經過的毫秒數 (看手錶)
  
  // 檢查是否到達設定的時間間隔
  if (currentMillis - lastBeepTime >= interval) {
    lastBeepTime = currentMillis; // 更新最後一次動作的時間 (重設手錶)
    
    // 切換狀態 (Toggle)
    if (buzzerState) {
      noTone(BUZZER_PIN); // 內建函式：停止發聲
      buzzerState = false;
    } else {
      tone(BUZZER_PIN, 2000); // 內建函式：背景發出 2000Hz 聲音
      buzzerState = true;
    }
  }
}

// --- 感測器函式 ---
float getDistance() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  
  // 讀取脈衝時間
  long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 25000); // 設定超時 25ms (約 4公尺)
  
  if (duration == 0) return 999; // 超時代表前方空曠
  return duration * 0.034 / 2;
}

// --- 馬達控制 ---
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

/*
 * 非阻塞 (Non-blocking) 音效教學範例
 * 
 * 目標：
 * 展示如何在 Arduino 上播放音樂或音效，同時還能做其他事情（例如控制馬達、閃爍 LED、讀取感測器）。
 * 如果使用 delay()，程式會暫停，導致車子頓挫或感測器失靈。
 * 使用 millis() 和狀態機 (State Machine) 可以解決這個問題。
 * 
 * 硬體需求：
 * - 蜂鳴器接在 Pin 11 (或其他支援 PWM 的腳位)
 */

#define BUZZER_PIN 11

// --- 變數設定 ---
// 用來記錄上一次動作的時間點 (就像手錶)
unsigned long previousToneMillis = 0;
unsigned long previousLedMillis = 0;

// 音效狀態機的變數
int melodyState = 0;      // 目前播到第幾個音
int MELODY_INTERVAL = 200;// 每個音的長度 (毫秒)

// 模擬主任務 (例如 LED 閃爍) 的變數
bool ledState = false;

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT); // Arduino 內建 LED

  Serial.println("====== non blocking start ======");
  Serial.println("you should hear regular sounds, and see Serial window keep printing numbers, representing the program is not blocked.");
}

void loop() {
  // 任務 1: 播放背景音效 (不會卡住程式)
  playBackgroundMusic();

  // 任務 2: 執行主程式 (例如控制馬達，這裡用閃爍 LED 和列印數字代替)
  runMainTask();
}

/*
 * 教學重點：非阻塞音效函式
 * 原理：
 * 1. 每次 loop() 執行時，都進來問：「時間到了嗎？」(millis() - previousMillis >= 間隔)
 * 2. 如果時間還沒到 -> 馬上離開，讓 CPU 去做別的事。
 * 3. 如果時間到了 -> 改變音調，並更新時間標記。
 */
void playBackgroundMusic() {
  unsigned long currentMillis = millis();

  // 檢查是否到了切換下一個音的時間
  if (currentMillis - previousToneMillis >= MELODY_INTERVAL) {
    previousToneMillis = currentMillis; // 更新時間標記

    // 根據目前的狀態 (melodyState) 決定發出什麼聲音
    // 這裡示範一個簡單的 Do-Re-Mi 循環
    switch (melodyState) {
      case 0:
        tone(BUZZER_PIN, 523); // Do (C5)
        Serial.println(" Do");
        break;
      case 1:
        tone(BUZZER_PIN, 587); // Re (D5)
        Serial.println(" Re");
        break;
      case 2:
        tone(BUZZER_PIN, 659); // Mi (E5)
        Serial.println(" Mi");
        break;
      case 3:
        noTone(BUZZER_PIN);    // 休息 (不發聲)
        Serial.println(" (rest)");
        break;
    }

    // 準備下一次要播的狀態 (循環 0 -> 1 -> 2 -> 3 -> 0...)
    melodyState++;
    if (melodyState > 3) {
      melodyState = 0;
    }
  }
}

/*
 * 模擬主任務
 * 這代表你的車子正在避障、讀取紅外線、或是控制馬達。
 * 這裡用 LED 快速閃爍來證明 CPU 一直在工作，沒有被聲音卡住。
 */
void runMainTask() {
  unsigned long currentMillis = millis();
  
  // 每 100ms 閃爍一次 LED (比音樂還快)
  if (currentMillis - previousLedMillis >= 50) {
    previousLedMillis = currentMillis;
    
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
    
    // 這裡可以放馬達控制程式碼
    // forward(200); ...
  }
}

/*
 * 雙蜂鳴器和弦 (Dual Buzzer Harmony) 範例
 * 
 * 原理：
 * 一般 Arduino (如 Uno/Nano) 的 tone() 函式一次只能驅動一個蜂鳴器，
 * 如果對第二個腳位呼叫 tone()，第一個會自動停止。
 * 
 * 要達成同時發聲 (和弦)，我們必須使用更底層的技巧：
 * 1. 一個蜂鳴器使用 tone() (佔用 Timer 2)。
 * 2. 另一個蜂鳴器我們必須「手動」產生頻率 (利用 millis 計時翻轉電位)。
 *    或者使用第三方函式庫 (如 ToneLibrary)，但這裡我們用最單純的手寫方式達成。
 */

// --- 硬體接腳 ---
#define BUZZER_MAIN_PIN 11  // 主旋律蜂鳴器 (使用硬體 tone)
#define BUZZER_SUB_PIN  12  // 和聲蜂鳴器 (使用軟體模擬)

// --- 音符頻率表 ---
#define NOTE_C4  262
#define NOTE_E4  330
#define NOTE_G4  392
#define NOTE_C5  523

// --- 變數設定 ---
unsigned long lastUpdate = 0;
const int MELODY_SPEED = 500; // 每 0.5 秒換一個音

// 軟體蜂鳴器變數
unsigned long previousSubBuzzerMicros = 0;
bool subBuzzerState = false;
int subBuzzerFreq = 0; // 0 代表靜音

int step = 0; // 進度

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_MAIN_PIN, OUTPUT);
  pinMode(BUZZER_SUB_PIN, OUTPUT);
  
  Serial.println("Dual Buzzer Harmony Test Start");
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. 每隔一段時間切換和弦 (主邏輯)
  if (currentMillis - lastUpdate >= MELODY_SPEED) {
    lastUpdate = currentMillis;
    playHarmony(step);
    step++;
    if (step > 3) step = 0;
  }

  // 2. 驅動軟體蜂鳴器 (必須在 loop 中極高速執行)
  handleSoftwareTone();
}

// 切換和弦組合
void playHarmony(int s) {
  switch (s) {
    case 0:
      // C Major (Do + Mi)
      Serial.println("Chord: C Major (C4 + E4)");
      tone(BUZZER_MAIN_PIN, NOTE_C4); // 主音：Do
      subBuzzerFreq = NOTE_E4;        // 和聲：Mi
      break;
      
    case 1:
      // C Major (Mi + Sol)
      Serial.println("Chord: / (E4 + G4)");
      tone(BUZZER_MAIN_PIN, NOTE_E4); // 主音：Mi
      subBuzzerFreq = NOTE_G4;        // 和聲：Sol
      break;
      
    case 2:
      // C Major (Sol + High Do)
      Serial.println("Chord: / (G4 + C5)");
      tone(BUZZER_MAIN_PIN, NOTE_G4); // 主音：Sol
      subBuzzerFreq = NOTE_C5;        // 和聲：High Do
      break;
      
    case 3:
      // 靜音
      Serial.println("Silence");
      noTone(BUZZER_MAIN_PIN);
      subBuzzerFreq = 0;
      break;
  }
}

// --- 軟體模擬 Tone (核心技術) ---
// 原理：如果不使用 Timer 中斷，我們就在 loop 裡用微秒 (micros) 來計時。
// 只要 loop 跑得夠快，就可以產生方波。
void handleSoftwareTone() {
  if (subBuzzerFreq <= 0) {
    digitalWrite(BUZZER_SUB_PIN, LOW);
    return;
  }

  unsigned long currentMicros = micros();
  // 計算半週期的時間 (微秒) = 1秒 / 頻率 / 2
  unsigned long interval = 1000000UL / subBuzzerFreq / 2;

  if (currentMicros - previousSubBuzzerMicros >= interval) {
    previousSubBuzzerMicros = currentMicros;
    
    // 翻轉電位
    subBuzzerState = !subBuzzerState;
    digitalWrite(BUZZER_SUB_PIN, subBuzzerState ? HIGH : LOW);
  }
}

/*
 * 手寫音頻與音樂 (Manual Tone Song)
 * 
 * 教學目標：
 * 1. 深入理解聲音原理：聲音其實就是空氣的震動。
 *    我們透過快速開關蜂鳴器 (HIGH/LOW)，就能模擬出震動。
 * 
 * 2. 手寫 newtone 函式：
 *    Arduino 雖然有內建 `tone()`，但自己寫一個 `newtone()` 能讓我們學會：
 *    - 頻率 (Frequency) 與 週期 (Period) 的關係：週期 = 1,000,000 / 頻率 (微秒)。
 *    - 佔空比 (Duty Cycle)：這裡我們用 50% (一半時間 HIGH，一半時間 LOW)。
 * 
 * 3. 實作一首簡單的歌曲：小星星。
 */

#define BUZZER_PIN 11

// 音符頻率表 (C大調) - 使用唱名命名
// 避開 Arduino 內建的 A0-A5 腳位名稱，A4 (La) 改用 La4
const int Do4 = 262;
const int Re4 = 294;
const int Mi4 = 330;
const int Fa4 = 349;
const int So4 = 392;
const int La4 = 440;
const int Si4 = 494;
const int Do5 = 523;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Playing Manual Tone Song...");
}

void loop() {
  // 小星星簡譜: 1 1 5 5 6 6 5 - 
  //             4 4 3 3 2 2 1 -
  
  // 第一句：一閃一閃亮晶晶
  playNote(Do4, 500);
  playNote(Do4, 500);
  playNote(So4, 500);
  playNote(So4, 500);
  playNote(La4, 500);
  playNote(La4, 500);
  playNote(So4, 1000); // 長音

  // 第二句：滿天都是小星星
  playNote(Fa4, 500);
  playNote(Fa4, 500);
  playNote(Mi4, 500);
  playNote(Mi4, 500);
  playNote(Re4, 500);
  playNote(Re4, 500);
  playNote(Do4, 1000); // 長音
  
  delay(2000); // 唱完休息兩秒
}

// --- 手寫音頻產生函式 ---
// t: pin (腳位)
// f: frequency (頻率 Hz)
// d: duration (持續時間 ms)
void newtone(int t, int f, int d) {
  // 計算週期 (Period) = 1秒 / 頻率
  // 因為單位是微秒 (us)，所以用 1,000,000 除以 頻率
  // 注意：數字後面的 'L' 代表這是一個 "Long" (長整數)。
  // Arduino Uno 的標準 int 只有 16 位元 (最大值 32,767)。
  // 1,000,000 遠大於 int 上限，所以編譯器通常會自動處理，但加上 'L' 是更保險的寫法。
  long p = 1000000L / f;
  
  long pulse = p / 2; // HIGH 和 LOW 各佔一半時間
  
  // 總共要震動多久？ d * 1000 改成微秒
  // 這裡的 'L' 非常重要！
  // 如果 d (持續時間) 是 33ms，那 33 * 1000 = 33000，這已經超過 int 上限 (32767) 了！
  // 如果不加 'L'，計算結果會溢位 (Overflow) 變成負數，導致程式出錯。
  // 加上 '1000L' 強制讓計算過程使用 Long 型態，避免溢位。
  for (long i = 0; i < d * 1000L; i += p) {
    digitalWrite(t, HIGH);
    delayMicroseconds(pulse);
    digitalWrite(t, LOW);
    delayMicroseconds(pulse);
  }
}

// 輔助函式：播放單一音符並稍微停頓，讓音符分開
void playNote(int note, int duration) {
  if (note > 0) {
    newtone(BUZZER_PIN, note, duration);
  } else {
    delay(duration); // 休止符
  }
  delay(50); // 音符之間的短暫間隔，讓聲音不黏在一起
}

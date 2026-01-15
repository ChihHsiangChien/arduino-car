/*
 * 遙控電子琴 (IR Remote Piano)
 * 
 * 教學目標：
 * 1. 結合紅外線輸入與蜂鳴器輸出：將每一個遙控器按鍵對應到一個音階。
 * 2. 頻率與音階對照：
 *    - 0: C4 (Do) - 262 Hz
 *    - 1: D4 (Re) - 294 Hz
 *    - 2: E4 (Mi) - 330 Hz
 *    - 3: F4 (Fa) - 349 Hz
 *    - 4: G4 (So) - 392 Hz
 *    - 5: A4 (La) - 440 Hz
 *    - 6: B4 (Si) - 494 Hz
 *    - 7: C5 (Do) - 523 Hz
 *    - 8: D5 (Re) - 587 Hz
 * 
 * 任務：試著用遙控器彈奏一首簡單的兒歌（例如小蜜蜂：533 422 1234555）。
 */

#include "SimpleIR.h"

#define IR_RECEIVER_PIN 10
#define BUZZER_PIN 11

// --- 音階定義 (Solfège Names) ---
// 為什麼不用 C4, D4, E4...？
// 因為 Arduino 的 A0~A5 已經被定義為「類比輸入腳位」了。
// 如果我們定義 #define A4 440，會導致編譯錯誤 (跟硬體腳位名稱衝突)。
// 使用 唱名 (Do, Re, Mi) 既直觀又不會撞名。

const int Do4 = 262;
const int Re4 = 294;
const int Mi4 = 330;
const int Fa4 = 349;
const int So4 = 392;
const int La4 = 440;
const int Si4 = 494;
const int Do5 = 523;
const int Re5 = 587;

// --- 節拍定義 ---
const int Beat   = 300; // 一拍
const int Beat2  = 600; // 兩拍
const int Beat05 = 150; // 半拍

SimpleIR ir(IR_RECEIVER_PIN);

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  ir.begin();
  
  Serial.println("IR Remote Piano Started.");
  Serial.println("Press 0-8 to play notes.");
  
  // 開機提示音
  tone(BUZZER_PIN, Do5, 100);
  delay(100);
  tone(BUZZER_PIN, Do5*2, 100);
}

void loop() {
  ir.check();
  
  if (ir.hasSignal()) {
    unsigned long key = ir.getResult();
    playNoteFromKey(key);
    ir.resume();
  }
}

void playNoteFromKey(unsigned long key) {
  int freq = 0;
  
  switch (key) {
    case 16738455: freq = Do4; Serial.println("Do"); break; // Button 0
    case 16724175: freq = Re4; Serial.println("Re"); break; // Button 1
    case 16718055: freq = Mi4; Serial.println("Mi"); break; // Button 2
    case 16743045: freq = Fa4; Serial.println("Fa"); break; // Button 3
    case 16716015: freq = So4; Serial.println("So"); break; // Button 4
    case 16726215: freq = La4; Serial.println("La"); break; // Button 5
    case 16734885: freq = Si4; Serial.println("Si"); break; // Button 6
    case 16728765: freq = Do5; Serial.println("Do5"); break; // Button 7
    case 16730805: freq = Re5; Serial.println("Re5"); break; // Button 8
    
    // EQ 鍵播放範例曲：小蜜蜂
    case 16748655: 
      Serial.println("Demo Song: Bumble Bee");
      playDemoSong();
      break;
      
    default: 
      Serial.println("Unknown Note");
      break;
  }
  
  if (freq > 0) {
    tone(BUZZER_PIN, freq, Beat); // 預設播放一拍
  }
}

void playDemoSong() {
  // 小蜜蜂簡譜：5 3 3 - 4 2 2 - 1 2 3 4 5 5 5
  // 現在看程式碼就像看樂譜一樣直觀！
  int melody[] = {
    So4, Mi4, Mi4, 
    Fa4, Re4, Re4, 
    Do4, Re4, Mi4, Fa4, So4, So4, So4
  };
  
  int duration[] = {
    Beat, Beat, Beat2, 
    Beat, Beat, Beat2, 
    Beat, Beat, Beat, Beat, Beat, Beat, Beat2
  };
  
  int totalNotes = sizeof(melody) / sizeof(melody[0]);
  
  for (int i=0; i<totalNotes; i++) {
    tone(BUZZER_PIN, melody[i], duration[i]);
    delay(duration[i] * 1.3); 
  }
}

/*
 * 記憶電子琴 (Memory Piano)
 * 
 * 教學目標：
 * 1. 雖然我們還沒學會機器人「移動」的記憶，但我們可以先來練習「旋律」的記憶。
 * 2. 銜接單元：從 `remote_piano.ino` (只會彈) -> `memory_piano.ino` (會記) -> `memory_path_recorder.ino` (會記動作)。
 * 
 * 功能說明：
 * 1. [演奏模式]：平常按數字鍵 0-8 是彈鋼琴，Arduino 會偷偷把你彈的音符記在陣列裡。
 * 2. [重播模式]：按下 OK 鍵 (中間鍵)，立刻重播剛剛彈的那一串旋律。
 */

#include "SimpleIR.h"

// --- 音階定義 ---
const int Do4 = 262;
const int Re4 = 294;
const int Mi4 = 330;
const int Fa4 = 349;
const int So4 = 392;
const int La4 = 440;
const int Si4 = 494;
const int Do5 = 523;
const int Re5 = 587;

#define IR_RECEIVER_PIN 10
#define BUZZER_PIN 11
#define NEXT_KEY 16712445 // NEXT鍵的紅外線編碼

SimpleIR ir(IR_RECEIVER_PIN);

// --- 記憶體 ---
const int MAX_NOTES = 50; // 最多記 50 個音
int noteMemory[MAX_NOTES]; // 儲存音符頻率
int noteIndex = 0;         // 目前記到第幾個音

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  ir.begin();
  
  Serial.println("Memory Piano Started.");
  Serial.println("Play notes (0-8), then press OK to replay.");
  
  tone(BUZZER_PIN, Do5, 100); delay(150);
  tone(BUZZER_PIN, Do5, 100);
}

void loop() {
  ir.check();
  
  if (ir.hasSignal()) {
    unsigned long key = ir.getResult();
    
    // 如果按的是 OK 鍵 -> 重播
    if (key == NEXT_KEY) {
      replayMelody();
    }
    // 否則當作是彈琴
    else {
      int freq = getFreqFromKey(key);
      if (freq > 0) {
        // 1. 發聲
        tone(BUZZER_PIN, freq, 200); 
        Serial.print("Note: "); Serial.println(freq);
        
        // 2. 記錄進陣列
        if (noteIndex < MAX_NOTES) {
          noteMemory[noteIndex] = freq;
          noteIndex++;
        } else {
          Serial.println("Memory Full (Resetting...)");
          noteIndex = 0; // 滿了就重頭洗掉
        }
      }
    }
    ir.resume();
  }
}

// 重播邏輯
void replayMelody() {
  if (noteIndex == 0) {
    Serial.println("Memory is empty!");
    return;
  }
  
  Serial.println("--- Replaying Melody ---");
  for (int i = 0; i < noteIndex; i++) {
    tone(BUZZER_PIN, noteMemory[i], 200);
    delay(250); // 固定拍子重播
  }
  
  // 播完清空記憶，準備下一次錄音
  // 若想保留記憶，把下面這行註解掉即可
  noteIndex = 0; 
  Serial.println("--- Cleared ---");
}

int getFreqFromKey(unsigned long key) {
  switch (key) {
    case 16738455: return Do4; // Button 0
    case 16724175: return Re4; // Button 1
    case 16718055: return Mi4; // Button 2
    case 16743045: return Fa4; // Button 3
    case 16716015: return So4; // Button 4
    case 16726215: return La4; // Button 5
    case 16734885: return Si4; // Button 6
    case 16728765: return Do5; // Button 7
    case 16730805: return Re5; // Button 8
    default: return 0;
  }
}

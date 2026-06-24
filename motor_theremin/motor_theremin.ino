void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
  brake();             // 開機先靜止
  Serial.begin(9600);  
  delay(1000);         
}

// 升級版馬達唱歌函式：給予不同的 toneValue，馬達就會發出不同的音高
void motorSing(int toneValue, int duration) {
  // 防呆機制：限制音量/音高範圍，太高車子會跑走，太低動不起來
  // if (toneValue > 110) toneValue = 110;
  // if (toneValue < 10) toneValue = 10;

  // 對馬達 2,3 與 4,5 輸出微小的 PWM 高頻震動
  digitalWrite(2, 1);
  analogWrite(3, 255 - toneValue);
  digitalWrite(4, 0);
  analogWrite(5, toneValue);
  
  delay(duration); // 維持這個音符多久
}

void loop() {
  int light = analogRead(A0);
  
  Serial.print("目前光照: ");
  Serial.print(light);

  // 【核心演算法】將光敏電阻的數值，對應到馬達唱歌的音高 (12 ~ 50)
  // 假設你房間的一般亮度是 600，手完全蓋住是 200
  // 當光線越強 (手抬高)，對應出來的數字越大 (音調越高)
  int currentTone = map(light, 100, 907, 50, 200);
  
  Serial.print("  ➔ 對應音高: ");
  Serial.println(currentTone);

  // 如果手完全放開，光線太強，或者手完全蓋死沒光，就讓它安靜 (不唱歌)
  if (light > 750 || light < 100) {
    brake(); 
    delay(50);
  } else {
    // 讓馬達以當下的音高唱出一個極短的音符（50毫秒）
    // 隨著 loop 快速重複，就會變成連續滑順的音調變化！
    motorSing(currentTone, 50); 
  }
}

// 煞車（安靜）函式
void brake() {
  digitalWrite(2, 0);
  digitalWrite(3, 0);
  digitalWrite(4, 0);
  digitalWrite(5, 0);
}

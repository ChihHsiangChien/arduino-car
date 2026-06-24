void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
  brake();             // 開機時確保馬達先靜止
  Serial.begin(9600);  // 開啟序列埠監控視窗，可以用來看 A0 的數值
  
  // 安全緩衝：開機後先等 3 秒才開始跑 loop 
  // 這樣萬一電路又導致斷線，你至少有 3 秒的時間可以點擊燒錄新程式！
  delay(3000); 
}

void loop() {
  int light = analogRead(A0);
  
  // 在序列埠監控視窗印出目前的光照數值
  Serial.print("Light value: ");
  Serial.println(light);

  // 將 0 ~ 1023 切分成 7 個區間，執行你寫的所有動作（皆為全速）
  if (light > 850) {
    // 【極強光】全力前進！
    forward(); 
  } 
  else if (light > 700) {
    // 【強光】全速左轉 (單邊輪轉動)
    turnLeft(); 
  } 
  else if (light > 550) {
    // 【中等偏亮】全速右轉 (單邊輪轉動)
    turnRight(); 
  } 
  else if (light > 400) {
    // 【中等偏暗】全速原地左旋 (兩側輪反向，轉比較快)
    spinLeft(); 
  } 
  else if (light > 250) {
    // 【暗】全速原地右旋 (兩側輪反向，轉比較快)
    spinRight(); 
  } 
  else if (light > 100) {
    // 【極暗】全速倒車！
    backward(); 
  } 
  else {
    // 【全黑 / 接近 0】完全煞車
    brake(); 
  }

  delay(100); // 稍微加長一點延遲到 100ms，讓小車在各動作切換時不會太過抖動
}
// -----------------------------------------------------------------
// 以下是原本自訂的馬達控制函式，維持不變（僅在 go() 內補上未宣告的 forward(v) 呼叫優化）
// -----------------------------------------------------------------

void go(int d) {
  // 起步
  for (int v = 8; v < 25; v++) {
    forward(v * 10);
    delay(d * 10);
  }
  // 前進
  forward(255);
  delay(d * 100);

  // 慢慢煞車
  for (int v = 25; v > 8; v--) {
    forward(v * 10);
    delay(d * 5);
  }
}

void forward() {
  digitalWrite(2, 1);
  digitalWrite(3, 0);
  digitalWrite(4, 1);
  digitalWrite(5, 0);
}

void forward(int v) {
  digitalWrite(2, 1);
  analogWrite(3, 255 - v);
  digitalWrite(4, 1);
  analogWrite(5, 255 - v);
}

void forward(int v_L, int v_R) {
  if (v_L > 255) v_L = 255;
  if (v_R > 255) v_R = 255;
  digitalWrite(2, 1);
  analogWrite(3, 255 - v_L);
  digitalWrite(4, 1);
  analogWrite(5, 255 - v_R);
}

void backward() {
  digitalWrite(2, 0);
  digitalWrite(3, 1);
  digitalWrite(4, 0);
  digitalWrite(5, 1);
}

void turnLeft() {
  digitalWrite(2, 0);
  digitalWrite(3, 0);
  digitalWrite(4, 1);
  digitalWrite(5, 0);
}

void turnRight() {
  digitalWrite(2, 1);
  digitalWrite(3, 0);
  digitalWrite(4, 0);
  digitalWrite(5, 0);
}

void spinLeft() {
  digitalWrite(2, 0);
  digitalWrite(3, 1);
  digitalWrite(4, 1);
  digitalWrite(5, 0);
}

void spinRight() {
  digitalWrite(2, 1);
  digitalWrite(3, 0);
  digitalWrite(4, 0);
  digitalWrite(5, 1);
}

void brake() {
  digitalWrite(2, 0);
  digitalWrite(3, 0);
  digitalWrite(4, 0);
  digitalWrite(5, 0);
}

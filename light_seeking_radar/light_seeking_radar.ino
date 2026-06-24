void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
  brake();             // 開機時確保馬達先靜止
  Serial.begin(9600);  
  
  delay(3000);         // 安全緩衝 3 秒
}

void loop() {
  int maxLight = 0;         // 用來記錄掃描到的最高亮度
  int bestTimeOffset = 0;   // 用來記錄在第幾毫秒時抓到最高亮度

  Serial.println("--- 開始雷達掃描（旋轉一圈） ---");
  
  // 【步驟 1】讓小車全速原地右旋 1.2 秒（你可以根據實際旋轉圈數微調 1200 這個數字）
  // 我們把這 1.2 秒切成 24 次微小的移動，每次移動 50 毫秒並量測亮度
  spinRight(); 
  
  for (int t = 0; t < 1200; t += 50) {
    delay(50);
    int currentLight = analogRead(A0);
    
    // 如果現在讀到的光比之前紀錄的還要亮，就更新最高紀錄
    if (currentLight > maxLight) {
      maxLight = currentLight;
      bestTimeOffset = t; // 記下是在旋轉過程中的哪個「時間點」最亮
    }
  }
  
  brake(); // 掃描完畢，先短暫煞車
  delay(200);

  // 【步驟 2】判斷環境。如果環境太暗（例如最高亮度還小於 200），代表沒手電筒照它，那就休息不衝刺
  if (maxLight < 200) {
    Serial.println("太暗了，找不到光源，5秒後重新掃描...");
    delay(5000);
    return; // 跳過後面的步驟，重新回到 loop 開頭掃描
  }

  // 【步驟 3】修正方向！
  // 剛剛是往右旋轉，現在我們要往左旋轉「相同的時間差」，把自己轉回最亮的方向
  Serial.print("鎖定最強光源！數值: ");
  Serial.println(maxLight);
  Serial.println("正在修正車頭朝向光源...");
  
  spinLeft();
  // 電腦計算：剛剛在第 bestTimeOffset 毫秒最亮，所以要往回轉 (1200 - bestTimeOffset) 毫秒
  delay(1200 - bestTimeOffset); 
  
  brake(); // 轉到位了，煞車定格
  delay(200);

  // 【步驟 4】全速衝刺！
  Serial.println("向光源衝刺 1.5 秒！");
  forward();    // 全速前進
  delay(1500);  // 衝刺 1.5 秒
  
  brake();      // 衝完煞車
  Serial.println("抵達目的地，準備下一次掃描...\n");
  delay(2000);  // 休息 2 秒，接著 loop 會重新開始下一輪雷達掃描
}

// -----------------------------------------------------------------
// 以下是你原本的馬達基本控制函式（維持不變，讓 loop 直接呼叫）
// -----------------------------------------------------------------

void forward() {
  digitalWrite(2, 1);
  digitalWrite(3, 0);
  digitalWrite(4, 1);
  digitalWrite(5, 0);
}

void backward() {
  digitalWrite(2, 0);
  digitalWrite(3, 1);
  digitalWrite(4, 0);
  digitalWrite(5, 1);
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

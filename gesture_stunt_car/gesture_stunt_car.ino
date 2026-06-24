void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
  brake();             
  Serial.begin(9600);  
  delay(1000);         
}

// 利用馬達高頻震動發出短音的函式
void playBeep(int duration) {
  // 給馬達一個極小的速度（轉不動，但會發出嗡嗡聲）
  // 這裡利用 3 號和 5 號腳位（PWM 腳）
  digitalWrite(2, 1);
  analogWrite(3, 255 - 25); // 數值 25 左右會發出聲音
  digitalWrite(4, 1);
  analogWrite(5, 255 - 25);
  
  delay(duration); // 叫多久
  brake();         // 停止發聲
}

void loop() {
  int baseLight = analogRead(A0);
  int threshold = baseLight - 150; 
  if (threshold < 50) threshold = 50; 

  int gestureCount = 0;   
  bool isDark = false;    

  // ====== 提示 1：發出開頭音，告訴你「可以開始揮手了！」 ======
  Serial.println("\n【Beep！開始偵測，請在 4 秒內揮手】");
  playBeep(150); // 短叫 0.15 秒
  delay(200);    // 停頓一下讓人類反應

  unsigned long startTime = millis();
  unsigned long lastCountdownTime = startTime;
  int secondsLeft = 4;

  while (millis() - startTime < 4000) { 
    int currentLight = analogRead(A0);

    // 每過一秒就在序列埠印出倒數（方便看電腦時確認）
    if (millis() - lastCountdownTime >= 1000) {
      secondsLeft--;
      Serial.print("倒數 "); Serial.print(secondsLeft); Serial.println(" 秒...");
      lastCountdownTime = millis();
    }

    // 狀態機手勢判斷
    if (currentLight < threshold && isDark == false) {
      isDark = true; 
    } 
    else if (currentLight >= threshold && isDark == true ) {
      gestureCount++;
      isDark = false;
      Serial.print("➔ 偵測到揮手！累計: "); Serial.println(gestureCount);
      
      // 每成功揮手一次，就微弱地「逼」一聲當作視覺/聽覺反饋
      playBeep(40); 
      delay(110); 
    }
    delay(20); 
  }

  // ====== 提示 2：偵測結束，連續逼兩聲 ======
  Serial.println("【Beep、Beep！偵測結束，開始執行特技】");
  playBeep(80);
  delay(80);
  playBeep(80);
  delay(300); // 留下一點緩衝時間再暴衝，比較安全

  // --- 根據次數執行特技（維持你原本的動作） ---
  if (gestureCount == 1) {
    forward(); delay(1000); brake();
  } 
  else if (gestureCount == 2) {
    digitalWrite(2, 1); digitalWrite(3, 0); digitalWrite(4, 0); digitalWrite(5, 0); delay(400);
    digitalWrite(2, 0); digitalWrite(3, 0); digitalWrite(4, 1); digitalWrite(5, 0); delay(400);
    digitalWrite(2, 1); digitalWrite(3, 0); digitalWrite(4, 0); digitalWrite(5, 0); delay(400);
    digitalWrite(2, 0); digitalWrite(3, 0); digitalWrite(4, 1); digitalWrite(5, 0); delay(400);
    brake();
  } 
  else if (gestureCount == 3) {
    spinRight(); delay(600);
    spinLeft(); delay(600);
    spinRight(); delay(600);
    brake();
  } 
  else if (gestureCount >= 4) {
    backward(); delay(1200); brake();
  } 

  Serial.println("特技結束。休息 3 秒...\n");
  delay(3000); 
}

// -----------------------------------------------------------------
// 原本的基本控制函式
// -----------------------------------------------------------------
void forward() {
  digitalWrite(2, 1); digitalWrite(3, 0); digitalWrite(4, 1); digitalWrite(5, 0);
}
void backward() {
  digitalWrite(2, 0); digitalWrite(3, 1); digitalWrite(4, 0); digitalWrite(5, 1);
}
void spinLeft() {
  digitalWrite(2, 0); digitalWrite(3, 1); digitalWrite(4, 1); digitalWrite(5, 0);
}
void spinRight() {
  digitalWrite(2, 1); digitalWrite(3, 0); digitalWrite(4, 0); digitalWrite(5, 1);
}
void brake() {
  digitalWrite(2, 0); digitalWrite(3, 0); digitalWrite(4, 0); digitalWrite(5, 0);
}

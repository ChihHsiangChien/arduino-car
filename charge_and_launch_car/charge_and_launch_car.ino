void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
  brake();             
  Serial.begin(9600);  
  delay(1000);         
}

// 馬達發聲功能
void motorBeep(int duration, int volume) {
  if (volume > 110) volume = 110; // 安全防呆
  digitalWrite(2, 1);
  analogWrite(3, 255 - volume);
  digitalWrite(4, 0); 
  analogWrite(5, volume);
  delay(duration);
  brake();
}

void loop() {
  // 1. 抓取目前的環境光作為初始基準
  int baseLight = analogRead(A0);
  int threshold = baseLight - 200; 
  if (threshold < 50) threshold = 50;

  Serial.println("環境正常，充電站待命中...（請蓋住 A0 進行蓄力）");
  
  // 2. 迴圈：一直亮著就一直卡在這邊，直到被蓋住為止
  while (analogRead(A0) >= threshold) {
    delay(100); 
  }

  // ==================================================
  // 【新增音效：開始遮光提示】發出兩聲由低到高的爬音
  // ==================================================
  Serial.println("➔ 偵測到覆蓋！");
  motorBeep(60, 50);  // 低音嗶
  delay(30);
  motorBeep(60, 90);  // 高音嗶
  Serial.println("⚡ 能量核心開始充能...");

  // 3. 進入【充能蓄力階段】
  unsigned long coverStartTime = millis(); 
  unsigned long lastHeartbeat = millis();
  int beepInterval = 400; 
  unsigned long finalChargeTime = 0; 

  // 當手一直蓋著（低於臨界值），就持續在迴圈內累加能量
  while (analogRead(A0) < threshold) {
    unsigned long duration = millis() - coverStartTime;

    if (duration > 5000) {
      duration = 5000; // 限制最高 5 秒
    }
    
    finalChargeTime = duration; 

    // 蓄力期間的心跳音效：隨著時間越來越急促 (400ms 縮短到 80ms)
    beepInterval = map(duration, 0, 5000, 400, 80);
    
    if (millis() - lastHeartbeat >= beepInterval) {
      Serial.print("⚡ 充能中... 蓄力值 (ms): "); Serial.println(finalChargeTime);
      motorBeep(20, 75); // 短促的充能音
      lastHeartbeat = millis();
    }
    
    delay(10); 
  }

  // ==================================================
  // 【新增音效：遮光結束提示】手拿開的瞬間，發出一個高頻發射音
  // ==================================================
  Serial.print("🚀 手掌移開！");
  motorBeep(120, 110); // 響亮的高頻發射音「啾！」
  
  Serial.print("釋放能量！準備前進時間 (ms): "); 
  Serial.println(finalChargeTime);
  delay(100); // 給音效一點餘韻

  // 4. 核心動作：根據剛剛遮光的時間，決定前進多久！
  forward();
  delay(finalChargeTime); 
  
  // 衝刺完畢，緊急煞車
  brake();
  Serial.println("🏁 衝刺完畢，冷卻 2 秒...\n");
  delay(2000); 
}

// -----------------------------------------------------------------
// 馬達基本控制函式
// -----------------------------------------------------------------
void forward() {
  digitalWrite(2, 1); digitalWrite(3, 0); digitalWrite(4, 1); digitalWrite(5, 0);
}
void brake() {
  digitalWrite(2, 0); digitalWrite(3, 0); digitalWrite(4, 0); digitalWrite(5, 0);
}

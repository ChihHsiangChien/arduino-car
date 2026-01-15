/*
 * 8字型繞行 (Figure 8 Demo)
 * 
 * 目標：
 * 1. 學習「差速轉向 (Differential Steering)」：
 *    - 先前的正方形與五角星大多使用「原地旋轉 (Spin)」，也就是左右輪反向轉。
 *    - 但真實車輛轉彎通常是「一邊快、一邊慢」。
 *    - 差速原理：外側輪子跑得比內側輪子快，車子就會轉彎。速差越小，轉彎半徑越大（大圓）；速差越大，轉彎半徑越小（急轉）。
 * 
 * 2. 練習複合路徑：
 *    - 8 字型其實就是兩個圓的組合：一個逆時針圓 + 一個順時針圓。
 *    - 挑戰在沒有感測器的情況下，讓兩個圓盡量一樣大，並接回原點。
 * 
 * 任務：讓車子走出一個 "8" 字形。
 */

#define MOTOR_L_B_IB 2
#define MOTOR_L_B_IA 3
#define MOTOR_R_A_IB 4
#define MOTOR_R_A_IA 5

// 參數設定 (需要針對地毯或地板摩擦力微調)
/*
 * 調校指南 (Tuning Guide):
 * 1. 調整圓的大小：
 *    - 想要大圓：減少 HIGH_SPEED 與 LOW_SPEED 的差距 (例如 255 vs 250)。
 *    - 想要小圓：增加差距。
 * 
 * 2. 調整圓的完整度：
 *    - 修改 CIRCLE_TIME：如果車子沒畫完一個圓，增加時間；畫過頭了，減少時間。
 * 
 * 3. 硬體誤差 (Hardware Bias) - 重要！
 *    - 幾乎所有的廉價直流馬達，左右兩顆特性都不會完全一樣。
 *    - 即使程式寫 `forward(255)`，車子可能還是會稍微偏左或偏右。
 *    - 在畫 8 字型時，如果你發現左轉圈比較快，右轉圈比較慢，這是正常的。
 *    - 修正方法：你需要針對比較弱的那一輪，稍微把速度數值調高一點點來補償 (軟體校正)。
 */
const int HIGH_SPEED = 255; // 外側輪速度
const int LOW_SPEED = 100;   // 內側輪速度 (建議與 HIGH 有 100~150 的差距，轉彎才明顯)
const int CIRCLE_TIME = 3500; // 畫一個圓大約需要的時間

// --- 直線校正參數 (影響 differentialDrive 的基礎出力) ---
const float LEFT_FACTOR = 1.0; 
const float RIGHT_FACTOR = 1.0;

void setup() {
  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  
  // 緩衝時間
  delay(2000);
}

void loop() {
  // 步驟 1: 逆時針畫圓 (靠左轉)
  // 左輪慢 (內側)，右輪快 (外側)
  differentialDrive(LOW_SPEED, HIGH_SPEED);
  delay(CIRCLE_TIME);
  
  // 步驟 2: 順時針畫圓 (靠右轉)
  // 左輪快 (外側)，右輪慢 (內側)
  differentialDrive(HIGH_SPEED, LOW_SPEED);
  delay(CIRCLE_TIME);
  
  // 步驟 3: 停車休息，準備下一輪
  brake();
  delay(2000);
}

// --- 差速驅動函式 ---
// 可以分別控制左右輪的速度
void differentialDrive(int leftSpeed, int rightSpeed) {
  // 應用校正參數
  // 注意：這裡只會將數值變小，不會變大超過 255
  int vL = leftSpeed * LEFT_FACTOR;
  int vR = rightSpeed * RIGHT_FACTOR;

  // 左輪
  digitalWrite(MOTOR_L_B_IB, HIGH);
  analogWrite(MOTOR_L_B_IA, 255 - vL);
  
  // 右輪
  digitalWrite(MOTOR_R_A_IB, HIGH);
  analogWrite(MOTOR_R_A_IA, 255 - vR);
}

void brake() {
  digitalWrite(MOTOR_L_B_IB, LOW);
  digitalWrite(MOTOR_L_B_IA, LOW);
  digitalWrite(MOTOR_R_A_IB, LOW);
  digitalWrite(MOTOR_R_A_IA, LOW);
}

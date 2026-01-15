/*
 * 直線校正工具 (Straight Line Calibration Tool)
 * 
 * 教學目標：
 * 1. 發現問題：車子為什麼走不了直線？
 *    - 因為左右兩個馬達的製造公差、齒輪摩擦力、輪胎抓地力都不可能完全一樣。
 *    - 這是所有輪型機器人都會遇到的「物理現實」。
 * 
 * 2. 解決問題：軟體校正 (Software Calibration)
 *    - 我們不能換掉馬達，但我們可以透過程式讓「強的輪子跑慢一點」。
 *    - 找出最佳的修正係數 (Factor)。
 * 
 * 操作方法：
 * 1. 上傳程式，車子會向前走 3 秒。
 * 2. 觀察車子偏向哪邊？
 *    - 偏左：代表右輪太快 -> 減小 RIGHT_FACTOR。
 *    - 偏右：代表左輪太快 -> 減小 LEFT_FACTOR。
 * 3. 修改程式中的 FACTOR 數值，重新上傳，直到車子能走得夠直為止。
 * 4. 記下這組黃金比例，用在之後所有的程式中！
 */

#define MOTOR_L_B_IB 2
#define MOTOR_L_B_IA 3
#define MOTOR_R_A_IB 4
#define MOTOR_R_A_IA 5

// --- 在這裡調整參數 ---
// 如果車子偏左 (右輪快)，把 RIGHT_FACTOR 改成 0.9, 0.85 試試...
// 如果車子偏右 (左輪快)，把 LEFT_FACTOR 改成 0.9, 0.85 試試...
const float LEFT_FACTOR = 1.0; 
const float RIGHT_FACTOR = 1.0;

const int TEST_SPEED = 255; // 測試速度

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  
  Serial.println("Calibration Start in 3s...");
  delay(3000); // 給你 3 秒鐘放車
  
  Serial.println("GO!");
  forward(TEST_SPEED);
  
  delay(3000); // 走 3 秒鐘 (這段時間越長，偏移越明顯)
  
  Serial.println("STOP!");
  brake();
}

void loop() {
  // 做完一次就停，不要重複，方便觀察
  // 如果想重測，請按 Arduino 上的 Reset 鍵
}

// 應用校正因子的前進函式
void forward(int v) {
  // 核心公式：目標速度 * 校正因子
  int vL = v * LEFT_FACTOR;
  int vR = v * RIGHT_FACTOR;
  
  Serial.print("L: "); Serial.print(vL);
  Serial.print(" | R: "); Serial.println(vR);
  
  digitalWrite(MOTOR_L_B_IB, HIGH);
  analogWrite(MOTOR_L_B_IA, 255 - vL);
  digitalWrite(MOTOR_R_A_IB, HIGH);
  analogWrite(MOTOR_R_A_IA, 255 - vR);
}

void brake() {
  digitalWrite(MOTOR_L_B_IB, LOW);
  digitalWrite(MOTOR_L_B_IA, LOW);
  digitalWrite(MOTOR_R_A_IB, LOW);
  digitalWrite(MOTOR_R_A_IA, LOW);
}

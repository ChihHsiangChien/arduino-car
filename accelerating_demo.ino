/*
 * 線性加速 (Smooth Acceleration Demo)
 * 
 * 教學目標：
 * 1. 理解 PWM (Pulse Width Modulation)：
 *    - Arduino 的 `analogWrite(0-255)` 其實不是真的輸出類比電壓，而是透過快速切換開關 (PWM) 來模擬電壓強弱。
 *    - 255 = 100% 全速, 127 = 50% 速度, 0 = 0% 停止。
 * 
 * 2. 認識「啟動死區 (Dead Zone)」：
 *    - 馬達要克服靜摩擦力才能開始轉動。
 *    - 通常 PWM 給到 10, 20, 30 時，馬達只會發出「嗡嗡聲」但不會動。
 *    - 直到突破某個門檻 (例如 80 或 100)，車子才會突然衝出去。
 * 
 * 3. 實作「斜坡控制 (Ramp Control)」：
 *    - 不要直接將速度設為 255，而是用迴圈慢慢累加 (0 -> 5 -> 10 ... -> 255)。
 *    - 這能避免車輪打滑，保護齒輪箱，並讓行駛質感更高級 (像真實車輛起步)。
 * 
 * 任務：車子會從靜止緩緩加速到最快，維持高速，再緩緩減速到停。
 */

#define MOTOR_L_B_IB 2
#define MOTOR_L_B_IA 3
#define MOTOR_R_A_IB 4
#define MOTOR_R_A_IA 5

// 馬達參數
const int MIN_POWER = 0;   
const int MAX_POWER = 255;
const int DEAD_ZONE = 100; // 靜摩擦力門檻 (若車子只有嗡嗡聲不走，請調大此值)

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  
  delay(1000);
}

void loop() {
  Serial.println("Action: Accelerating...");
  
  // 1. 加速階段 (從 0 加到 255)
  // 這是一個「斜坡 (Ramp)」函數
  for (int speed = MIN_POWER; speed <= MAX_POWER; speed += 5) {
    // 當 speed < DEAD_ZONE (例如 80) 時，車子可能只會有高頻電流音 (Humming) 但不會動。
    // 這很正常，因為扭力還不足以克服摩擦力。
    // 現實中的解法：可以直接從 DEAD_ZONE 開始起跳 (例如 int speed = DEAD_ZONE)。
    
    forward(speed);
    Serial.print("Speed: "); Serial.println(speed);
    
    // delay(50) 決定了加速的快慢 (斜率)
    // 數字越小，加速越快；數字越大，加速越慢
    delay(50); 
  }

  // 2. 高速巡航
  Serial.println("Action: Cruising...");
  delay(1000);

  // 3. 減速階段 (從 255 減到 0)
  Serial.println("Action: Decelerating...");
  for (int speed = MAX_POWER; speed >= MIN_POWER; speed -= 5) {
    forward(speed);
    Serial.print("Speed: "); Serial.println(speed);
    delay(50);
  }

  // 4. 停車休息
  Serial.println("Action: Stopped.");
  brake();
  delay(2000);
  
  // 重複循環
}

void forward(int v) {
  // 保護機制：若 v < 0 則當作 0
  if (v < 0) v = 0;
  
  digitalWrite(MOTOR_L_B_IB, HIGH);
  analogWrite(MOTOR_L_B_IA, 255 - v); 
  digitalWrite(MOTOR_R_A_IB, HIGH);
  analogWrite(MOTOR_R_A_IA, 255 - v);
}

void brake() {
  digitalWrite(MOTOR_L_B_IB, LOW);
  digitalWrite(MOTOR_L_B_IA, LOW);
  digitalWrite(MOTOR_R_A_IB, LOW);
  digitalWrite(MOTOR_R_A_IA, LOW);
}

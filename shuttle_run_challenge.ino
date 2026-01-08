/*
 * 折返跑挑戰 (Shuttle Run Challenge)
 * 
 * 單元驗收：馬達控制總驗收
 * 
 * 任務目標：
 * 結合前面學到的「直線控制」、「參數調校」、「加減速技巧」，完成一次完美的折返跑。
 * 
 * 流程：
 * 1. [去程]：平滑起步加速 -> 高速行駛 -> 減速煞車停在終點線。
 * 2. [轉身]：原地旋轉 180 度 (U-Turn)。
 * 3. [回程]：平滑起步加速 -> 高速行駛 -> 減速煞車停在起點線。
 * 
 * 你需要挑戰的點：
 * 1. 距離控制：調整加速與減速的迴圈次數，讓車子剛好跑這段距離 (例如 2 公尺)。
 * 2. 轉向精準度：調整 180 度迴轉的時間參數，確保車子轉得正，不會歪掉。
 */

#define MOTOR_L_B_IB 2
#define MOTOR_L_B_IA 3
#define MOTOR_R_A_IB 4
#define MOTOR_R_A_IA 5

// --- 參數區 (請依實際場地調整) ---
const int TOP_SPEED = 255;
const int TURN_TIME_180 = 900; // 迴轉 180 度所需時間
const int CRUISE_TIME = 1000;  // 全速衝刺的時間

// --- 直線修正參數 (Straight Line Calibration) ---
// 真實的馬達通常左右轉速不同，導致車子無法走直線。
// 如果車子偏左 (代表右輪比較快)，請把 RIGHT_FACTOR 調小 (例如 0.9)。
// 如果車子偏右 (代表左輪比較快)，請把 LEFT_FACTOR 調小。
// 範圍：0.0 (不動) ~ 1.0 (全速)
const float LEFT_FACTOR = 1.0; 
const float RIGHT_FACTOR = 1.0; 

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  
  delay(2000); // 準備時間
}

void loop() {
  // 1. 去程
  Serial.println("Phase 1: Go!");
  softStart();        // 平滑起步
  forward(TOP_SPEED); // 確保全速
  delay(CRUISE_TIME); // 維持高速一段時間
  softStop();         // 減速煞車
  
  delay(1000); // 休息一下，消除慣性

  // 2. 轉身
  Serial.println("Phase 2: U-Turn!");
  spinLeft(200);      // 原地旋轉
  delay(TURN_TIME_180);
  brake();
  
  delay(1000); // 準備衝刺回程

  // 3. 回程
  Serial.println("Phase 3: Return!");
  softStart();
  forward(TOP_SPEED);
  delay(CRUISE_TIME);
  softStop();
  
  Serial.println("Mission Complete.");
  
  // 任務結束，永久停止
  while(1) {
    brake();
  }
}

// --- 功能函式 ---

// 平滑起步 (Ramp Up)
void softStart() {
  for (int s = 60; s <= TOP_SPEED; s += 5) {
    forward(s);
    delay(20); // 調整這裡可以改變加速度
  }
}

// 平滑煞車 (Ramp Down)
void softStop() {
  for (int s = TOP_SPEED; s >= 0; s -= 10) { // 煞車通常比加速快一點
    forward(s);
    delay(20);
  }
  brake();
}

// 基礎馬達控制
void forward(int v) {
  // 應用校正參數
  int vL = v * LEFT_FACTOR;
  int vR = v * RIGHT_FACTOR;
  
  digitalWrite(MOTOR_L_B_IB, HIGH); analogWrite(MOTOR_L_B_IA, 255 - vL);
  digitalWrite(MOTOR_R_A_IB, HIGH); analogWrite(MOTOR_R_A_IA, 255 - vR);
}
void spinLeft(int v) {
  digitalWrite(MOTOR_L_B_IB, LOW); analogWrite(MOTOR_L_B_IA, v);
  digitalWrite(MOTOR_R_A_IB, HIGH); analogWrite(MOTOR_R_A_IA, 255 - v);
}
void brake() {
  digitalWrite(MOTOR_L_B_IB, LOW); digitalWrite(MOTOR_L_B_IA, LOW);
  digitalWrite(MOTOR_R_A_IB, LOW); digitalWrite(MOTOR_R_A_IA, LOW);
}

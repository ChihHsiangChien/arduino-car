/*
 * 畫五角星 (Star Path Demo)
 * 
 * 教學目標：
 * 1. 進階開迴路控制：挑戰更複雜的幾何圖形。
 * 2. 幾何計算：五角星 (Pentagram) 的每個頂點轉角是多少？
 *    - 答案是 144 度。
 *    - 計算方式：五角星的一筆畫其實是繞了兩圈 (360 * 2 = 720 度)。
 *    - 720 / 5 = 144 度。
 * 
 * 任務：讓車子走出一筆畫的五角星。
 * 
 * 參數挑戰：
 * 跟正方形的 90 度不同，這次要轉 144 度，時間常數需要重新調整。
 * 數學估算：假設 90 度需要 450ms，那 144 度大約需要 450 * (144/90) = 720ms。
 * 實際值仍需視地面摩擦力微調。
 */

#define MOTOR_L_B_IB 2
#define MOTOR_L_B_IA 3
#define MOTOR_R_A_IB 4
#define MOTOR_R_A_IA 5

const int SPEED = 255;
const int MOVE_TIME = 1000; // 邊長時間
const int TURN_TIME = 720;  // 144 度轉彎時間 (預設值)

// --- 直線校正參數 (請依 straight_line_calibration.ino 結果修改) ---
const float LEFT_FACTOR = 1.0; 
const float RIGHT_FACTOR = 1.0;

void setup() {
  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  
  // 啟動後等待 2 秒讓使用者把車放好
  delay(2000); 
}

void loop() {
  // 五角星有 5 個邊
  for(int i=0; i<5; i++) {
    forward(SPEED);
    delay(MOVE_TIME); // 畫一條線
    
    brake();
    delay(300);       // 停頓一下消除慣性
    
    spinRight(SPEED);
    delay(TURN_TIME); // 轉彎 144 度 (銳角轉彎)
    
    brake();
    delay(300);       // 停頓
  }

  // 畫完收工，永久停止
  while(1) {
    brake();
  }
}

void forward(int v) {
  int vL = v * LEFT_FACTOR;
  int vR = v * RIGHT_FACTOR;

  digitalWrite(MOTOR_L_B_IB, HIGH);
  analogWrite(MOTOR_L_B_IA, 255 - vL);
  digitalWrite(MOTOR_R_A_IB, HIGH);
  analogWrite(MOTOR_R_A_IA, 255 - vR);
}

void spinRight(int v) {
  digitalWrite(MOTOR_L_B_IB, HIGH);
  analogWrite(MOTOR_L_B_IA, 255 - v);
  digitalWrite(MOTOR_R_A_IB, LOW);
  analogWrite(MOTOR_R_A_IA, v);
}

void brake() {
  digitalWrite(MOTOR_L_B_IB, LOW);
  digitalWrite(MOTOR_L_B_IA, LOW);
  digitalWrite(MOTOR_R_A_IB, LOW);
  digitalWrite(MOTOR_R_A_IA, LOW);
}

/*
 * 畫正方形 (Square Path Demo)
 * 
 * 教學目標：
 * 1. 學習「開迴路控制 (Open-Loop Control)」：預先寫好腳本，不管環境如何，車子照著劇本走。
 * 2. 練習調整參數：因為沒有編碼器 (Encoder) 或陀螺儀，你需要不斷測試並手動修改 delay 的時間，
 *    才能讓車子轉出完美的 90 度直角。
 * 
 * 任務：讓車子走一個正方形路徑，最後回到原點停車。
 */

#define MOTOR_L_B_IB 2
#define MOTOR_L_B_IA 3
#define MOTOR_R_A_IB 4
#define MOTOR_R_A_IA 5

const int SPEED = 200;
const int MOVE_TIME = 1000; // 前進 1 秒 (依速度不同代表不同距離)
const int TURN_TIME = 450;  // 轉彎時間 (這就是你要調整的參數，試著讓它剛好轉90度)

void setup() {
  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  
  // 等待 2 秒讓使用者把車放好
  delay(2000); 
}

void loop() {
  // 走四邊形 (重複 4 次)
  for(int i=0; i<4; i++) {
    forward(SPEED);
    delay(MOVE_TIME); // 前進邊長
    
    brake();
    delay(200);       // 停頓一下消除慣性，讓動作更精準
    
    spinRight(SPEED);
    delay(TURN_TIME); // 轉彎 90 度
    
    brake();
    delay(200);       // 停頓
  }

  // 走完一圈，永久停止
  while(1) {
    brake();
  }
}

void forward(int v) {
  digitalWrite(MOTOR_L_B_IB, HIGH);
  analogWrite(MOTOR_L_B_IA, 255 - v);
  digitalWrite(MOTOR_R_A_IB, HIGH);
  analogWrite(MOTOR_R_A_IA, 255 - v);
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

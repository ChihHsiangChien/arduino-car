/*
 * 紅外線循線車 (Line Following Car)
 * 
 * 任務目標：
 * 讓車子沿著地面上的黑線行走。
 * 
 * 硬體設置：
 * 1. 兩個紅外線感測器 (原本的避障感測器) 朝下安裝。
 * 2. 左感測器：接腳 6
 * 3. 右感測器：接腳 7
 * 
 * 原理：
 * - 白色地面：反射紅外線 -> 感測器輸出 LOW (0)
 * - 黑色線條：吸收紅外線 -> 感測器輸出 HIGH (1)
 * 
 * 邏輯表：
 * 左(6) | 右(7) | 狀態       | 動作
 * -----|-------|------------|------------
 * LOW  | LOW   | 都在白地   | 直走 (Forward) (假設線在中間)
 * HIGH | LOW   | 左邊踩線   | 左轉 (Turn Left)
 * LOW  | HIGH  | 右邊踩線   | 右轉 (Turn Right)
 * HIGH | HIGH  | 都在黑線   | 停止 (Stop) (或是十字路口)
 */

// --- 硬體接腳定義 ---
#define MOTOR_L_B_IB 2    // 左輪 B-IB
#define MOTOR_L_B_IA 3    // 左輪 B-IA (PWM)
#define MOTOR_R_A_IB 4    // 右輪 A-IB
#define MOTOR_R_A_IA 5    // 右輪 A-IA (PWM)

#define IR_LINE_L 6       // 左側循線感測器
#define IR_LINE_R 7       // 右側循線感測器

// --- 速度設定 ---
// 循線通常不需要太快，以免衝出跑道
const int DRIVE_SPEED = 150; 
const int TURN_SPEED = 180;

void setup() {
  Serial.begin(115200);

  // 初始化馬達
  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  
  // 初始化感測器
  pinMode(IR_LINE_L, INPUT);
  pinMode(IR_LINE_R, INPUT);
  
  Serial.println("Line Following Car Initialized.");
  delay(1000); // 等待1秒準備
}

void loop() {
  // 1. 讀取感測器狀態
  // HIGH = 黑線 (吸收), LOW = 白地 (反射)
  int valL = digitalRead(IR_LINE_L);
  int valR = digitalRead(IR_LINE_R);
  
  // 為了除錯方便，印出狀態 (正式跑的時候可以註解掉以免拖慢速度)
  // Serial.print("L:"); Serial.print(valL);
  // Serial.print(" R:"); Serial.println(valR);

  // 2. 判斷邏輯
  if (valL == LOW && valR == LOW) {
    // [白, 白] -> 線在中間 -> 直走
    forward(DRIVE_SPEED);
  }
  else if (valL == HIGH && valR == LOW) {
    // [黑, 白] -> 左邊感測器碰到線 -> 車身偏右了 -> 向左修正
    spinLeft(TURN_SPEED);
  }
  else if (valL == LOW && valR == HIGH) {
    // [白, 黑] -> 右邊感測器碰到線 -> 車身偏左了 -> 向右修正
    spinRight(TURN_SPEED);
  }
  else {
    // [黑, 黑] -> 遇到停止線或十字路口 -> 停止
    brake();
  }
  
  // 循線迴圈不需要 delay，越快反應越好
}

// --- 馬達控制函式 ---
void forward(int v) {
  digitalWrite(MOTOR_L_B_IB, HIGH); analogWrite(MOTOR_L_B_IA, 255 - v);
  digitalWrite(MOTOR_R_A_IB, HIGH); analogWrite(MOTOR_R_A_IA, 255 - v);
}

void spinLeft(int v) {
  digitalWrite(MOTOR_L_B_IB, LOW); analogWrite(MOTOR_L_B_IA, v);
  digitalWrite(MOTOR_R_A_IB, HIGH); analogWrite(MOTOR_R_A_IA, 255 - v);
}

void spinRight(int v) {
  digitalWrite(MOTOR_L_B_IB, HIGH); analogWrite(MOTOR_L_B_IA, 255 - v);
  digitalWrite(MOTOR_R_A_IB, LOW); analogWrite(MOTOR_R_A_IA, v);
}

void brake() {
  digitalWrite(MOTOR_L_B_IB, LOW); digitalWrite(MOTOR_L_B_IA, LOW);
  digitalWrite(MOTOR_R_A_IB, LOW); digitalWrite(MOTOR_R_A_IA, LOW);
}

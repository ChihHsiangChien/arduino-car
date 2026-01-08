/*
 * 線性加速 (Smooth Acceleration Demo)
 * 
 * 教學目標：
 * 1. 理解 PWM (Pulse Width Modulation) 與馬達轉速的關係。
 * 2. 認識「啟動死區 (Dead Zone)」：PWM 不是給 1 馬達就會轉，通常需要給到 60~100 之間才能克服靜摩擦力。
 * 3. 實作「平滑起步」與「緩減速」：這能保護齒輪箱，並讓行駛質感更高級。
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
  for (int speed = MIN_POWER; speed <= MAX_POWER; speed += 5) {
    // 若速度太低無法讓馬達轉動，我們可以直接跳過死區，或者就讓它慢慢嗡嗡響
    // 這裡示範簡單的線性增加
    forward(speed);
    Serial.print("Speed: "); Serial.println(speed);
    delay(50); // 每 50ms 增加一點速度
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

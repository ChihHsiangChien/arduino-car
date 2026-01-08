/*
 * 基礎避障 (Simple Avoidance)
 * 
 * 教學目標：
 * 1. 學習自動控制最核心的邏輯：「感測 -> 判斷 -> 動作」。
 * 2. 這是最單純的自駕車程式：沒有音效、沒有複雜的狀態機、沒有非阻塞程式碼。
 * 3. 使用 `delay()` (阻塞式) 來處理轉彎，雖然會讓程式暫停，但對於初學者來說邏輯最直觀。
 * 
 * 邏輯：
 * - 只要前方 20cm 內有東西 -> 停車 -> 後退 -> 轉彎。
 * - 否則 -> 一直前進。
 */

#define MOTOR_L_B_IB 2
#define MOTOR_L_B_IA 3
#define MOTOR_R_A_IB 4
#define MOTOR_R_A_IA 5
#define ULTRASONIC_TRIG 8
#define ULTRASONIC_ECHO 9

const int DRIVE_SPEED = 180;
const int TURN_SPEED = 200;

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  
  // 啟動後等待 2 秒讓使用者放開手
  delay(2000);
}

void loop() {
  float distance = getDistance();
  Serial.print("Distance: "); Serial.println(distance);

  // 簡單的二分法邏輯
  if (distance < 20) {
    // --- 避障流程 (Blocking) ---
    // 程式執行到這裡會「卡住」直到動作完成，這是最簡單的寫法
    
    Serial.println("Obstacle! Stopping.");
    brake();
    delay(200); // 停頓
    
    Serial.println("Backing up...");
    backward(DRIVE_SPEED);
    delay(500); // 後退 0.5 秒
    
    Serial.println("Turning...");
    spinLeft(TURN_SPEED);
    delay(400); // 轉彎 0.4 秒
    
    brake();
    delay(200); // 再次停頓確認安全
    
  } else {
    // --- 正常行駛 ---
    forward(DRIVE_SPEED);
  }
  
  delay(50); // 避免感測器讀取太快
}

float getDistance() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 30000); 
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

void forward(int v) {
  digitalWrite(MOTOR_L_B_IB, HIGH); analogWrite(MOTOR_L_B_IA, 255 - v);
  digitalWrite(MOTOR_R_A_IB, HIGH); analogWrite(MOTOR_R_A_IA, 255 - v);
}
void backward(int v) {
  digitalWrite(MOTOR_L_B_IB, LOW); analogWrite(MOTOR_L_B_IA, v);
  digitalWrite(MOTOR_R_A_IB, LOW); analogWrite(MOTOR_R_A_IA, v);
}
void spinLeft(int v) {
  digitalWrite(MOTOR_L_B_IB, LOW); analogWrite(MOTOR_L_B_IA, v);
  digitalWrite(MOTOR_R_A_IB, HIGH); analogWrite(MOTOR_R_A_IA, 255 - v);
}
void brake() {
  digitalWrite(MOTOR_L_B_IB, LOW); digitalWrite(MOTOR_L_B_IA, LOW);
  digitalWrite(MOTOR_R_A_IB, LOW); digitalWrite(MOTOR_R_A_IA, LOW);
}

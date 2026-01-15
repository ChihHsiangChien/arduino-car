/*
 * 跟隨車 (Follow Me Car)
 * 
 * 教學目標：
 * 1. 學習「回授控制 (Feedback Control)」的基礎觀念。
 * 2. 與「避障」邏輯相反：避障是看到東西要跑，跟隨是看到東西要追。
 * 3. 實作「保持距離」的行為：太遠要追，太近要退。
 * 
 * 行為邏輯：
 * - 距離 > 20cm 且 < 40cm: 前進 (追)
 * - 距離 < 15cm: 後退 (躲)
 * - 距離在 15~20cm 之間: 停止 (保持社交距離)
 * - 距離 > 50cm: 視為目標丟失，停止動作
 */

#define MOTOR_L_B_IB 2
#define MOTOR_L_B_IA 3
#define MOTOR_R_A_IB 4
#define MOTOR_R_A_IA 5
#define ULTRASONIC_TRIG 8
#define ULTRASONIC_ECHO 9
#define BUZZER_PIN 11

const int MAX_SPEED = 180;
const int MIN_SPEED = 130;

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_L_B_IB, OUTPUT);
  pinMode(MOTOR_L_B_IA, OUTPUT);
  pinMode(MOTOR_R_A_IB, OUTPUT);
  pinMode(MOTOR_R_A_IA, OUTPUT);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  Serial.println("Follow Me Car Start!");
  tone(BUZZER_PIN, 500, 100);
}

void loop() {
  float distance = getDistance();
  Serial.print("Target Dist: "); Serial.println(distance);

  if (distance > 20 && distance < 50) {
    // --- 追蹤模式 ---
    // 距離越遠，跑越快 (簡單的 P-Control 概念)
    int speed = map(distance, 20, 50, MIN_SPEED, MAX_SPEED);
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);
    
    Serial.print("Chase! Speed: "); Serial.println(speed);
    forward(speed);
  } 
  else if (distance > 1 && distance < 15) {
    // --- 後退模式 (太近了) ---
    Serial.println("Too Close! Backing up.");
    backward(150);
  }
  else {
    // --- 停止模式 ---
    // 包含: 距離剛好 (15-20cm) 或是 目標丟失 (>50cm)
    brake();
  }
  
  delay(100); // 簡單調節採樣率
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
  digitalWrite(MOTOR_L_B_IB, HIGH);
  analogWrite(MOTOR_L_B_IA, 255 - v);
  digitalWrite(MOTOR_R_A_IB, HIGH);
  analogWrite(MOTOR_R_A_IA, 255 - v);
}

void backward(int v) {
  digitalWrite(MOTOR_L_B_IB, LOW);
  analogWrite(MOTOR_L_B_IA, v);
  digitalWrite(MOTOR_R_A_IB, LOW);
  analogWrite(MOTOR_R_A_IA, v);
}

void brake() {
  digitalWrite(MOTOR_L_B_IB, LOW);
  digitalWrite(MOTOR_L_B_IA, LOW);
  digitalWrite(MOTOR_R_A_IB, LOW);
  digitalWrite(MOTOR_R_A_IA, LOW);
}

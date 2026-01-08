/*
 * 序列繪圖儀儀表板 (Sensor Serial Plotter Dashboard)
 * 
 * 教學目標：
 * 學會使用 Arduino IDE 的強大工具：「序列繪圖儀 (Serial Plotter)」。
 * 透過圖形化介面，我們可以清楚看到感測器數值的波動、雜訊以及紅外線的觸發時機。
 * 
 * 使用方法：
 * 1. 上傳此程式。
 * 2. 在 Arduino IDE 上方選單點選「工具 (Tools)」 -> 「序列繪圖儀 (Serial Plotter)」。
 *    (注意：必須先關閉原本的文字型 Serial Monitor，兩者不能同時開)
 * 3. 調整鮑率 (Baud Rate) 為 115200。
 * 
 * 你會看到：
 * - 藍線：超音波距離
 * - 紅線/綠線：左右紅外線狀態 (0 或 100)
 */

#define IR_AVOID_L 6
#define IR_AVOID_R 7
#define ULTRASONIC_TRIG 8
#define ULTRASONIC_ECHO 9

void setup() {
  Serial.begin(115200);
  pinMode(IR_AVOID_L, INPUT);
  pinMode(IR_AVOID_R, INPUT);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
}

void loop() {
  // 1. 讀取數據
  float dist = getDistance();
  // 將紅外線讀數 (0/1) 放大到 0/100 以便在圖表上觀察
  int irLeft = !digitalRead(IR_AVOID_L) * 100; 
  int irRight = !digitalRead(IR_AVOID_R) * 100;

  // 2. 輸出給繪圖儀的特定格式
  // 格式： "變數名1:數值1,變數名2:數值2,..."
  // 這裡我們多加了兩個固定值 Min:0 和 Max:200 來固定圖表的Y軸範圍，避免圖表一直跳動
  Serial.print("Min:0,");
  Serial.print("Max:200,");
  
  Serial.print("Distance:");
  Serial.print(dist);
  Serial.print(",");
  
  Serial.print("IR_Left:");
  Serial.print(irLeft);
  Serial.print(",");
  
  Serial.print("IR_Right:");
  Serial.println(irRight);

  delay(50); // 繪圖更新率
}

float getDistance() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 30000); 
  if (duration == 0) return 0; // 改為0以便繪圖
  float d = duration * 0.034 / 2;
  if (d > 200) d = 200; // 限制最大值以便繪圖美觀
  return d;
}

/*
 * 布萊登堡避障與尋光車 (Braitenberg Vehicles)
 * 
 * 教學目標：
 * 1. 理解布萊登堡車 (Braitenberg Vehicles) 的經典人工生命與控制理論。
 * 2. 學習如何使用雙光敏電阻 (類比輸入 A0 & A1) 進行感測與馬達速度的直接耦合 (Sensor-Motor Coupling)。
 * 3. 實作並比較四種經典車型：
 *    - 車型 2a (膽小 Fear)：同側興奮聯結 (Ipsilateral Excitatory)。避開光源，強光下高速逃跑。
 *    - 車型 2b (侵略 Aggression)：對側興奮聯結 (Contralateral Excitatory)。朝向光源前進，越近跑越快，直到撞上。
 *    - 車型 3a (愛慕 Love)：同側抑制聯結 (Ipsilateral Inhibitory)。朝向光源前進，越近跑越慢，最後停在光源前。
 *    - 車型 3b (探險 Explorer)：對側抑制聯結 (Contralateral Inhibitory)。避開光源，在黑暗處以高速向外探索。
 * 
 * 接線說明：
 * - 左輪馬達：D2 (IB), D3 (IA - PWM)
 * - 右輪馬達：D4 (IB), D5 (IA - PWM)
 * - 左光敏電阻 (L_SENSOR)：A0
 * - 右光敏電阻 (R_SENSOR)：A1
 * 
 * 操作方法：
 * - 燒錄程式後，開啟序列埠監控視窗 (鮑率 9600)
 * - 輸入數字 1, 2, 3, 4 可切換不同的布萊登堡車型行為。
 */

#define MOTOR_L_IB 2
#define MOTOR_L_IA 3
#define MOTOR_R_IB 4
#define MOTOR_R_IA 5

#define L_SENSOR A0
#define R_SENSOR A1

// 預設參數定義
int sensorMin = 1023; // 動態校正最小值
int sensorMax = 0;    // 動態校正最大值
const int BASE_SPEED = 60;   // 基礎啟動死區速度，防止馬達動不起來
const int MAX_SPEED = 200;  // 限制最高速度，防止暴衝

// 車型代號定義
enum VehicleType {
  VEHICLE_2A_FEAR = 1,       // 2a: 膽小鬼 (同側興奮)
  VEHICLE_2B_AGGRESSION = 2, // 2b: 侵略者 (對側興奮)
  VEHICLE_3A_LOVE = 3,       // 3a: 愛慕者 (同側抑制)
  VEHICLE_3B_EXPLORER = 4    // 3b: 探險家 (對側抑制)
};

VehicleType currentMode = VEHICLE_2A_FEAR;

void setup() {
  Serial.begin(9600);
  
  pinMode(MOTOR_L_IB, OUTPUT);
  pinMode(MOTOR_L_IA, OUTPUT);
  pinMode(MOTOR_R_IB, OUTPUT);
  pinMode(MOTOR_R_IA, OUTPUT);
  
  brake();
  
  Serial.println("================================================");
  Serial.println("   布萊登堡尋光避障車 (Braitenberg Vehicles)");
  Serial.println("================================================");
  Serial.println("請開始進行 3 秒鐘的光感測器校正...");
  Serial.println("請使用手電筒照向兩顆光敏電阻，並用手遮擋，以記錄明暗範圍。");
  
  // 校正 3 秒
  unsigned long startCalibrate = millis();
  while (millis() - startCalibrate < 3000) {
    int valL = analogRead(L_SENSOR);
    int valR = analogRead(R_SENSOR);
    
    if (valL < sensorMin) sensorMin = valL;
    if (valR < sensorMin) sensorMin = valR;
    if (valL > sensorMax) sensorMax = valL;
    if (valR > sensorMax) sensorMax = valR;
    delay(50);
  }
  
  // 防呆：如果明暗差距太小，使用預設經驗值
  if (sensorMax - sensorMin < 100) {
    sensorMin = 150;
    sensorMax = 850;
    Serial.println("校正範圍太小，使用預設值：150 ~ 850");
  } else {
    Serial.print("校正完成！讀值範圍: ");
    Serial.print(sensorMin);
    Serial.print(" ~ ");
    Serial.println(sensorMax);
  }
  
  printModeDescription();
}

void loop() {
  // 檢查是否有來自序列埠的切換命令
  if (Serial.available() > 0) {
    char input = Serial.read();
    if (input >= '1' && input <= '4') {
      currentMode = (VehicleType)(input - '0');
      brake();
      printModeDescription();
      delay(500);
    }
  }

  // 讀取左右感測器
  int rawL = analogRead(L_SENSOR);
  int rawR = analogRead(R_SENSOR);

  // 限制感測器數值在校正範圍內
  int valL = constrain(rawL, sensorMin, sensorMax);
  int valR = constrain(rawR, sensorMin, sensorMax);

  int leftSpeed = 0;
  int rightSpeed = 0;

  // 根據不同車型進行感測器-馬達耦合
  switch (currentMode) {
    
    case VEHICLE_2A_FEAR:
      // 同側興奮 (Ipsilateral Excitatory)
      // 左感測器控制左輪，右感測器控制右輪；越亮跑越快
      leftSpeed = map(valL, sensorMin, sensorMax, BASE_SPEED, MAX_SPEED);
      rightSpeed = map(valR, sensorMin, sensorMax, BASE_SPEED, MAX_SPEED);
      break;

    case VEHICLE_2B_AGGRESSION:
      // 對側興奮 (Contralateral Excitatory)
      // 左感測器控制右輪，右感測器控制左輪；越亮跑越快
      leftSpeed = map(valR, sensorMin, sensorMax, BASE_SPEED, MAX_SPEED);
      rightSpeed = map(valL, sensorMin, sensorMax, BASE_SPEED, MAX_SPEED);
      break;

    case VEHICLE_3A_LOVE:
      // 同側抑制 (Ipsilateral Inhibitory)
      // 左感測器控制左輪，右感測器控制右輪；越亮跑越慢
      leftSpeed = map(valL, sensorMin, sensorMax, MAX_SPEED, 0);
      rightSpeed = map(valR, sensorMin, sensorMax, MAX_SPEED, 0);
      break;

    case VEHICLE_3B_EXPLORER:
      // 對側抑制 (Contralateral Inhibitory)
      // 左感測器控制右輪，右感測器控制左輪；越亮跑越慢
      leftSpeed = map(valR, sensorMin, sensorMax, MAX_SPEED, 0);
      rightSpeed = map(valL, sensorMin, sensorMax, MAX_SPEED, 0);
      break;
  }

  // 驅動馬達
  driveMotors(leftSpeed, rightSpeed);

  // 輸出除錯資訊
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 300) {
    Serial.print("Sensors - L: "); Serial.print(rawL);
    Serial.print(" | R: "); Serial.print(rawR);
    Serial.print("  ==>  Speeds - L: "); Serial.print(leftSpeed);
    Serial.print(" | R: "); Serial.println(rightSpeed);
    lastPrint = millis();
  }

  delay(20);
}

// 驅動馬達的核心控制函式
void driveMotors(int leftSpeed, int rightSpeed) {
  // 左輪控制
  if (leftSpeed >= 0) {
    digitalWrite(MOTOR_L_IB, HIGH);
    analogWrite(MOTOR_L_IA, 255 - leftSpeed);
  } else {
    digitalWrite(MOTOR_L_IB, LOW);
    analogWrite(MOTOR_L_IA, -leftSpeed);
  }

  // 右輪控制
  if (rightSpeed >= 0) {
    digitalWrite(MOTOR_R_IB, HIGH);
    analogWrite(MOTOR_R_IA, 255 - rightSpeed);
  } else {
    digitalWrite(MOTOR_R_IB, LOW);
    analogWrite(MOTOR_R_IA, -rightSpeed);
  }
}

// 煞車
void brake() {
  digitalWrite(MOTOR_L_IB, LOW);
  digitalWrite(MOTOR_L_IA, LOW);
  digitalWrite(MOTOR_R_IB, LOW);
  digitalWrite(MOTOR_R_IA, LOW);
}

// 列印目前模式的中文說明
void printModeDescription() {
  Serial.println("\n------------------------------------------------");
  Serial.print("目前模式: ");
  switch (currentMode) {
    case VEHICLE_2A_FEAR:
      Serial.println("【車型 2a - 膽小鬼 (Fear)】");
      Serial.println("聯結方式: 同側興奮 (Ipsilateral Excitatory)");
      Serial.println("行為特性: 討厭光線。哪側有光就往另一側轉彎逃避；如果強光直射，會以全速逃離！");
      break;
    case VEHICLE_2B_AGGRESSION:
      Serial.println("【車型 2b - 侵略者 (Aggression)】");
      Serial.println("聯結方式: 對側興奮 (Contralateral Excitatory)");
      Serial.println("行為特性: 喜好光線。哪側有光就朝哪側轉身；越亮衝得越快，像在發動攻擊。");
      break;
    case VEHICLE_3A_LOVE:
      Serial.println("【車型 3a - 愛慕者 (Love)】");
      Serial.println("聯結方式: 同側抑制 (Ipsilateral Inhibitory)");
      Serial.println("行為特性: 渴望溫暖與光芒。會默默轉身靠近光源，接近光源時速度減慢，最終停在光前守候。");
      break;
    case VEHICLE_3B_EXPLORER:
      Serial.println("【車型 3b - 探險家 (Explorer)】");
      Serial.println("聯結方式: 對側抑制 (Contralateral Inhibitory)");
      Serial.println("行為特性: 喜愛未知黑暗。在強光下會轉身逃避，且在越黑暗的地方探索速度越快。");
      break;
  }
  Serial.println("選單: 輸入 1 (Fear), 2 (Aggression), 3 (Love), 4 (Explorer) 切換車型");
  Serial.println("------------------------------------------------\n");
}

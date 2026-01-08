#include <IRremote.hpp>

int carSpeed = 150; // Initial speed

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(10, ENABLE_LED_FEEDBACK);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  pinMode(11, OUTPUT);  
}

void loop() {
  if (IrReceiver.decode()) {
    // Only act if we get a valid code (ignoring 0 which sometimes happens on error/repeat with some libs)
    if (IrReceiver.decodedIRData.decodedRawData != 0) {
       irControlCar();
    }
    IrReceiver.resume(); // Receive the next value
  }
}

void irControlCar() {
  uint32_t rawData = IrReceiver.decodedIRData.decodedRawData;
  // Serial.println(rawData); // Debug

  switch (rawData) {
    case 16738455:
      Serial.println("button 0");
      break;

    case 16724175:
      Serial.println("button 1");
      break;

    case 16718055:
      Serial.println("button 2");
      forward(carSpeed);
      break;

    case 16743045:
      Serial.println("button 3");
      break;

    case 16716015:
      Serial.println("button 4");
      spinLeft();
      break;

    case 16726215:
      Serial.println("button 5");
      brake();
      break;

    case 16734885:
      Serial.println("button 6");
      spinRight();
      break;

    case 16728765:
      Serial.println("button 7");
      break;

    case 16730805:
      Serial.println("button 8");
      backward(carSpeed);
      break;

    case 16732845:
      Serial.println("button 9");
      break;

    case 16750695:
      Serial.println("button 100+");
      break;

    case 16756815:
      Serial.println("button 200+");
      break;

    case 16769055:
      Serial.println("button -");
      carSpeed -= 20;
      if (carSpeed < 0) carSpeed = 0;
      Serial.print("Speed: ");
      Serial.println(carSpeed);
      break;

    case 16754775:
      Serial.println("button +");
      carSpeed += 20;
      if (carSpeed > 255) carSpeed = 255;
      Serial.print("Speed: ");
      Serial.println(carSpeed);
      break;

    case 16748655:
      Serial.println("button EQ");
      break;

    case 16753245:
      Serial.println("PREV");
      break;

    case 16736925:
      Serial.println("NEXT");
      break;

    case 16769565:
      Serial.println("PLATPAUSE");
      break;

    case 16720605:
      Serial.println("VOL-");
      break;

    case 16712445:
      Serial.println("VOL-");
      break;

    case 16761405:
      Serial.println("END");
      break;

    default:
      break;
  }
}


void forward() {
  // 前進 full speed
  forward(255);
}

void forward(int v) {
  // 前進
  digitalWrite(2, HIGH);
  analogWrite(3, 255 - v);
  digitalWrite(4, HIGH);
  analogWrite(5, 255 - v);
}

void backward() {
  // 後退 full speed
  backward(255);
}

void backward(int v) {
  // 後退
  digitalWrite(2, LOW);
  analogWrite(3, v);
  digitalWrite(4, LOW);
  analogWrite(5, v);
}

void turnRight() {
  // 右轉 full
  turnRight(255);
}

void turnRight(int v) {
  // 右轉
  digitalWrite(2, HIGH);
  analogWrite(3, 255 - v);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW); 
}

void turnLeft() {
  // 左轉 full
  turnLeft(255);
}

void turnLeft(int v) {
  // 左轉
  digitalWrite(2, LOW);
  digitalWrite(3, LOW); 
  digitalWrite(4, HIGH);
  analogWrite(5, 255 - v); 
}

void brake() {
  // 煞車
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
}

void spinRight() {
  // 右旋: Left Forward, Right Backward
  digitalWrite(2, HIGH);
  digitalWrite(3, LOW);  // Left Forward Full
  digitalWrite(4, LOW);
  digitalWrite(5, HIGH); // Right Backward Full
}

void spinLeft() {
  // 左旋: Left Backward, Right Forward
  digitalWrite(2, LOW);
  digitalWrite(3, HIGH); // Left Backward Full
  digitalWrite(4, HIGH);
  digitalWrite(5, LOW);  // Right Forward Full
}

void newtone(int t, int f, int d) {
  int p = 1000000L / f;
  int pulse = p / 2;
  for (long i = 0; i < d * 1000L; i += p) {
    digitalWrite(t, HIGH);
    delayMicroseconds(pulse);
    digitalWrite(t, LOW);
    delayMicroseconds(pulse);
  }
}

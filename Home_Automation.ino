#include <Servo.h>
#include <SoftwareSerial.h>

#define BT_RX 10
#define BT_TX 11

SoftwareSerial bt(BT_RX, BT_TX);
Servo lockServo;

volatile int dimming = 64; 
int triacPin = 9;
int zeroCrossPin = 2;

void setup() {
  pinMode(triacPin, OUTPUT);
  pinMode(zeroCrossPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(zeroCrossPin), zeroCrossDetected, RISING);

  lockServo.attach(6);
  lockServo.write(0); 

  bt.begin(9600);
  Serial.begin(9600);
}

void loop() {
  if (bt.available()) {
    char cmd = bt.read();
    int val = -1;

    
    if (bt.available()) {
      val = bt.read() - '0'; 
    }

    if (cmd == 'L' && val >= 0 && val <= 9) {
      dimming = map(val, 0, 9, 128, 0); 
      Serial.print("Light Level: "); Serial.println(val);
    }
    else if (cmd == 'F' && val >= 0 && val <= 9) {
      dimming = map(val, 0, 9, 128, 0);
      Serial.print("Fan Speed: "); Serial.println(val);
    }
    else if (cmd == 'D') {
      if (val == 1) {
        lockServo.write(90); 
        Serial.println("Door Locked");
      } else if (val == 0) {
        lockServo.write(0);  
        Serial.println("Door Unlocked");
      }
    }
  }
}

void zeroCrossDetected() {
  int delayTime = (75 * (128 - dimming)) / 128;
  delayMicroseconds(delayTime);
  digitalWrite(triacPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triacPin, LOW);
}

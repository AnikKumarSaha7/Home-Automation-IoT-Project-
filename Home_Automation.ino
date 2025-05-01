#include <Servo.h>
#include <SoftwareSerial.h>
#include <Keypad.h>
#define BT_RX 10
#define BT_TX 11
SoftwareSerial bt(BT_RX, BT_TX);
Servo lockServo;
volatile int dimming = 64;
int triacPin = 9;
int zeroCrossPin = 2;
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {3, 4, 5, 6};
byte colPins[COLS] = {7, 8, A0, A1};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
String inputPassword = "";
String correctPassword = "1234"; 
bool isLocked = true;

void setup() {
  pinMode(triacPin, OUTPUT);
  pinMode(zeroCrossPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(zeroCrossPin), zeroCrossDetected, RISING);
  lockServo.attach(6);
  lockDoor(); 
  bt.begin(9600);
  Serial.begin(9600);
}

void loop() {
  handleBluetooth();
  handleKeypad();
}

void handleBluetooth() {
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
  }
}

void handleKeypad() {
  char key = keypad.getKey();
  if (key) {
    if (key == '#') {
      if (inputPassword == correctPassword) {
        unlockDoor();
        Serial.println("Correct Password - Door Unlocked");
      } else {
        lockDoor();
        Serial.println("Wrong Password - Door Locked");
      }
      inputPassword = ""; 
    }
    else if (key == '*') {
      inputPassword = ""; 
      Serial.println("Input Cleared");
    }
    else {
      inputPassword += key;
      Serial.print("Entered: "); Serial.println(inputPassword);
    }
  }
}

void lockDoor() {
  lockServo.write(90);
  isLocked = true;
}

void unlockDoor() {
  lockServo.write(0);
  isLocked = false;
}

void zeroCrossDetected() {
  int delayTime = (75 * (128 - dimming)) / 128;
  delayMicroseconds(delayTime);
  digitalWrite(triacPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triacPin, LOW);
}

#include "Arduino.h"
#include "Wire.h" // เรียกใช้งาน Library Wire
#include "LiquidCrystal_I2C.h" // เรียกใช้งาน Library LiquidCrystal_I2C

LiquidCrystal_I2C lcd(0x27, 16, 2);

IRAM_ATTR void startStop();
IRAM_ATTR void resetTimer();

int START_STOP_BUTTON = D3;  
int RESET_BUTTON = D4;
int LED_PIN = D5;  

unsigned long startTime = 0;
unsigned long elapsedTime = 0;
unsigned long previousSecond = 0;
bool isRunning = false;
bool ledState = LOW;

void displayTime(unsigned long time);
void toggleLED();

int state;
const int  displayTimer = 0;

void setup() {

  pinMode(START_STOP_BUTTON, INPUT_PULLUP);
  pinMode(RESET_BUTTON, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  lcd.init(); // กำหนดให้ LCD ทำงาน
  lcd.backlight(); // เปิดไฟหน้าจอ LCD

  // กำหนดการตอบสนองแบบ interrupt สำหรับขาดิจิตอลที่ระบุโดยค่าคงที่(ในกรณีนี้คือเมื่อสัญญาณเปลี่ยนจาก HIGH เป็น LOW ซึ่งระบุโดย FALLING)
  attachInterrupt(digitalPinToInterrupt(START_STOP_BUTTON), startStop, FALLING);
  attachInterrupt(digitalPinToInterrupt(RESET_BUTTON), resetTimer, FALLING);

  state = displayTimer;
}

void loop() {

  if (state == displayTimer){ 
    lcd.setCursor(3, 0);
    lcd.print("Stopwatch");
    if (isRunning) {  //ตรวจสอบว่านาฬิกากำลังทำงานอยู่หรือไม่
      elapsedTime = millis() - startTime; //คำนวณเวลาที่ผ่านไปตั้งแต่เวลาเริ่มต้นแล้ว ดังนั้น elapsedTime จะเป็นผลต่างระหว่าง millis() กับ startTime  
      unsigned long seconds = (elapsedTime / 1000) % 60; // คำนวณวินาที
      if (seconds != previousSecond) { // ตรวจสอบว่าเวลาผ่านไป 1 วินาทีหรือไม่
        previousSecond = seconds;
        toggleLED(); // เรียกใช้ฟังก์ชันเพื่อเปลี่ยนสถานะ LED
      }
    }

    displayTime(elapsedTime); //ใช้เพื่อแสดงเวลาที่ผ่านไปบนจอ LCD
  }
}

void displayTime(unsigned long time) {
  lcd.setCursor(3, 1);
  unsigned long minutes = (time / 60000) % 60; // หารเพื่อหานาที
  unsigned long seconds = (time / 1000) % 60; // หารเพื่อหาวินาที
  unsigned long milliseconds = time % 1000; // หารเพื่อหามิลลิวินาที

  // สร้างข้อความที่จะแสดงบนจอ LCD ในรูปแบบ HH:MM:SS.mmm
  char buffer[10]; // สร้างตัวแปร buffer ขนาด 10 สำหรับเก็บข้อความ
  sprintf(buffer, "%02lu:%02lu:%03lu", minutes, seconds, milliseconds); // ใช้ sprintf() เพื่อจัดรูปแบบข้อความเป็นเลขฐาน 10

  lcd.print(buffer); // แสดงข้อความที่มีรูปแบบเป็นเลขฐาน 10 บนจอ LCD
}

void toggleLED() {
  ledState = !ledState; // สลับสถานะของ LED
  digitalWrite(LED_PIN, ledState); // เปลี่ยนสถานะของ LED ตามสถานะที่กำหนด
}

IRAM_ATTR void startStop() {
  if (!isRunning) {
    startTime = millis() - elapsedTime; // บันทึกเวลาที่เริ่มต้นการจับเวลาโดยใช้ค่า millis() ปัจจุบันลบด้วยเวลาที่ผ่านไป (elapsedTime)
    isRunning = true; // เปลี่ยนสถานะการทำงานของจับเวลาเป็นเป็นเริ่มทำงาน
  } else {
    isRunning = false; // เปลี่ยนสถานะการทำงานของจับเวลาเป็นไม่ทำงาน
  }
}

IRAM_ATTR void resetTimer() {
  elapsedTime = 0; // รีเซ็ตค่าเวลาที่ผ่านไปให้เป็น 0
  isRunning = false;
  digitalWrite(LED_PIN, LOW); // เมื่อรีเซ็ตนาฬิกา ก็ปิด LED
}
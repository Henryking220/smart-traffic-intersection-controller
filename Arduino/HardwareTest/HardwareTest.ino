#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int A_RED = 2;
const int A_YELLOW = 3;
const int A_GREEN = 4;
const int B_RED = 5;
const int B_YELLOW = 6;
const int B_GREEN = 7;
const int BUZZER = 8;

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {9, 10, 11, 12};
byte colPins[COLS] = {13, A0, A1, A2};

Keypad keypad = Keypad(
  makeKeymap(keys),
  rowPins,
  colPins,
  ROWS,
  COLS
);

void setup() {
  pinMode(A_RED, OUTPUT);
  pinMode(A_YELLOW, OUTPUT);
  pinMode(A_GREEN, OUTPUT);
  pinMode(B_RED, OUTPUT);
  pinMode(B_YELLOW, OUTPUT);
  pinMode(B_GREEN, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  lcd.init();
  lcd.backlight();

  Serial.begin(9600);

  lcd.clear();
  lcd.print("TRAFFIC SYSTEM");
  lcd.setCursor(0, 1);
  lcd.print("HARDWARE TEST");

  delay(2000);

  lcd.clear();
  lcd.print("TEST: RED");
  digitalWrite(A_RED, HIGH);
  digitalWrite(B_RED, HIGH);
  delay(2000);
  digitalWrite(A_RED, LOW);
  digitalWrite(B_RED, LOW);

  lcd.clear();
  lcd.print("TEST: YELLOW");
  digitalWrite(A_YELLOW, HIGH);
  digitalWrite(B_YELLOW, HIGH);
  delay(2000);
  digitalWrite(A_YELLOW, LOW);
  digitalWrite(B_YELLOW, LOW);

  lcd.clear();
  lcd.print("TEST: GREEN");
  digitalWrite(A_GREEN, HIGH);
  digitalWrite(B_GREEN, HIGH);
  delay(2000);
  digitalWrite(A_GREEN, LOW);
  digitalWrite(B_GREEN, LOW);

  lcd.clear();
  lcd.print("TEST: BUZZER");
  tone(BUZZER, 1000);
  delay(1000);
  noTone(BUZZER);

  lcd.clear();
  lcd.print("KEYPAD TEST");
  lcd.setCursor(0, 1);
  lcd.print("PRESS ANY KEY");

  Serial.println("=== TRAFFIC HARDWARE TEST ===");
  Serial.println("Press any keypad key.");
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    Serial.print("KEY PRESSED: ");
    Serial.println(key);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("KEY PRESSED:");
    lcd.setCursor(0, 1);
    lcd.print(key);
  }
}

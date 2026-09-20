#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// =====================================================
// LCD
// =====================================================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// =====================================================
// TRAFFIC LIGHT PINS
// =====================================================
const byte A_RED    = 2;
const byte A_YELLOW = 3;
const byte A_GREEN  = 4;

const byte B_RED    = 5;
const byte B_YELLOW = 6;
const byte B_GREEN  = 7;

const byte BUZZER = 8;

// =====================================================
// KEYPAD
// =====================================================
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

// =====================================================
// TIMING
// =====================================================
const unsigned long GREEN_TIME = 10000;
const unsigned long YELLOW_TIME = 3000;
const unsigned long ALL_RED_TIME = 2000;
const unsigned long PEDESTRIAN_TIME = 10000;
const unsigned long EMERGENCY_TIME = 15000;

// =====================================================
// STATES
// =====================================================
enum SystemState {
  ROAD_A_GREEN,
  ROAD_A_YELLOW,
  ALL_RED_TO_B,

  ROAD_B_GREEN,
  ROAD_B_YELLOW,
  ALL_RED_TO_A,

  PEDESTRIAN,
  EMERGENCY_A,
  EMERGENCY_B,

  MANUAL
};

SystemState state = ROAD_A_GREEN;

unsigned long stateStart = 0;
unsigned long stateDuration = GREEN_TIME;

bool pedestrianRequest = false;

unsigned long lastLCDUpdate = 0;

// =====================================================
// LIGHT CONTROL
// =====================================================
void allLightsOff() {
  digitalWrite(A_RED, LOW);
  digitalWrite(A_YELLOW, LOW);
  digitalWrite(A_GREEN, LOW);

  digitalWrite(B_RED, LOW);
  digitalWrite(B_YELLOW, LOW);
  digitalWrite(B_GREEN, LOW);
}

void roadAGreen() {
  allLightsOff();
  digitalWrite(A_GREEN, HIGH);
  digitalWrite(B_RED, HIGH);
}

void roadAYellow() {
  allLightsOff();
  digitalWrite(A_YELLOW, HIGH);
  digitalWrite(B_RED, HIGH);
}

void roadBGreen() {
  allLightsOff();
  digitalWrite(A_RED, HIGH);
  digitalWrite(B_GREEN, HIGH);
}

void roadBYellow() {
  allLightsOff();
  digitalWrite(A_RED, HIGH);
  digitalWrite(B_YELLOW, HIGH);
}

void allRed() {
  allLightsOff();
  digitalWrite(A_RED, HIGH);
  digitalWrite(B_RED, HIGH);
}

// =====================================================
// LCD
// =====================================================
void showLCD(const char* line1, const char* line2, int seconds) {
  lcd.setCursor(0, 0);
  lcd.print("                ");

  lcd.setCursor(0, 0);
  lcd.print(line1);

  lcd.setCursor(0, 1);
  lcd.print("                ");

  lcd.setCursor(0, 1);
  lcd.print(line2);

  if (seconds >= 0) {
    lcd.setCursor(13, 1);

    if (seconds < 10)
      lcd.print(" ");

    lcd.print(seconds);
    lcd.print("s");
  }
}

// =====================================================
// CHANGE STATE
// =====================================================
void changeState(SystemState newState, unsigned long duration) {
  state = newState;
  stateStart = millis();
  stateDuration = duration;

  noTone(BUZZER);

  switch (state) {
    case ROAD_A_GREEN:
      roadAGreen();
      break;

    case ROAD_A_YELLOW:
      roadAYellow();
      break;

    case ALL_RED_TO_B:
      allRed();
      break;

    case ROAD_B_GREEN:
      roadBGreen();
      break;

    case ROAD_B_YELLOW:
      roadBYellow();
      break;

    case ALL_RED_TO_A:
      allRed();
      break;

    case PEDESTRIAN:
      allRed();
      break;

    case EMERGENCY_A:
      allRed();
      break;

    case EMERGENCY_B:
      allRed();
      break;

    case MANUAL:
      allRed();
      break;
  }
}

// =====================================================
// PEDESTRIAN
// =====================================================
void startPedestrian() {
  changeState(PEDESTRIAN, PEDESTRIAN_TIME);
  pedestrianRequest = false;
}

void runPedestrian() {
  unsigned long elapsed = millis() - stateStart;

  int secondsLeft =
    (PEDESTRIAN_TIME - elapsed + 999) / 1000;

  showLCD(
    "PEDESTRIAN",
    "CROSS NOW",
    secondsLeft
  );

  if (secondsLeft > 3) {
    static unsigned long lastBeep = 0;

    if (millis() - lastBeep >= 1000) {
      lastBeep = millis();
      tone(BUZZER, 1000, 100);
    }
  }
  else if (secondsLeft > 0) {
    static unsigned long lastFastBeep = 0;

    if (millis() - lastFastBeep >= 400) {
      lastFastBeep = millis();
      tone(BUZZER, 1600, 100);
    }
  }

  if (elapsed >= PEDESTRIAN_TIME) {
    noTone(BUZZER);
    changeState(ROAD_A_GREEN, GREEN_TIME);
  }
}

// =====================================================
// EMERGENCY
// =====================================================
void runEmergency(bool roadA) {
  unsigned long elapsed = millis() - stateStart;

  int secondsLeft =
    (EMERGENCY_TIME - elapsed + 999) / 1000;

  if (roadA) {
    allLightsOff();

    digitalWrite(A_GREEN, HIGH);
    digitalWrite(B_RED, HIGH);

    showLCD(
      "!! EMERGENCY !!",
      "ROAD A PRIORITY",
      secondsLeft
    );
  }
  else {
    allLightsOff();

    digitalWrite(A_RED, HIGH);
    digitalWrite(B_GREEN, HIGH);

    showLCD(
      "!! EMERGENCY !!",
      "ROAD B PRIORITY",
      secondsLeft
    );
  }

  static unsigned long lastEmergencyBeep = 0;

  if (millis() - lastEmergencyBeep >= 1000) {
    lastEmergencyBeep = millis();
    tone(BUZZER, 1800, 200);
  }

  if (elapsed >= EMERGENCY_TIME) {
    noTone(BUZZER);
    changeState(ROAD_A_GREEN, GREEN_TIME);
  }
}

// =====================================================
// MANUAL MODE
// =====================================================
void runManual() {
  showLCD(
    "MANUAL MODE",
    "A GREEN / B RED",
    -1
  );

  allLightsOff();

  digitalWrite(A_GREEN, HIGH);
  digitalWrite(B_RED, HIGH);
}

// =====================================================
// KEYPAD
// =====================================================
void checkKeypad() {
  char key = keypad.getKey();

  if (!key)
    return;

  if (key == '1') {
    if (
      state != EMERGENCY_A &&
      state != EMERGENCY_B &&
      state != PEDESTRIAN &&
      state != MANUAL
    ) {
      pedestrianRequest = true;

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("PEDESTRIAN");

      lcd.setCursor(0, 1);
      lcd.print("REQUEST QUEUED");

      tone(BUZZER, 1000, 200);

      delay(500);
    }
  }

  else if (key == 'A') {
    changeState(
      EMERGENCY_A,
      EMERGENCY_TIME
    );
  }

  else if (key == 'B') {
    changeState(
      EMERGENCY_B,
      EMERGENCY_TIME
    );
  }

  else if (key == 'C') {
    changeState(
      ROAD_A_GREEN,
      GREEN_TIME
    );
  }

  else if (key == 'D') {
    changeState(
      MANUAL,
      0
    );
  }

  else if (key == '#') {
    changeState(
      ROAD_A_GREEN,
      GREEN_TIME
    );
  }

  else if (key == '*') {
    pedestrianRequest = false;

    noTone(BUZZER);

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("SYSTEM RESET");

    allRed();

    delay(1000);

    changeState(
      ROAD_A_GREEN,
      GREEN_TIME
    );
  }
}

// =====================================================
// AUTOMATIC TRAFFIC
// =====================================================
void runAutomaticTraffic() {
  if (
    state == PEDESTRIAN ||
    state == EMERGENCY_A ||
    state == EMERGENCY_B ||
    state == MANUAL
  ) {
    return;
  }

  unsigned long elapsed =
    millis() - stateStart;

  if (elapsed >= stateDuration) {
    switch (state) {
      case ROAD_A_GREEN:
        changeState(
          ROAD_A_YELLOW,
          YELLOW_TIME
        );
        break;

      case ROAD_A_YELLOW:
        if (pedestrianRequest) {
          startPedestrian();
        }
        else {
          changeState(
            ALL_RED_TO_B,
            ALL_RED_TIME
          );
        }
        break;

      case ALL_RED_TO_B:
        changeState(
          ROAD_B_GREEN,
          GREEN_TIME
        );
        break;

      case ROAD_B_GREEN:
        changeState(
          ROAD_B_YELLOW,
          YELLOW_TIME
        );
        break;

      case ROAD_B_YELLOW:
        if (pedestrianRequest) {
          startPedestrian();
        }
        else {
          changeState(
            ALL_RED_TO_A,
            ALL_RED_TIME
          );
        }
        break;

      case ALL_RED_TO_A:
        changeState(
          ROAD_A_GREEN,
          GREEN_TIME
        );
        break;

      default:
        break;
    }
  }
}

// =====================================================
// LCD UPDATE
// =====================================================
void updateLCD() {
  if (millis() - lastLCDUpdate < 200)
    return;

  lastLCDUpdate = millis();

  unsigned long elapsed =
    millis() - stateStart;

  int secondsLeft = 0;

  if (stateDuration > elapsed) {
    secondsLeft =
      (stateDuration - elapsed + 999) / 1000;
  }

  switch (state) {
    case ROAD_A_GREEN:
      showLCD(
        "ROAD A: GREEN",
        "ROAD B: RED",
        secondsLeft
      );
      break;

    case ROAD_A_YELLOW:
      showLCD(
        "ROAD A: YELLOW",
        "ROAD B: RED",
        secondsLeft
      );
      break;

    case ALL_RED_TO_B:
      showLCD(
        "ALL RED",
        "CHANGING TO B",
        secondsLeft
      );
      break;

    case ROAD_B_GREEN:
      showLCD(
        "ROAD A: RED",
        "ROAD B: GREEN",
        secondsLeft
      );
      break;

    case ROAD_B_YELLOW:
      showLCD(
        "ROAD A: RED",
        "ROAD B:YELLOW",
        secondsLeft
      );
      break;

    case ALL_RED_TO_A:
      showLCD(
        "ALL RED",
        "CHANGING TO A",
        secondsLeft
      );
      break;

    default:
      break;
  }
}

// =====================================================
// SETUP
// =====================================================
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

  allRed();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("SMART TRAFFIC");

  lcd.setCursor(0, 1);
  lcd.print("SYSTEM READY");

  delay(2000);

  changeState(
    ROAD_A_GREEN,
    GREEN_TIME
  );
}

// =====================================================
// MAIN LOOP
// =====================================================
void loop() {
  checkKeypad();

  if (state == PEDESTRIAN) {
    runPedestrian();
  }
  else if (state == EMERGENCY_A) {
    runEmergency(true);
  }
  else if (state == EMERGENCY_B) {
    runEmergency(false);
  }
  else if (state == MANUAL) {
    runManual();
  }
  else {
    runAutomaticTraffic();
    updateLCD();
  }
}

// ============================================================
//  Landslide Early Warning System — Tinkercad Simulation
//  Soil Moisture Sensor → A0 (Potentiometer 1)
//  Rainfall Sensor     → A1 (Potentiometer 2)
//  Green LED           → Pin 10 (Normal)
//  Yellow LED          → Pin 11 (Watch)
//  Red LED             → Pin 9  (Warning)
//  Buzzer              → Pin 8
// ============================================================

// --- Pin Definitions ---
const int MOISTURE_PIN = A0;
const int RAINFALL_PIN = A1;
const int GREEN_LED    = 10;
const int YELLOW_LED   = 11;
const int RED_LED      = 9;
const int BUZZER       = 8;

// --- Thresholds (0–1023 analog range) ---
// T1/R1 = Watch level (OR logic)
// T2/R2 = Warning level (AND logic)
const int T1 = 400;   // Moisture Watch threshold (~40% saturation)
const int T2 = 700;   // Moisture Warning threshold (~70% saturation)
const int R1 = 350;   // Rainfall Watch threshold
const int R2 = 650;   // Rainfall Warning threshold

// --- Timing for intermittent buzzer (Watch state) ---
unsigned long previousMillis = 0;
const long BUZZ_INTERVAL = 500;  // buzz every 500ms in Watch state
bool buzzState = false;

void setup() {
  pinMode(GREEN_LED,  OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED,    OUTPUT);
  pinMode(BUZZER,     OUTPUT);
  Serial.begin(9600);
  Serial.println("=== Landslide EWS Simulation Started ===");
  Serial.println("Moisture_Raw | Rainfall_Raw | Alert_State");
  Serial.println("--------------------------------------------");
}

void loop() {
  // --- Read sensor values ---
  int moistureVal = analogRead(MOISTURE_PIN);
  int rainfallVal = analogRead(RAINFALL_PIN);

  // --- Determine alert state ---
  // WARNING: moisture AND rainfall both above upper thresholds
  bool isWarning = (moistureVal >= T2 && rainfallVal >= R2);
  // WATCH: moisture OR rainfall above lower threshold (but not full Warning)
  bool isWatch   = !isWarning && (moistureVal >= T1 || rainfallVal >= R1);
  // NORMAL: everything below lower thresholds

  // --- Apply alert outputs ---
  if (isWarning) {
    // All LEDs off except Red
    digitalWrite(GREEN_LED,  LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED,    HIGH);
    // Continuous buzzer
    tone(BUZZER, 1000);
    Serial.print(moistureVal);
    Serial.print("         | ");
    Serial.print(rainfallVal);
    Serial.println("          | *** WARNING ***");
  }
  else if (isWatch) {
    // All LEDs off except Yellow
    digitalWrite(GREEN_LED,  LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED,    LOW);
    // Intermittent buzzer using millis() (non-blocking)
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= BUZZ_INTERVAL) {
      previousMillis = currentMillis;
      buzzState = !buzzState;
      if (buzzState) {
        tone(BUZZER, 600);
      } else {
        noTone(BUZZER);
      }
    }
    Serial.print(moistureVal);
    Serial.print("         | ");
    Serial.print(rainfallVal);
    Serial.println("          | -- WATCH --");
  }
  else {
    // Normal state — Green LED only, buzzer silent
    digitalWrite(GREEN_LED,  HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED,    LOW);
    noTone(BUZZER);
    Serial.print(moistureVal);
    Serial.print("         | ");
    Serial.print(rainfallVal);
    Serial.println("          | Normal");
  }

  delay(300); // Read every 300ms
}
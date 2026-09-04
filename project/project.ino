#include <U8g2lib.h>
#include <Wire.h>
// ========== OLED ==========
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
// ========== PINS ==========
const int SOIL_PIN        = A0;   // Soil Moisture
const int RAIN_PIN        = 2;    // Rain Sensor (DO)
const int WATER_LEVEL_PIN = A1;   // HW-038 Tank Level
const int RELAY_PIN       = 7;    // Relay for valve/pump
// ========== THRESHOLDS (same as your individual codes) ==========
const int SOIL_DRY_THRESHOLD = 500;     // Same as your soil code
const int TANK_MIN_WATER     = 30;      // Minimum % to allow watering (adjust later)
const int TANK_CRITICAL      = 8;       // Below this = Critical level
const int TANK_FULL          = 100;     // At/above this = Full

void setup() {
  Serial.begin(9600);
  pinMode(RAIN_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);   // Relay OFF at start (active LOW)
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(10, 30, "Rain-Aware System");
  u8g2.sendBuffer();
  delay(2000);
}

void loop() {
  // ========== 1. Read Soil (same as your code) ==========
  int soilValue = analogRead(SOIL_PIN);
  bool soilDry = (soilValue > SOIL_DRY_THRESHOLD);
  // ========== 2. Read Rain (same as your code) ==========
  bool isRaining = (digitalRead(RAIN_PIN) == LOW);   // Change to HIGH if your module is inverted
  // ========== 3. Read Tank Level (same as your HW-038 code) ==========
  int rawValue = analogRead(WATER_LEVEL_PIN);
  int tankPercent = map(rawValue, 0, 600, 0, 100);   // Keep your calibration number
  tankPercent = constrain(tankPercent, 0, 100);
  bool hasWater = (tankPercent >= TANK_MIN_WATER);

  // ========== 3b. Tank Status ==========
  String tankStatus;
  if (tankPercent < TANK_CRITICAL) {
    tankStatus = "CRITICAL";
  } else if (tankPercent >= TANK_FULL) {
    tankStatus = "FULL";
  } else {
    tankStatus = "OK";
  }

  // ========== 4. Decision Logic ==========
  bool shouldWater = soilDry && hasWater && !isRaining;
  // Control Relay
  if (shouldWater) {
    digitalWrite(RELAY_PIN, LOW);    // Turn ON
  } else {
    digitalWrite(RELAY_PIN, HIGH);   // Turn OFF
  }

  // ========== 5. Display on OLED ==========
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  // Soil
  u8g2.setCursor(0, 10);
  u8g2.print("Soil: ");
  u8g2.print(soilValue);
  u8g2.print(soilDry ? " DRY" : " WET");
  // Rain
  u8g2.setCursor(0, 22);
  u8g2.print("Rain: ");
  u8g2.print(isRaining ? "YES" : "NO");
  // Tank
  u8g2.setCursor(0, 34);
  u8g2.print("Tank: ");
  u8g2.print(tankPercent);
  u8g2.print("% (");
  u8g2.print(tankStatus);
  u8g2.print(")");
  // Valve
  u8g2.setCursor(0, 46);
  u8g2.print("Valve: ");
  u8g2.print(shouldWater ? "OPEN" : "CLOSED");
  // Status
  u8g2.setCursor(0, 58);
  if (tankStatus == "CRITICAL") {
    u8g2.print("Tank Critical!");
  } else if (shouldWater) {
    u8g2.print("Watering...");
  } else {
    u8g2.print("Waiting");
  }
  u8g2.sendBuffer();

  // Serial Monitor
  Serial.print("Soil: "); Serial.print(soilValue);
  Serial.print(soilDry ? " DRY" : " WET");
  Serial.print(" | Rain: "); Serial.print(isRaining ? "YES" : "NO");
  Serial.print(" | Tank: "); Serial.print(tankPercent);
  Serial.print("% ("); Serial.print(tankStatus); Serial.print(")");
  Serial.print(" | Valve: "); Serial.println(shouldWater ? "OPEN" : "CLOSED");

  delay(1000);
}
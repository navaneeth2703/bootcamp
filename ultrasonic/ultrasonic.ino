#define TRIG_PIN 9
#define ECHO_PIN 10

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  long duration;
  float distance_cm;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);;
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    Serial.println("No echo received");
  } else {
    distance_cm = duration * 0.0343 / 2.0;
    Serial.print("Distance: ");
    Serial.print(distance_cm);
    Serial.println(" cm");
  }

  delay(500);
}

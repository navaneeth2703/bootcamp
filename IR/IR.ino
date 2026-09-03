void setup() {
  pinMode(3,INPUT);
  Serial.begin(9600)
}

void loop() {
int x = digitalRead(3) 
Serial.print("IR : ");
Serial.println(x);  

}
  




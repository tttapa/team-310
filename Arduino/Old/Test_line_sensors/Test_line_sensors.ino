void setup() {
  Serial.begin(115200);
  pinMode(4, OUTPUT);
}

void loop() {
  int amb1 = analogRead(A1);
  int amb2 = analogRead(A2);
  digitalWrite(4, HIGH);
  delay(1);
  int refl1 = analogRead(A1);
  int refl2 = analogRead(A2);
  digitalWrite(4, LOW);
  delay(1);
  Serial.print(amb1 - refl1);
  Serial.print('\t');
  Serial.println(amb2 - refl2);
  delay(50);
}

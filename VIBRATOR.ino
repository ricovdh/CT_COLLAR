/** vibrator **/

void vib1() {
  digitalWrite(vibrator, HIGH);
  delay(1000);
  digitalWrite(vibrator, LOW);
}

void vib2() {
  digitalWrite(vibrator, HIGH);
  delay(500);
  digitalWrite(vibrator, LOW);
}

void vib3() {
  digitalWrite(vibrator, HIGH);
  delay(500);
  digitalWrite(vibrator, LOW);
  delay(500);
  digitalWrite(vibrator, HIGH);
  delay(500);
  digitalWrite(vibrator, LOW);
  delay(500);
}

void vib4() {
  digitalWrite(vibrator, HIGH);
  delay(200);
  digitalWrite(vibrator, LOW);
  delay(200);
  digitalWrite(vibrator, HIGH);
  delay(200);
  digitalWrite(vibrator, LOW);
  delay(200);
  digitalWrite(vibrator, HIGH);
  delay(200);
  digitalWrite(vibrator, LOW);
  delay(200);
}
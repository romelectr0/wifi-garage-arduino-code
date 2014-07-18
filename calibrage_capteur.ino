int photopin = A0;
int photostate = analogRead(photopin);
void setup () {
  Serial.begin(9600);
}
void loop() {
  Serial.println(photostate);
  delay(1000);
}

#define generator 23
#define start 22

void setup() {
  pinMode(generator, OUTPUT);
  pinMode(start, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(start, HIGH);
  for (int i = 0; i < 50000000; i ++)
  {
    digitalWrite(generator, HIGH);
    digitalWrite(generator, LOW);
  }
  digitalWrite(start, LOW);
  delay(5000);
}

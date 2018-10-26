#define PULSE 23
#define SIGNAL 22

void setup() {
  pinMode(PULSE, OUTPUT);
  pinMode(SIGNAL, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  // Set signal to start reading
  digitalWrite(SIGNAL, HIGH);
  delay(200); // Give pi some time to start reading
  
  for (int i = 0; i < 500000; i ++)
  {
    digitalWrite(PULSE, HIGH);
    digitalWrite(PULSE, LOW);
    // Minimal period for real time kernal to handle each interrupt
    delayMicroseconds(106);
//    delay(1);
  }

  // Set signal to stop reading
  digitalWrite(SIGNAL, LOW);
  delay(5000);
}

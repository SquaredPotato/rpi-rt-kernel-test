#define PLS 28  // Pulse output         // Yellow
#define ACT 27  // RP's ready signal    // Blue
#define VER 24  // Own ready signal     // White
#define RES 23  // Timers reset signal  // Red

byte actStatus;
byte resStatus;

// Simple functions to update ACT and RES only when needed
void actChange();
void resChange();

void setup() {
  pinMode(ACT, INPUT);
  pinMode(RES, INPUT);
  pinMode(PLS, OUTPUT);
  pinMode(VER, OUTPUT);

  // Initialise outputs
  digitalWrite(PLS, LOW);
  digitalWrite(VER, LOW);

  // Minimise time spent reading pins in loop
  attachInterrupt(digitalPinToInterrupt(ACT), actChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RES), resChange, CHANGE);
}

void loop() {
  // Wait for Pi to give start signal
  while (digitalRead(ACT) == LOW)
  { }
  
  // Set verification that we're ready to start pulsing
  digitalWrite(VER, HIGH);
  
  // Wait for timer
  while (digitalRead(RES) == HIGH)
  { }

  // Send pulses as long as the pi requests
  while(digitalRead(ACT) == HIGH)
  {
    digitalWrite(PLS, HIGH);
    digitalWrite(PLS, LOW);
  }

  // Verify that we're done sending
  digitalWrite(VER, LOW);
}

void actChange()
{ actStatus = digitalRead(ACT); }

void resChange()
{ resStatus = digitalRead(RES); }


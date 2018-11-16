void requestEvent();
void receiveEvent(int numBytes);

void setup() {
	Wire.begin(SLAVE_ADDR);			// Start i2c in slave mode
	Wire.onRequest(requestEvent);	// Interrupt on data request by master
	Wire.onReceive(receiveEvent);	// Interrupt on data receive from master
	Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

}

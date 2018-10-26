#include <stdio.h>
#include <zconf.h>
#include <wiringPi.h>
#include <sys/time.h>

// Physical pin numbers
#define ADRSIGN 26
#define PULSE 20
#define LSIGN 21

void* isr();

int counted = 0;

int main()
{
	wiringPiSetupGpio();        // Setup without remapping

	pinMode(LSIGN, OUTPUT);     // Signals when we're done counting
	pinMode(ADRSIGN, INPUT);    // Signals while arduino is sending
	pinMode(PULSE, INPUT);      // Arduino signal input

	pullUpDnControl(PULSE, PUD_UP); // Avoid noise

	wiringPiISR(PULSE, INT_EDGE_BOTH, (void (*)(void)) isr);

	// Wait for arduino to stop pulsing so we start on next set of pulses
	printf("Syncing with arduino\n");
	while (digitalRead(ADRSIGN))
	{ }

	while (1)
	{
		digitalWrite(LSIGN, LOW);

		// Wait for arduino to start pulsing
		printf("Waiting for arduino\n");
		while (!digitalRead(ADRSIGN))
		{ }
		printf("Counting pulses...\n");
		counted = 0;

		// Small delay to give arduino time to start sending
		delay(1000);

		// Wait for arduino to stop pulsing
		while (digitalRead(ADRSIGN))
		{
			//			waitForInterrupt(PULSE, 0);
			//			++ counted;
		}

		int totalCounted = counted;
		digitalWrite(LSIGN, HIGH);

		printf("Counted pulses during ADRSIGN: %d\n", totalCounted);

		delay(3000);
	}

	return 0;
}

void* isr()
{ counted ++; }
#include <stdio.h>
#include <zconf.h>
#include <wiringPi.h>
#include <sys/time.h>

#define TARGET 10000000

// Physical pin numbers
#define VER 26  // Arduino's ready signal   // White
#define PLS 20  // Pulse input              // Yellow
#define ACT 21  // Own ready signal         // Blue
#define RES 16  // Timers reset signal      // Red

void counter();
void resChange();

int counted = 0, reset = 0;

int main()
{
	// Setup without remapping
	wiringPiSetupGpio();

	pinMode(VER, INPUT);
	pinMode(PLS, INPUT);
	pinMode(RES, INPUT);
	pinMode(ACT, OUTPUT);

	pullUpDnControl(VER, PUD_DOWN);
	pullUpDnControl(RES, PUD_DOWN);
	pullUpDnControl(PLS, PUD_DOWN);

	// ISR to count pulses and to keep track of RESET
	wiringPiISR(PLS, INT_EDGE_RISING, counter);
	wiringPiISR(RES, INT_EDGE_FALLING, resChange);

	for (int i = 0; i < 100; i ++)
	{
		counted = 0;
		reset = 0;

		// Wait for timer
		delay(1000);

		printf("Setting ACT to HIGH, iteration: %d\n", i);
		digitalWrite(ACT, HIGH);

		printf("Waiting for RES to unset\n");
		while (digitalRead(RES) == HIGH)
		{ }

		// Note, without "delay(1)" the while loop won't escape, don't remove
		printf("Counting pulses, target: %d\n", TARGET);
		while (counted < TARGET)
		{ delay(1); }

		digitalWrite(ACT, LOW);
		printf("End of iteration\n");
	}

	return 0;
}

void counter()
{
	counted ++;
}

void resChange()
{ reset ++; }
#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define FPATH "/home/pi/timer/times.csv"
#define TARGET "10000000"

#define VER 26  // Arduino's ready signal   // White
#define ACT 21  // Counter's ready signal   // Blue
#define RES 16  // Timer reset signal       // Red

int save(int iteration, struct timespec act_high, struct timespec act_low, struct timespec ver_high,
		 struct timespec ver_low);
void toString(struct timespec machine, char *str);

int main()
{
	// Setup without remapping
	wiringPiSetupGpio();

	struct timespec act_high, act_low, ver_high, ver_low;

	pinMode(VER, INPUT);
	pinMode(ACT, INPUT);
	pinMode(RES, OUTPUT);

	pullUpDnControl(ACT, PUD_DOWN);
	pullUpDnControl(VER, PUD_DOWN);

	printf("RES: HIGH\n");
	digitalWrite(RES, HIGH);

	for (int i = 0; i < 100; i ++)
	{
		printf("Waiting for arduino and counter, iteration: %d\n", i);
		while (digitalRead(ACT) == LOW)
		{ }

		clock_gettime(CLOCK_MONOTONIC_RAW, &act_high);

		while (digitalRead(VER) == LOW)
		{ }

		clock_gettime(CLOCK_MONOTONIC_RAW, &ver_high);

		printf("Arduino and counter active, releasing RES\n");
		digitalWrite(RES, LOW);

		printf("Pi is counting\n");
		// Pi is counting while ACT is low
		while (digitalRead(ACT) == HIGH)
		{ }

		clock_gettime(CLOCK_MONOTONIC_RAW, &act_low);

		while (digitalRead(VER) == HIGH)
		{ }

		clock_gettime(CLOCK_MONOTONIC_RAW, &ver_low);

		printf("Counter done, setting RES\n");
		digitalWrite(RES, HIGH);

		if (save(i, act_high, act_low, ver_high, ver_low) == 1)
		{ printf("Error while saving\n"); }

		delay(100);
	}

	return 0;
}

int save(int iteration, struct timespec act_high, struct timespec act_low, struct timespec ver_high,
		 struct timespec ver_low)
{
	FILE *fp;
	// Open file in append mode if it exists
	if (access(FPATH, F_OK) == 0)
	{ fp = fopen(FPATH, "a"); }
	else
	{
		// Open file if write mode if it doesn't
		fp = fopen(FPATH, "w");

		if (fp == NULL)
		{
			fprintf(stderr, "Error opening file");
			return 1;
		}

		// Print colons into file
		fprintf(fp, "%s\n", "ITER_CNT, ACT_HIGH, ACT_LOW, VER_HIGH, VER_LOW, PLS_SND, PLS_REC");
	}

	// 10000000 is target from "../counter/src/main.c:6"
	char *ah = malloc(64), *al = malloc(64), *vh = malloc(64), *vl = malloc(64), ct[3], ps[24];

	sprintf(ct, "%d", iteration);

	// According to "../docs/length_commit_3.bmp", the frequency of the arduino's signal is about 177kHz
	int freq = 177000;
	// Duration in seconds
	double duration = (ver_low.tv_sec - ver_high.tv_sec) * 1000000 + (ver_low.tv_nsec - ver_high.tv_nsec) / 1000;
	// Convert to string
	sprintf(ps, "%lf", freq * duration);

	// Convert times to string
	toString(act_high, ah);
	toString(act_low, al);
	toString(ver_high, vh);
	toString(ver_low, vl);

	// Write data to file
	fprintf(fp, "%s, %s, %s, %s, %s, %s, %s\n", ct, ah, al, vh, vl, ps, TARGET);

	fclose(fp);

	// Free memory that we allocated in toString();
	free(ah);
	free(al);
	free(vh);
	free(vl);

	return 0;
}

void toString(struct timespec machine, char *str)
{ sprintf(str, "%lld.%.9ld", (long long)machine.tv_sec, machine.tv_nsec); }


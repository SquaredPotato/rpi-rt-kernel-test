#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define FPATH "/home/pi/timer/times.csv"
#define TARGET 10000000

#define VER 26  // Arduino's ready signal   // White
#define ACT 21  // Counter's ready signal   // Blue
#define RES 16  // Timer reset signal       // Red

int save(int iteration, struct timespec act_high, struct timespec act_low, struct timespec ver_high,
		 struct timespec ver_low);
char * toHuman(struct timespec machine);

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

	for (int i = 39; i < 100; i ++)
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

	if (access(FPATH, F_OK) == 0)
	{
		// Open file in append mode if it exists
		fp = fopen(FPATH, "a");
	}
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

	char *ah = toHuman(act_high),
		 *al = toHuman(act_low),
		 *vh = toHuman(ver_high),
		 *vl = toHuman(ver_low),
		 ct[3], ps[24], *pr = "10000000"; // Target from "../counter/src/main.c:6"

	sprintf(ct, "%d", iteration);

	// According to "../docs/length_commit_3.bmp", the frequency of the arduino's signal is about 177kHz
	int freq = 177000;
	// Duration in seconds
	double duration = (ver_low.tv_sec - ver_high.tv_sec) * 1000000 + (ver_low.tv_nsec - ver_high.tv_nsec) / 1000;
	// Convert to string
	sprintf(ps, "%lf", freq * duration);

	// Write data to file
	fprintf(fp, "%s, ", ct); // Count
	fprintf(fp, "%s, ", ah); // ACT_HIGH
	fprintf(fp, "%s, ", al); // ACT_LOW
	fprintf(fp, "%s, ", vh); // VER_HIGH
	fprintf(fp, "%s, ", vl); // VER_LOW
	fprintf(fp, "%s, ", ps); // Pulses send
	fprintf(fp, "%s\n", pr); // Pulses counted

	fclose(fp);

	// Free memory that we allocated in toHuman();
	free(ah);
	free(al);
	free(vh);
	free(vl);

	return 0;
}

char * toHuman(struct timespec machine)
{
//	struct tm *ptm = gmtime(&machine.tv_nsec);
	char *buf = malloc(64);
	sprintf(buf, "%lld.%.9ld", (long long)machine.tv_sec, machine.tv_nsec);
//	strftime(buf, sizeof buf, "%T", ptm);
	return buf;
}


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
char* toHuman(struct timespec machine);

int main()
{
	// Setup without remapping
	wiringPiSetupGpio();

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
		while (digitalRead(VER) == LOW || digitalRead(ACT) == LOW)
		{ }

		printf("Arduino and counter active, releasing RES\n");
		digitalWrite(RES, LOW);

		printf("Pi is counting\n");
		// Pi is counting while ACT is low
		while (digitalRead(ACT) == HIGH)
		{ }

		printf("Counter done, setting RES\n");
		digitalWrite(RES, HIGH);

		delay(100);

		// TODO :save timestamps
	}


	return 0;
}

int save(int iteration, struct timespec act_high, struct timespec act_low, struct timespec ver_high,
         struct timespec ver_low)
{
	FILE *fp;
	int ret;

	if (access(FPATH, F_OK) == 0)
	{
		// Open file in append mode if it exists
		fp = fopen(FPATH, "a");
		ret = 0;
	}
	else
	{
		// Open file if write mode if it doesn't
		fp = fopen(FPATH, "w");

		if (fp == NULL)
		{
			fprintf(stderr, "Error opening file");
			ret = 1;
		}

		// Print colons into file
		fprintf(fp, "ACT_HIGH, ACT_LOW, VER_HIGH, VER_LOW, PLS_SND, PLS_REC\n");
	}

	if (ret == 0)
	{
		char *ah = toHuman(act_high),
			 *al = toHuman(act_low),
			 *vh = toHuman(ver_high),
			 *vl = toHuman(ver_low),
			 *ps, *pr = "10000000"; // Target from "../counter/src/main.c:6"

		// According to "../docs/length_commit_3.bmp", the frequency of the arduino's signal is about 177kHz
		int freq = 177000;
		// Duration in seconds
		double duration = ver_high.tv_nsec - ver_low.tv_nsec / 1000000000.0;

		sprintf(ps, "%lf", freq * duration);

	}

	return ret;
}

char* toHuman(struct timespec machine)
{
	struct tm *ptm = gmtime(&machine);
	char buf[8];
	strftime(buf, sizeof buf, "%T", ptm);
	return &buf;
}


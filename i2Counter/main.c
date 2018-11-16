#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>

// Constants
#define MAX_SENT_BYTES 9

uint32_t calcCrc(uint8_t *message);

// Globals
int deviceHandle;
int readBytes;
__s32 *buffer;

int main()
{
	printf("hoi");

	// initialize buffer
	buffer[0] = 0x00;

	int devAddr = 0x05;

	printf("hoi");

	// open device on /dev/i2c-1
	if ((deviceHandle = open("/dev/i2c-1", O_RDWR)) < 0)
	{
		printf("Error: Couldn't open i2c bus: %d\n", deviceHandle);
		return 1;
	}

	printf("hoi");

	// connect to arduino as i2c slave
	if (ioctl(deviceHandle, I2C_FUNC_SLAVE, devAddr) < 0) {
		printf("Error: Device not found\n");
		return 1;
	}

	printf("hoi");

	readBytes = write(deviceHandle, buffer, 1);
	if (readBytes != 1)
	{ printf("Error: No ack bit received, exiting\n"); }
	else
	{
		for (int i = 0; i < MAX_SENT_BYTES; i ++)
		{ buffer[i] =   (deviceHandle); }

		for (int i = 0; i < MAX_SENT_BYTES; i ++)
		{ printf("%d ", buffer[i]); }
	}

	return 0;
}



uint32_t calcCrc(uint8_t *message)
{
	uint32_t i = 0, j, byte, crc, mask;
	crc = 0xFFFFFFFF;

	while (message[i] != 0)
	{
		byte = message[i];
		crc = crc ^ byte;

		for (j = 7; j >= 0; j--)
		{
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
		i ++;
	}

	return crc;
}
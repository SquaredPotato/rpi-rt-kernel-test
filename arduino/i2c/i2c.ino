#include<Wire.h>

#define SLAVE_ADDR     0x05
#define REG_MAP_SIZE   8
#define MAX_SENT_BYTES 4
#define INT_MAX        4294967296 // 32 bit maximum

uint8_t registerMap[REG_MAP_SIZE];
uint8_t registerMapTemp[REG_MAP_SIZE - 1];
uint8_t receivedCommands[MAX_SENT_BYTES];
uint32_t recNum = 0;
int received;

void requestEvent();
void receiveEvent();
void *getMsg();
void makeNum(uint32_t num, uint8_t *array);
uint32_t calcCrc(uint8_t *message);

void setup()
{
	Wire.begin(SLAVE_ADDR);
	Wire.onRequest(requestEvent);
	Wire.onReceive(receiveEvent);
	Serial.begin(9600);

	// Wait for number to use as message
	while (!received) {}
	makeNum(recNum, registerMap);
	Serial.write(recNum);

	getMsg();
}

void loop()
{ }

void *getMsg() 
{	// Puts a 32 bits number and a 32 bits CRC in byte array
	// uint32_t num = random(0, INT_MAX);
	uint8_t numArray[4] = {
		(uint8_t) recNum >> 24,
		(uint8_t) recNum >> 16,
		(uint8_t) recNum >> 8,
		(uint8_t) recNum
	};

	uint32_t crc = calcCrc(numArray);
	uint8_t crcArray[4] = {
		(uint8_t) crc >> 24,
		(uint8_t) crc >> 16,
		(uint8_t) crc >> 8,
		(uint8_t) crc
	};

	uint8_t registerMap[REG_MAP_SIZE] = {
		numArray[0],
		numArray[1],
		numArray[2],
		numArray[3],
		crcArray[0],
		crcArray[1],
		crcArray[2],
		crcArray[3],
	};
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

	return ~crc;
}

void makeNum(uint32_t num, uint8_t *array)
{
	num |= array[0] << 24;
	num |= array[1] << 16;
	num |= array[2] << 8;
	num |= array[3];
}

void requestEvent()
{ Wire.write(registerMap, REG_MAP_SIZE); }  //Set the buffer up to send all bytes of data 

void receiveEvent(int numBytes)
{
	for (int i = 0; i < numBytes; i ++)
	{
		if (i < MAX_SENT_BYTES) // Write data to array
		{ receivedCommands[i] = Wire.read(); }
		else                    // When we're not expecting more, discard data
		{ Wire.read(); }
	}

	received = 1;
}

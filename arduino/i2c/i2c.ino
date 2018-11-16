#include <Wire.h>

#define SLAVE_ADDR     0x05
#define REG_MAP_SIZE   8
#define MAX_SENT_BYTES 4
#define INT_MAX        4294967295 // 32 bit maximum

uint8_t registerMap[REG_MAP_SIZE];
uint8_t registerMapTemp[REG_MAP_SIZE - 1];
uint8_t receivedMap[MAX_SENT_BYTES];
uint32_t recNum = 0;
int received = 0, updated = 0, recBytes;

void requestEvent();
void receiveEvent();
void getMsg();
uint32_t makeNum(uint8_t *array, int start, int end);
uint32_t calcCrc(uint8_t *message);

void setup()
{
	Wire.begin(SLAVE_ADDR);			// Start i2c in slave mode
	Wire.onRequest(requestEvent);	// Interrupt on data request by master
	Wire.onReceive(receiveEvent);	// Interrupt on data receive from master
	Serial.begin(9600);
}

// Not really needed since this is interrupt based
void loop()
{
	if (received > 0)
	{
		received = 0;
		Serial.write("Data received, updating return values\n");

		Serial.write("Number of bytes received: [ ");
		Serial.write(String(recBytes).c_str());
		Serial.write(" ]\n");

		getMsg();
		recNum = makeNum(receivedMap, 0, MAX_SENT_BYTES - 1);
		Serial.write("Received: [ ");
		Serial.write(String(recNum).c_str());
		Serial.write(" ] from master\n");
		uint32_t ownNum, crc;
		
		ownNum |= registerMap[0] << 24;
		ownNum |= registerMap[1] << 16;
		ownNum |= registerMap[2] << 8;
		ownNum |= registerMap[3];

		crc |= registerMap[4] << 24;
		crc |= registerMap[5] << 16;
		crc |= registerMap[6] << 8;
		crc |= registerMap[7];

		Serial.write("ownNum: [ ");
		Serial.write(String(ownNum).c_str());
		Serial.write(" ]\ncrc: [ ");
		Serial.write(String(crc).c_str());
		Serial.write(" ]\n");

		Serial.write("Sending on request:\n");
		for (int i = 0; i < REG_MAP_SIZE; i ++)
		{ Serial.write(String(registerMap[i]).c_str()); }
		Serial.write("\n");
	}
}

void getMsg() 
{	// Puts a 32 bits number and a 32 bits CRC in byte array
	// uint32_t num = random(0, INT_MAX);
	Serial.write("getMsg\n");

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

	for (int i = 0; i < 8; i ++) 
	{ Serial.write(String(numArray[i]).c_str()); }

	for (int i = 0; i < REG_MAP_SIZE / 2; i ++)
	{ registerMap[i] = numArray[i]; }

	for (int i = 4; i < REG_MAP_SIZE; i ++)
	{ registerMap[i] = crcArray[i]; }

	Serial.write("registerMap: [ ");
	for (int i = 0; i < REG_MAP_SIZE; i ++)
	{
		Serial.write(String(registerMap[i]).c_str());
		Serial.write(", ");
	}
	Serial.write(" ]\n");
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

// Makes 32 bit int from byte array
uint32_t makeNum(uint8_t *array, int start, int end)
{
	uint32_t num;

	for (int i = start, shift = (end - start) * 8; i < end; i ++, shift -= 8)
	{ num |= array[i] << shift; }

	return num;

	// num |= array[0] << 24;
	// num |= array[1] << 16;
	// num |= array[2] << 8;
	// num |= array[3];
}

void requestEvent()
{
	for (int i = 0; i < REG_MAP_SIZE; i ++)
	{ Wire.write(registerMap[i]); }
	Wire.write('\n');
}

// Writes received data to receivedMap
void receiveEvent(int numBytes)
{
	recBytes = numBytes;
	for (int i = 0; i < numBytes; i ++)
	{
		if (i < MAX_SENT_BYTES) // Write data to array
		{
			receivedMap[i] = Wire.read();
		}
		else
		{
			Wire.read();
		}
	}
	received ++;
}

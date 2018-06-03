#include "xtea.h"


void xtea_encrypt(const uint32_t key[4], uint8_t* const data, const uint16_t len)
{
	const uint32_t* const k = key;
	uint32_t* const buffer = (uint32_t*) data;

	int read_pos = 0;

	while (read_pos < (len / 4)) {
		uint32_t v0 = buffer[read_pos], v1 = buffer[read_pos + 1];
		uint32_t delta = 0x61C88647;
	 	uint32_t sum = 0;

		for (int32_t i = 0; i < 32; ++i) {
			v0 += ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
			sum -= delta;
			v1 += ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[sum>>11 & 3]);
		}

		buffer[read_pos] = v0; buffer[read_pos + 1] = v1;
		read_pos = read_pos + 2;
	}

}

void xtea_decrypt(const uint32_t key[4], uint8_t* const data, const uint16_t len)
{
	const uint32_t* const k = key;
	uint32_t* const buffer = (uint32_t*) data;
	int read_pos = 0;
	
	while (read_pos < (len / 4)) {
		uint32_t v0 = buffer[read_pos], v1 = buffer[read_pos + 1];
		uint32_t delta = 0x61C88647;
		uint32_t sum = 0xC6EF3720;

		for (int32_t i = 0; i < 32; i++) {
			v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[sum>>11 & 3]);
			sum += delta;
			v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
		}

		buffer[read_pos] = v0; buffer[read_pos + 1] = v1;
		read_pos = read_pos + 2;
	}

}


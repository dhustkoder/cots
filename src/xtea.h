#ifndef COTS_XTEA_H_
#define COTS_XTEA_H_
#include <stdint.h>


void xtea_encrypt(const uint32_t key[4], uint8_t* data, uint16_t len);
void xtea_decrypt(const uint32_t key[4], uint8_t* data, uint16_t len);


#endif

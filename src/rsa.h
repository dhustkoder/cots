#ifndef COTS_RSA_H_
#define COTS_RSA_H_
#include <stdint.h>

void rsa_init(void);
void rsa_term(void);
void rsa_decrypt(uint8_t buffer[128]); // 128 bytes

#endif
#ifndef COTS_NETMSG_H_
#define COTS_NETMSG_H_
#include <stdint.h>


struct netmsg {
	uint16_t len;
	uint8_t* buf;
};


#define NETMSG_OPCODE_ENTER_ACCOUNT ((uint8_t)1)


#endif
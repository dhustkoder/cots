#ifndef COTS_CONNECTION_H_
#define COTS_CONNECTION_H_
#include <stdbool.h>


struct netmsg {
	uint16_t len;
	uint8_t* buf;
	void* conn_info;
};


#define COTS_IP_ADDR_BUFFER_SIZE  (32)

/* protocol opcodes */
#define COTS_OPCODE_ENTER_ACCOUNT ((uint8_t)1)

void connection_init(void(*login_protocol_callback)(struct netmsg netmsg));
void connection_term(void);
void connection_poll(int ms);
void connection_get_ip_addr(void* conn_info, char buffer[COTS_IP_ADDR_BUFFER_SIZE]);
void connection_send(void* conn_info, const uint8_t* data, uint16_t len);


#endif

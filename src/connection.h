#ifndef COTS_CONNECTION_H_
#define COTS_CONNECTION_H_
#include <stdbool.h>


struct netmsg {
	uint16_t len;
	uint8_t* const buf;
};


struct conn_info {
	struct netmsg in_nm;
	struct netmsg out_nm;
	uint32_t xtea_key[4];
	void* const internal;
};


typedef void(*connection_callback_t)(struct conn_info* ci);


#define COTS_IP_ADDR_BUFFER_SIZE  (32)


/* protocol opcodes */
#define COTS_OPCODE_ENTER_ACCOUNT ((uint8_t)1)


void connection_init(connection_callback_t login_callback);
void connection_term(void);
void connection_poll(int ms);
void connection_get_ip_addr(struct conn_info* ci, char buffer[COTS_IP_ADDR_BUFFER_SIZE]);


#endif

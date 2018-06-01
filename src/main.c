#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "rsa.h"
#include "memory.h"
#include "connection.h"


static void enter_account(struct netmsg netmsg)
{
	rsa_decrypt(netmsg.buf + 17);

	const uint32_t account_number = memread_u32(netmsg.buf + 34);

	const uint16_t account_password_len = memread_u16(netmsg.buf + 38);
	char account_password[account_password_len + 1];
	memcpy(account_password, netmsg.buf + 40, account_password_len);
	account_password[account_password_len] = '\0';

	log_debug("Login attempt:\n"
	          "Account Number: %" PRIu32 "\n"
	          "Account Password: %s\n",
	           account_number, account_password);

}

static void login_protocol_handler(struct netmsg netmsg)
{

	char ip_addr[COTS_IP_ADDR_BUFFER_SIZE];
	connection_get_ip_addr(netmsg.conn_info, ip_addr);
	const uint8_t opcode = memread_u8(netmsg.buf);

	log_debug("New Request From: %s\n"
	          "Protocol Opcode: %" PRIu8 "\n"
	          "Client OS: %" PRIu16 "\n"
	          "Client Version: %" PRIu16 "\n",
	          ip_addr, opcode,
	          memread_u16(netmsg.buf + 1), memread_u16(netmsg.buf + 3));

	switch (opcode) {
	case COTS_OPCODE_ENTER_ACCOUNT:
		enter_account(netmsg);
		break;
	}

}



int main(void) 
{
	log_init();
	rsa_init();
	connection_init(login_protocol_handler);
	
	for (;;) {
		connection_poll(16000);
	}

	connection_term();
	rsa_term();
	log_term();
	return EXIT_SUCCESS;
}


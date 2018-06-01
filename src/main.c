#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "rsa.h"
#include "netmsg.h"
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


	printf("Account Number: %" PRIu32 "\n"
	       "Account Password: %s\n",
	        account_number, account_password);

}

static void connection_callback(struct netmsg netmsg)
{
	const uint8_t opcode = memread_u8(netmsg.buf);

	printf("Protocol Opcode: %" PRIu8 "\n"
		   "Client OS: %" PRIu16 "\n"
	       "Client Version: %" PRIu16 "\n",
	       opcode,
	       memread_u16(netmsg.buf + 1),
	       memread_u16(netmsg.buf + 3));

	switch (opcode) {
	case NETMSG_OPCODE_ENTER_ACCOUNT:
		enter_account(netmsg);
		break;
	}

}



int main(void) 
{
	rsa_init();
	connection_init(connection_callback);
	for (;;) {
		connection_poll(16000);
	}
	rsa_term();
	connection_term();
	return EXIT_SUCCESS;
}


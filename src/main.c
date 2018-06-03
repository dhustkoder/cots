#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "log.h"
#include "rsa.h"
#include "memory.h"
#include "connection.h"


static bool signal_recv = false;

static void signal_handler(int sig)
{
	log_info("Received signal: %d", sig);
	signal_recv = true;
}

static void enter_account(struct conn_info* const ci)
{
	rsa_decrypt(ci->input_msg.buf + 19);

	const uint32_t account_number = memread_u32(ci->input_msg.buf + 36);

	const uint16_t account_password_len = memread_u16(ci->input_msg.buf + 40);
	char account_password[account_password_len + 1];
	memcpy(account_password, ci->input_msg.buf + 42, account_password_len);
	account_password[account_password_len] = '\0';

	log_debug("Login attempt:\n"
	          "Account Number: %" PRIu32 "\n"
	          "Account Password: %s\n",
	           account_number, account_password);

}

static void login_protocol_handler(struct conn_info* const ci)
{

	char ip_addr[COTS_IP_ADDR_BUFFER_SIZE];
	connection_get_ip_addr(ci, ip_addr);
	const uint8_t opcode = memread_u8(ci->input_msg.buf + 2);

	log_debug("New Request From: %s\n"
	          "Protocol Opcode: %" PRIu8 "\n"
	          "Client OS: %" PRIu16 "\n"
	          "Client Version: %" PRIu16 "\n",
	          ip_addr, opcode,
	          memread_u16(ci->input_msg.buf + 3), memread_u16(ci->input_msg.buf + 5));

	switch (opcode) {
	case COTS_OPCODE_ENTER_ACCOUNT:
		enter_account(ci);
		break;
	}

}



int main(void) 
{
	log_init();
	rsa_init();
	connection_init(login_protocol_handler);
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	
	while (!signal_recv) {
		connection_poll(16000);
	}

	connection_term();
	rsa_term();
	log_term();
	return EXIT_SUCCESS;
}


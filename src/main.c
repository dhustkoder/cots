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

static void opcode_enter_account(struct conn_info* const ci)
{
	ci->xtea_key[0] = memread_u32(ci->in_nm.buf + 18);
	ci->xtea_key[1] = memread_u32(ci->in_nm.buf + 22);
	ci->xtea_key[2] = memread_u32(ci->in_nm.buf + 26);
	ci->xtea_key[3] = memread_u32(ci->in_nm.buf + 30);


	log_debug("XTEA KEY RECEIVED: \n"
	         "[0] = $%.4x\n"
	         "[1] = $%.4x\n"
	         "[2] = $%.4x\n"
	         "[3] = $%.4x\n",
	         ci->xtea_key[0], ci->xtea_key[1],
	         ci->xtea_key[2], ci->xtea_key[3]); 


	const uint16_t client_os = memread_u16(ci->in_nm.buf + 1);
	const uint16_t client_version = memread_u16(ci->in_nm.buf + 3);

	const uint32_t account_number = memread_u32(ci->in_nm.buf + 34);

	const uint16_t account_password_len = memread_u16(ci->in_nm.buf + 38);
	char account_password[account_password_len + 1];
	memcpy(account_password, ci->in_nm.buf + 40, account_password_len);
	account_password[account_password_len] = '\0';

	log_debug("Login attempt:\n"
	          "Client OS: %" PRIu16 "\n"
		  "Client Version: %" PRIu16 "\n"
	          "Account Number: %" PRIu32 "\n"
	          "Account Password: %s\n",
		   client_os, client_version,
	           account_number, account_password);

	const char* const test_msg = "Server Response: In Maintenance.";
	const uint16_t test_msg_len = strlen(test_msg);
	memwrite_u8(ci->out_nm.buf, 0x0A);
	memwrite_u16(ci->out_nm.buf + 1, test_msg_len);
	memcpy(ci->out_nm.buf + 3, test_msg, test_msg_len);
	ci->out_nm.len = 3 + test_msg_len;

}

static void login_protocol_handler(struct conn_info* const ci)
{
	char ip_addr[COTS_IP_ADDR_BUFFER_SIZE];
	connection_get_ip_addr(ci, ip_addr);
	const uint8_t opcode = memread_u8(ci->in_nm.buf);

	log_debug("New Request From: %s to login protocol\n"
	          "Protocol Opcode: %" PRIu8 "\n",
	          ip_addr, opcode);

	switch (opcode) {
	case COTS_OPCODE_ENTER_ACCOUNT: opcode_enter_account(ci); break;
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


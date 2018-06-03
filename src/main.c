#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "log.h"
#include "rsa.h"
#include "xtea.h"
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


	const uint32_t xtea_key[4] = {
		memread_u32(ci->input_msg.buf + 20),
		memread_u32(ci->input_msg.buf + 24),
		memread_u32(ci->input_msg.buf + 28),
		memread_u32(ci->input_msg.buf + 32)
	};


	log_info("XTEA KEY RECEIVED: \n"
	         "[0] = $%.4x\n"
	         "[1] = $%.4x\n"
	         "[2] = $%.4x\n"
	         "[3] = $%.4x\n",
	         xtea_key[0], xtea_key[1], xtea_key[2], xtea_key[3]); 


	const uint32_t account_number = memread_u32(ci->input_msg.buf + 36);

	const uint16_t account_password_len = memread_u16(ci->input_msg.buf + 40);
	char account_password[account_password_len + 1];
	memcpy(account_password, ci->input_msg.buf + 42, account_password_len);
	account_password[account_password_len] = '\0';

	log_debug("Login attempt:\n"
	          "Account Number: %" PRIu32 "\n"
	          "Account Password: %s\n",
	           account_number, account_password);


	uint8_t out[] = {
		0x28,
		0x00,
		0xd0,
		0x8a,
		0x7c,
		0xec,
		0xcc,
		0x40,
		0x78,
		0x40,
		0xdb,
		0x78,
		0x4e,
		0x2f,
		0x76,
		0x2c,
		0xff,
		0x36,
		0x3f,
		0x8f,
		0xff,
		0x62,
		0x33,
		0x8b,
		0xc4,
		0xca,
		0x3b,
		0xf0,
		0x34,
		0x39,
		0x04,
		0x1f,
		0x4f,
		0x19,
		0x3a,
		0x3e,
		0x68,
		0x54,
		0xe6,
		0x83,
		0x3b,
		0x0b
	};

	memcpy(ci->output_msg.buf, out, sizeof(out));
	ci->output_msg.len = sizeof(out);

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


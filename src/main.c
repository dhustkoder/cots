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
	rsa_decrypt(ci->input_msg.buf + 17);


	const uint32_t xtea_key[4] = {
		memread_u32(ci->input_msg.buf + 18),
		memread_u32(ci->input_msg.buf + 22),
		memread_u32(ci->input_msg.buf + 26),
		memread_u32(ci->input_msg.buf + 30)
	};


	log_info("XTEA KEY RECEIVED: \n"
	         "[0] = $%.4x\n"
	         "[1] = $%.4x\n"
	         "[2] = $%.4x\n"
	         "[3] = $%.4x\n",
	         xtea_key[0], xtea_key[1], xtea_key[2], xtea_key[3]); 


	const uint32_t account_number = memread_u32(ci->input_msg.buf + 34);

	const uint16_t account_password_len = memread_u16(ci->input_msg.buf + 38);
	char account_password[account_password_len + 1];
	memcpy(account_password, ci->input_msg.buf + 40, account_password_len);
	account_password[account_password_len] = '\0';

	log_debug("Login attempt:\n"
	          "Account Number: %" PRIu32 "\n"
	          "Account Password: %s\n",
	           account_number, account_password);

	const char* const test_msg = "Server response: closing connection.";
	const uint16_t test_msg_len = strlen(test_msg);
	memwrite_u8(ci->output_msg.buf + 2, 0x0A);
	memwrite_u16(ci->output_msg.buf + 3, test_msg_len);
	memcpy(ci->output_msg.buf + 5, test_msg, test_msg_len);
	uint16_t output_len = 5 + test_msg_len;
	
	if ((output_len % 8) != 0) {
		output_len += 8 - (output_len % 8);
	}

	memwrite_u16(ci->output_msg.buf, output_len - 2);
	ci->output_msg.len = output_len;

	printf("\n\n========== before xtea ========\n\n");
	for (uint16_t i = 0; i < ci->output_msg.len; ++i) {
		printf("0x%.2x\n", ci->output_msg.buf[i]);
	}

	xtea_encrypt(xtea_key, ci->output_msg.buf, ci->output_msg.len);

	printf("\n\n========== after xtea ========\n\n");
	for (uint16_t i = 0; i < ci->output_msg.len; ++i) {
		printf("0x%.2x\n", ci->output_msg.buf[i]);
	}

}

static void login_protocol_handler(struct conn_info* const ci)
{

	char ip_addr[COTS_IP_ADDR_BUFFER_SIZE];
	connection_get_ip_addr(ci, ip_addr);
	const uint8_t opcode = memread_u8(ci->input_msg.buf);

	log_debug("New Request From: %s\n"
	          "Protocol Opcode: %" PRIu8 "\n"
	          "Client OS: %" PRIu16 "\n"
	          "Client Version: %" PRIu16 "\n",
	          ip_addr, opcode,
	          memread_u16(ci->input_msg.buf + 1), memread_u16(ci->input_msg.buf + 3));

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


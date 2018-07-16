#include <string.h>
#include <inttypes.h>
#include "memory.h"
#include "rsa.h"
#include "log.h"
#include "login_protocol.h"


static void opcode_enter_account(struct conn_info* const ci)
{
	rsa_decrypt(ci->in_nm.buf + 17);

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


void login_protocol_callback(struct conn_info* const ci)
{
	const uint8_t opcode = memread_u8(ci->in_nm.buf);
	
	switch (opcode) {
	case COTS_OPCODE_ENTER_ACCOUNT: opcode_enter_account(ci); break;
	}
	
}


#include <stdio.h>
#include "mongoose.h"
#include "log.h"
#include "rsa.h"
#include "xtea.h"
#include "memory.h"
#include "connection.h"


static struct mg_mgr mgr;
static char current_login_protocol_addr[40];
static char current_game_protocol_addr[40];
connection_callback_t login_protocol_clbk;


static void ev_handler(struct mg_connection* const nc,
                       const int ev, void* const evp,
                       void* const addr)
{
	((void)evp);

	switch (ev) {
	case MG_EV_POLL: // Sent to each connection on each mg_mgr_poll() call
		break;
	case MG_EV_ACCEPT: // New connection accepted. union socket_address *
		break;
	case MG_EV_CONNECT: // connect() succeeded or failed. int *  
		break;

	case MG_EV_RECV: { // Data has been received. int *num_bytes

		log_debug("MG_RECV_PACKET size: %zu", nc->recv_mbuf.len);

		uint8_t output_buffer[256];
		memset(output_buffer, 0, sizeof(output_buffer));

		struct conn_info ci = {

			.in_nm = {
				.len = memread_u16(nc->recv_mbuf.buf),
				.buf = (uint8_t*)nc->recv_mbuf.buf + 2,
			},

			.out_nm = {
				.len = 0,
				.buf = output_buffer + 4,
			},

			.internal = nc

		};

		if (addr == current_login_protocol_addr) {
			login_protocol_clbk(&ci);
		}

		if (ci.out_nm.len > 0) {
			const uint16_t header_size = 2;
			const uint16_t encrypted_header_size = 2;
			const uint16_t msg_size = ci.out_nm.len;
			uint16_t body_size = msg_size + encrypted_header_size;

			if ((body_size % 8) != 0)
				body_size += 8 - (body_size % 8);

			ci.out_nm.len = body_size + header_size;

			log_debug("sending message encrypted header: %" PRIu16, msg_size);
			memwrite_u16(ci.out_nm.buf - 2, msg_size);

			xtea_encrypt(ci.xtea_key, ci.out_nm.buf - 2, body_size);

			log_debug("sending message header: %" PRIu16, body_size);
			memwrite_u16(ci.out_nm.buf - 4, body_size);

			log_debug("sending message final length: %" PRIu16, ci.out_nm.len);
			mg_send(ci.internal, ci.out_nm.buf - 4, ci.out_nm.len);

		}

		mbuf_remove(&nc->recv_mbuf, nc->recv_mbuf.len);
		break;
	}
	
	case MG_EV_SEND: // Data has been written to a socket. int *num_bytes 
		break;

	case MG_EV_CLOSE: // Connection is closed. NULL 
		break;

	case MG_EV_TIMER: // now >= conn->ev_timer_time. double * 
		break;

	default:
		log_warn("MG_EV_UNKNOWN %d", ev);
		break;
	}
}


bool connection_init(connection_callback_t login_protocol_callback,
                     const char* const login_protocol_addr, 
                     const char* const game_protocol_addr)
{
	log_info("Initializing Connection System...");
	log_info("Connection Login Protocol addr: %s", login_protocol_addr);
	log_info("Connection Game Protocol addr: %s", game_protocol_addr);

	strcpy(current_login_protocol_addr, login_protocol_addr);
	strcpy(current_game_protocol_addr, game_protocol_addr);

	mg_mgr_init(&mgr, NULL);
	if (mg_bind(&mgr, current_login_protocol_addr, ev_handler, (void*)current_login_protocol_addr) == NULL) {
		log_fatal("Failed to initialize Connection.");
		mg_mgr_free(&mgr);
		return false;
	}

	login_protocol_clbk = login_protocol_callback;
	return true;
}

void connection_term(void)
{
	log_info("Terminating Connection System...");
	mg_mgr_free(&mgr);
}

void connection_poll(const int ms)
{
	mg_mgr_poll(&mgr, ms);
}

void connection_get_ip_addr(struct conn_info* const ci, char buffer[COTS_IP_ADDR_BUFFER_SIZE])
{
	int sz = mg_conn_addr_to_str(ci->internal, buffer,
	                             COTS_IP_ADDR_BUFFER_SIZE,
	                             MG_SOCK_STRINGIFY_IP|MG_SOCK_STRINGIFY_REMOTE);
	buffer[sz] = '\0';
}



#include <stdio.h>
#include "mongoose.h"
#include "log.h"
#include "memory.h"
#include "connection.h"


static struct mg_mgr mgr;
static const char* const login_url = "tcp://localhost:7171";
static const char* const game_url = "udp://localhost:7272";
connection_callback_t login_protocol_clbk;


static void ev_handler(struct mg_connection* const nc, int ev, void* p, void* const url)
{
	((void)p);
	switch (ev) {
	case MG_EV_RECV: {

		log_info("MG RECV EV");

		struct conn_info ci = {
			.input_msg = {
				.len = memread_u16(nc->recv_mbuf.buf),
				.buf = (uint8_t*) nc->recv_mbuf.buf + 2
			},
			.output_msg = {
				.len = 0,
				.buf = NULL
			},
			.internal = nc
		};

		if (url == login_url)
			login_protocol_clbk(&ci);

		if (ci.output_msg.len > 0)
			mg_send(ci.internal, ci.output_msg.buf, ci.output_msg.len);

		mbuf_remove(&nc->recv_mbuf, nc->recv_mbuf.len);

		break;
	}
	
	case MG_EV_SEND:
		log_info("MG SEND EV");
		break;

	default:
		log_warn("MG EV UNKNOWN %d", ev);
		break;
	}
}


void connection_init(connection_callback_t login_protocol_callback)
{
	log_info("Initializing Connection System...");
	log_info("Connection Login Protocol url: %s", login_url);
	log_info("Connection Game Protocol url: %s", game_url);
	mg_mgr_init(&mgr, NULL);
	mg_bind(&mgr, login_url, ev_handler, (void*)login_url);
	login_protocol_clbk = login_protocol_callback;
}

void connection_term(void)
{
	log_info("Terminating Connection System...");
	mg_mgr_free(&mgr);
}

void connection_poll(int ms)
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


#include <stdio.h>
#include "mongoose.h"
#include "log.h"
#include "memory.h"
#include "connection.h"


static struct mg_mgr mgr;
static const char* const url = "tcp://localhost:7171";
void(*login_protocol_clbk)(struct netmsg netmsg);

static void login_protocol_ev_handler(struct mg_connection* const nc, int ev, void* const p)
{
	((void)p);

	switch (ev) {
	case MG_EV_RECV: {
		log_info("MG RECV EV");
		struct netmsg netmsg = {
			.len = memread_u16(nc->recv_mbuf.buf),
			.buf = (uint8_t*) nc->recv_mbuf.buf + 2,
			.conn_info = nc
		};

		login_protocol_clbk(netmsg);

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


void connection_init(void(*login_protocol_callback)(struct netmsg netmsg))
{
	log_info("Initializing Connection System...");
	log_info("Connection LoginProtocol url: %s", url);
	mg_mgr_init(&mgr, NULL);
	mg_bind(&mgr, url, login_protocol_ev_handler);
	login_protocol_clbk = login_protocol_callback;
}

void connection_term(void)
{
	mg_mgr_free(&mgr);
}

void connection_poll(int ms)
{
	mg_mgr_poll(&mgr, ms);
}

void connection_get_ip_addr(void* conn_info, char buffer[COTS_IP_ADDR_BUFFER_SIZE])
{
	int sz = mg_conn_addr_to_str(conn_info, buffer, COTS_IP_ADDR_BUFFER_SIZE, MG_SOCK_STRINGIFY_IP|MG_SOCK_STRINGIFY_REMOTE);
	buffer[sz] = '\0';
}


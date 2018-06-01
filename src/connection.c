#include <stdio.h>
#include "mongoose.h"
#include "log.h"
#include "memory.h"
#include "connection.h"


static struct mg_mgr mgr;
static const char* const login_url = "tcp://localhost:7171";
static const char* const game_url = "udp://localhost:7272";
void(*login_protocol_clbk)(struct netmsg netmsg);

static void ev_handler(struct mg_connection* const nc, int ev, void* p, void* const url)
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

		if (url == login_url)
			login_protocol_clbk(netmsg);

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


void connection_init(void(*login_protocol_callback)(struct netmsg netmsg))
{
	log_info("Initializing Connection System...");
	log_info("Connection LoginProtocol url: %s", login_url);
	log_info("Connection GameProtocol url: %s", game_url);
	mg_mgr_init(&mgr, NULL);
	mg_bind(&mgr, login_url, ev_handler, (void*)login_url);
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

void connection_send(void* const conn_info, const uint8_t* const data, const uint16_t len)
{
	mg_send(conn_info, data, len);
}

void connection_get_ip_addr(void* conn_info, char buffer[COTS_IP_ADDR_BUFFER_SIZE])
{
	int sz = mg_conn_addr_to_str(conn_info, buffer, COTS_IP_ADDR_BUFFER_SIZE, MG_SOCK_STRINGIFY_IP|MG_SOCK_STRINGIFY_REMOTE);
	buffer[sz] = '\0';
}


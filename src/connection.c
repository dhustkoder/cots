#include <stdio.h>
#include "mongoose.h"
#include "netmsg.h"
#include "memory.h"
#include "connection.h"


static struct mg_mgr mgr;
static const char* const url = "tcp://localhost:7171";
void(*connection_clbk)(struct netmsg netmsg);

static void ev_handler(struct mg_connection* const nc, int ev, void* const p)
{
	((void)nc);
	((void)p);

	switch (ev) {
	case MG_EV_RECV: {
		
		printf("MG RECV EV\n");
		
		printf("\nFULL RECV MESSAGE: [\n");
		
		for (size_t i = 0; i < nc->recv_mbuf.len - 1; ++i) {
			printf("$%.2x, \n", (uint8_t)nc->recv_mbuf.buf[i]);
		}

		printf("$%.2x\n]\n\n", nc->recv_mbuf.buf[nc->recv_mbuf.len - 1]);

		struct netmsg netmsg = {
			.len = memread_u16(nc->recv_mbuf.buf),
			.buf = (uint8_t*) nc->recv_mbuf.buf + 2
		};

		connection_clbk(netmsg);

		break;
	}
	
	case MG_EV_SEND:
		printf("MG SEND EV\n");
		break;

	default:
		printf("NS UNKNOWN\n");
		break;
	}
}


bool connection_init(void(*clbk)(struct netmsg netmsg))
{
	mg_mgr_init(&mgr, NULL);
	mg_bind(&mgr, url, ev_handler);
	connection_clbk = clbk;
	return true;
}

void connection_term(void)
{
	mg_mgr_free(&mgr);
}

void connection_poll(int ms)
{
	mg_mgr_poll(&mgr, ms);
}



#include <stdio.h>
#include "fossa.h"
#include "connection.h"


static struct ns_mgr mgr;
static const char* const url = "tcp://localhost:7171";


static void ev_handler(struct ns_connection* const nc, int ev, void* const p)
{
	((void)nc);
	((void)p);
	switch (ev) {
	case NS_RECV:
		printf("RECV EV\n");
		{
			struct mbuf* const io = &nc->recv_mbuf;
			printf("PROTOCOL ID: %" PRIu8 "\n"
			       "CLIENT OS: %" PRIu16 "\n"
			       "CLIENT VERSION: %" PRIu16 "\n",
			       *((uint8_t*)&io->buf[0]),
			       *((uint16_t*)&io->buf[3]),
			       *((uint16_t*)&io->buf[5]));
			printf("\n");
		}
		break;
	case NS_SEND:
		printf("SEND EV\n");
		break;
	}
}


bool connection_init(void)
{
	ns_mgr_init(&mgr, NULL);
	ns_bind(&mgr, url, ev_handler);
	return true;
}

void connection_term(void)
{
	ns_mgr_free(&mgr);
}

void connection_poll(int ms)
{
	ns_mgr_poll(&mgr, ms);
}



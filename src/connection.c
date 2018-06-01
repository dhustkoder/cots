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



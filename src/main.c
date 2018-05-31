#include "fossa.h"


static void ev_handler(struct ns_connection *nc, int ev, void *p) 
{
	struct mbuf *io = &nc->recv_mbuf;
	(void) p;

	switch (ev) {
	case NS_RECV:
		{
			printf("package len: %ld\n", io->len);
			for (int i = 0; i < io->len; ++i) {
				uint8_t byte = io->buf[i];
				if (byte >= 1 && byte <= 127 && byte != '\n' && byte != '\t') {
					printf("\'%c\', ", (char)io->buf[i]);
				} else {
					printf("%X, ", ((uint8_t)io->buf[i]));
				}
			}
			printf("\n");
		}
		mbuf_remove(io, io->len);        // Discard message from recv buffer
		break;
	default:
		break;
	}
}


int main(void) 
{
	struct ns_mgr mgr;
	const char *port1 = "tcp://192.168.0.104:7171";

	ns_mgr_init(&mgr, NULL);
	ns_bind(&mgr, port1, ev_handler);

	printf("Starting echo mgr on ports %s\n", port1);
	for (;;) {
		ns_mgr_poll(&mgr, 16000);
	}
	ns_mgr_free(&mgr);

	return 0;
}
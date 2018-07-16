#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdatomic.h>
#include "log.h"
#include "rsa.h"
#include "connection.h"
#include "game_protocol.h"
#include "login_protocol.h"

static atomic_bool signal_recv = false;


static void signal_handler(int sig)
{
	log_info("Received signal: %d", sig);
	atomic_store(&signal_recv, true);
}

int main(const int argc, const char* argv[]) 
{
	if (argc < 3) {
		fprintf(stderr,
		 "Usage: %s [login protocol address] [game protocol address]\n"
		 "Example: %s \"tcp://192.168.0.1:7171\" \"udp://192.168.0.1:7172\"\n",
		 argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	
	log_init();
	rsa_init();
	
	if (!connection_init(login_protocol_callback,
	                     game_protocol_callback,
	                     argv[1], argv[2])) {
		rsa_term();
		log_term();
		return EXIT_FAILURE;
	}

	atomic_init(&signal_recv, false);
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	
	while (!atomic_load(&signal_recv)) {
		connection_poll(16000);
	}

	connection_term();
	rsa_term();
	log_term();
	return EXIT_SUCCESS;;
}


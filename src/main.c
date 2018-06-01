#include <stdlib.h>
#include "connection.h"


int main(void) 
{
	connection_init();
	for (;;) {
		connection_poll(16000);
	}
	connection_term();
	return EXIT_SUCCESS;
}


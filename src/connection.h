#ifndef COTS_CONNECTION_H_
#define COTS_CONNECTION_H_
#include <stdbool.h>


bool connection_init(void);
void connection_term(void);
void connection_poll(int ms);


#endif

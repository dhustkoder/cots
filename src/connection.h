#ifndef COTS_CONNECTION_H_
#define COTS_CONNECTION_H_
#include <stdbool.h>

struct netmsg;

bool connection_init(void(*clbk)(struct netmsg netmsg));
void connection_term(void);
void connection_poll(int ms);


#endif

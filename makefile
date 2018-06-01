
INCLUDES=-Iextern -Iextern/mongoose 
CFLAGS=-std=c11 -Wall -Wextra -O0 -g -fsanitize=address $(INCLUDES) \
       -DMG_ENABLE_CALLBACK_USERDATA -DLOG_USE_COLOR -DCOTS_DEBUG
LDFLAGS=-lgmp

SRC=$(wildcard src/*.c) \
    $(wildcard extern/mongoose/*.c) \
    $(wildcard extern/logc/src/*.c)

OBJS=$(SRC:.c=.o)


all: cots

cots: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

objs/%.o: $(SRC)
	@mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(shell ls objs/*.d 2>/dev/null)

clean:
	rm -rf $(OBJS)

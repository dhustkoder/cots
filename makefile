
INCLUDES=-Iextern -Iextern/mongoose 
CFLAGS=-std=c11 -Wall -Wextra -O0 -g -fsanitize=address $(INCLUDES) \
       -DMG_ENABLE_CALLBACK_USERDATA -DMG_ENABLE_THREADS \
       -DLOG_USE_COLOR -DCOTS_DEBUG
       
LDFLAGS=-lgmp


SRC=$(wildcard src/*.c) \
    $(wildcard extern/mongoose/*.c) \
    $(wildcard extern/logc/src/*.c)


OBJS=$(patsubst %.c, objs/%.o, $(SRC))


all: cots


cots: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)


objs/%.o: %.c
	@mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) -MP -MD -c $< -o $@


-include $(shell find objs/ -name '*.d' 2>/dev/null)


clean:
	rm -rf $(OBJS)


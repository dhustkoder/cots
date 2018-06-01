
INCLUDES=-Iextern/mongoose
CFLAGS=-std=c11 -Wall -Wextra -O0 -g -fsanitize=address $(INCLUDES)
LDFLAGS=-lgmp

SRC=$(wildcard src/*.c) \
    $(wildcard extern/mongoose/*.c)
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


INCLUDES=-I../fossa
CFLAGS=-std=c11 -Wall -Wextra -O0 -g -fsanitize=address $(INCLUDES)
LDFLAGS=-lgmp

SRC=src/%.c
OBJS=$(patsubst src/%.c, objs/%.o, $(wildcard src/*.c))

all: cots

cots: $(OBJS)
	$(CC) $(CFLAGS) ../fossa/*.c $^ -o $@ $(LDFLAGS)

objs/%.o: $(SRC)
	@mkdir -p objs
	$(CC) $(CFLAGS) -MP -MD -c $< -o $@

-include $(shell ls objs/*.d 2>/dev/null)

clean:
	rm -rf objs

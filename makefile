
CFLAGS=-O0 -g -fsanitize=address
INCS=-I../fossa
SRC=src/*.c ../fossa/*.c
LIBS=

all:
	$(CC) $(CFLAGS) $(INCS) $(SRC) $(LIBS) -o cots


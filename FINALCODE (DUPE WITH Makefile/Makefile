CC = gcc
CFLAGS = -I. -lcurl -lcjson
OBJ = main.o headerfile1.o email.o

all: main

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f *.o main


CC     = gcc
CFLAGS = --std=gnu89 -O3

all: gen_input

gen_input: gen_input.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) gen_input

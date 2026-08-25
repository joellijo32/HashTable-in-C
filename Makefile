CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -g -Isrc
LDFLAGS = -lm

SRC     = src/hash_table.c src/prime.c

.PHONY: all test clean

all: build/hashtable

build/hashtable: $(SRC) src/main.c | build
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

build/test_hashtable: $(SRC) src/test_hashtable.c | build
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test: build/test_hashtable
	./build/test_hashtable

build:
	mkdir -p build

clean:
	rm -rf build/*

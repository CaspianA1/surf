CC = clang
OUT = surf
CFLAGS = -Wall -Wformat -lncurses -o bin/$(OUT)

all: build run

build:
	$(CC) -O3 $(CFLAGS) src/$(OUT).c

run:
	./bin/$(OUT) || (reset; printf "An error occurred.\n")

debug:
	$(CC) -ggdb3 -O0 $(CFLAGS) src/$(OUT).c
	lldb bin/$(OUT); reset
	
clean:
	rm -r bin/*

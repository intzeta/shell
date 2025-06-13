CC = cc
CCFLAGS = -std=c99 -Wall -Wextra -Wpedantic -g
CCFLAGS += -D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: dir shell

dir:
	mkdir -p ./bin

run: all
	bin/shell

shell: $(OBJ)
	$(CC) -o bin/shell $(OBJ)

%.o: %.c
	$(CC) -o $@ -c $< $(CCFLAGS)

clean:
	rm -rf $(OBJ) bin

.PHONY = all clean

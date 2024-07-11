OUT = build/silc

CFILES = $(wildcard src/*.c src/*/*.c fir/util/*.c)
OFILES = $(patsubst %.c, build/%.o, $(CFILES))

CC = gcc
FLAGS = -g -std=c11 -Wall -Wextra -pedantic
INCLUDES = -Ifir/include -Ifir/util

.Phony: all
all: $(OUT)

$(OUT): $(OFILES)
	$(CC) $(FLAGS) $^ -o $@

build/%.o: %.c
	mkdir -p $(@D)
	$(CC) -c $^ $(FLAGS) $(INCLUDES) -Isrc -o $@

.Phony: clean
clean:
	-rm -r build

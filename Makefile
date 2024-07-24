OUT = build/silc

CFILES = $(wildcard src/*.c src/*/*.c)
OFILES = $(patsubst %.c, build/%.o, $(CFILES))

CC = gcc
FLAGS = -g -std=c11 -Wall -Wextra -pedantic -fsanitize=address
INCLUDES = -Ifir/include -Ifir/util

FIR_LIB = fir/build/fir.a


.Phony: all
all: $(OUT)


$(OUT): $(FIR_LIB) $(OFILES)
	$(CC) $(FLAGS) $^ -o $@

build/%.o: %.c
	mkdir -p $(@D)
	$(CC) -c $^ $(FLAGS) $(INCLUDES) -Isrc -o $@

.Phony: clean
clean:
	-rm -r build

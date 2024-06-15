OUT = build/silc

CFILES = $(wildcard src/*.c src/*/*.c)
OFILES = $(patsubst src/%.c, build/%.o, $(CFILES))

FIR_CFILES = $(wildcard libs/fir/src/*.c libs/fir/src/*/*.c)
FIR_OFILES = $(patsubst libs/fir/src/%.c, build/fir/%.o, $(FIR_CFILES))

CC = gcc
FLAGS = -g -std=c11 -Wall -Wextra -pedantic -Werror
INCLUDES = -Ilibs/fir/include

.Phony: all
all: $(OUT)

$(OUT): $(OFILES) build/fir.a
	$(CC) $(FLAGS) $^ -o $@

build/%.o: src/%.c
	mkdir -p $(@D)
	$(CC) -c $^ $(FLAGS) $(INCLUDES) -Isrc -o $@

build/fir.a: $(FIR_OFILES)
	ar -rc $@ $^

build/fir/%.o: libs/fir/src/%.c
	mkdir -p $(@D)
	$(CC) -c $^ $(FLAGS) $(INCLUDES) -o $@

.Phony: clean
clean:
	-rm -r build

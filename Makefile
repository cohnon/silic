CC = gcc -std=c99
FLAGS = -g -Wall -Wextra -Werror -pedantic
INCLUDES = -Iinclude -Isrc

BUILD_DIR = build
OUT = $(BUILD_DIR)/silic

CFILES = $(wildcard src/*.c src/*/*.c)
OFILES = $(patsubst %.c, $(BUILD_DIR)/%.o, $(CFILES))


# commands
.Phony: all
all: $(OUT)

.Phony: clean
clean:
	rm -r $(BUILD_DIR)

$(OUT): $(OFILES)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) -c $(FLAGS) $(INCLUDES) $^ -o $@

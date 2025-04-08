CC = gcc -std=c17
FLAGS = -g -Wall -Wextra -Werror -pedantic
INCLUDES = -Iinclude -Isrc

BUILD_DIR = build
BIN = $(BUILD_DIR)/silic

CFILES = $(wildcard src/*.c src/*/*.c)
OFILES = $(patsubst %.c, $(BUILD_DIR)/%.o, $(CFILES))

# commands
.Phony: all
all: $(BIN)

.Phony: clean
clean:
	rm -r $(BUILD_DIR)


$(BIN): $(OFILES)
	$(CC) $^ -o $@

$(TEST): $(TEST_CFILES) $(BIN)
	$(CC) $(FLAGS) $(INCLUDES) $^ -o $@

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) -c $(FLAGS) $(INCLUDES) $^ -o $@

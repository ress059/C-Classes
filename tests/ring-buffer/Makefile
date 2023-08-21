BIN := bin
CC := gcc
C_STANDARD := c99
OPT := O0
DEPFLAGS = -MP -MD

INCLUDE_DIRECTORY := ../../include
SRC_DIRECTORY := ../../src/ring-buffer
BUILD_DIRECTORY := .
TEST_SRC_DIRECTORY := .

CFLAGS = -Wall -Wextra -g $(foreach D, $(INCLUDE_DIRECTORY), -I$(D)) -$(OPT) -std=$(C_STANDARD)

SRC_FILES = $(wildcard $(SRC_DIRECTORY)/*.c)
OBJECTS = $(patsubst %.c,%.o,$(SRC_FILES))

TEST_FILES = $(wildcard $(TEST_SRC_DIRECTORY)/*.c)
OBJECTS += $(patsubst %.c,%.o,$(TEST_FILES))


all: $(BIN)

$(BIN): $(OBJECTS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
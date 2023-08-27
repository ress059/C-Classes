PROJNAME := UnitTest
CC := gcc
CFLAGS := -Wall -Wextra
INC :=

BUILD_DIR := ../builds

UNITY_DIR := ../../Unity
UNITY_INC := $(UNITY_DIR)/src
UNITY_INC += $(UNITY_DIR)/extras/memory/src/
INC += $(UNITY_INC)
UNITY_SRC := $(UNITY_DIR)/src/unity.c
UNITY_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o, $(notdir $(UNITY_SRC)))

SRC_DIR := ../../src
SRC_INC := ../../include
INC += $(SRC_INC)
SRC := $(SRC_DIR)/ring-buffer/ring_buffer_static.c
SRC_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o, $(notdir $(SRC)))

TEST_DIR := ./
TEST_INC := ./
INC += $(TEST_INC)
TEST_SRC := $(wildcard *.c)
TEST_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o, $(notdir $(TEST_SRC)))


all: $(PROJNAME)

debug:
# @echo $(CC) $(CFLAGS) $(foreach D, $(INC), -I$(D)) -o $@ $^
	@echo $(UNITY_OBJECTS)
	@echo $(SRC_OBJECTS)
	@echo $(TEST_OBJECTS)
	@echo $(wildcard $(BUILD_DIR)/*.o)

$(PROJNAME): $(UNITY_OBJECTS) $(SRC_OBJECTS) $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $(foreach D, $(INC), -I$(D)) -o $(BUILD_DIR)/$@ $^

$(UNITY_OBJECTS): $(UNITY_SRC)
# @mkdir $(BUILD_DIR)
	$(CC) $(CFLAGS) $(foreach D, $(INC), -I$(D)) -c -o $@ $^

$(SRC_OBJECTS): $(SRC)
	$(CC) $(CFLAGS) $(foreach D, $(INC), -I$(D)) -c -o $@ $^

$(TEST_OBJECTS): $(TEST_SRC)
	$(CC) $(CFLAGS) $(foreach D, $(INC), -I$(D)) -c -o $@ $^

clean:
	rm -f $(wildcard $(BUILD_DIR)/*.o)

# .PHONY: all clean
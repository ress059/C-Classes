# EXE := unittest
# CC := gcc
# C_STANDARD := c99
# CPPFLAGS = -Wall -Wextra -g -std=$(C_STANDARD)
# # OPT := O0
# # DEPFLAGS = -MP -MD

# # SRC = ../../src
# # INC = ../../include
# # SRC_SUBDIRS = $(wildcard $(SRC)/*)

# # CFLAGS = -Wall -Wextra -g $(foreach D, $(INCLUDE_DIRECTORY), -I$(D)) -$(OPT) -std=$(C_STANDARD)


# # DIRECTORIES. HARDCODED
# UNITY_HOME = ../../Unity
# SRC_INC = ../../include
# TEST_INC = ../header-overrides


# # Include Paths and Linker
# CFLAGS += -I$(UNITY_HOME)/src
# CFLAGS += -I$(SRC_INC)/
# CFLAGS += -I$(TEST_INC)/
# CFLAGS += -I.
# LDFLAGS +=
# # CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h # Optional include for memory leak debugging
# # CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h # Optional include for memory leak debugging


# # Unit Tests. HARDCODED
# TEST_SRC_FILES = test_ring_buffer_static.c
# TEST_SRC_FILES += $(wildcard $(UNITY_HOME)/src/*.c)



# # Objects.
# OBJECTS = $(patsubst %.c, %.o, $(TEST_SRC_FILES))


# # Make
# all: $(EXE)

# $(EXE): $(OBJECTS)
# 	$(CC) $< -o $@ $(LDFLAGS)

# $(UNITY_HOME)/src/unity.o: $(UNITY_HOME)/src/unity.c test_ring_buffer_static.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# test_ring_buffer_static.o: $(UNITY_HOME)/src/unity.o test_ring_buffer_static.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# clean:




# WORKING
# CC := gcc
# CFLAGS := -Wall -Wextra -IUnity

# UNITY_DIR := ../../Unity/src
# UNITY_SRC := $(UNITY_DIR)/unity.c

# TEST_SRC := test_ring_buffer_static.c

# UNITY_OBJECTS := $(UNITY_SRC:.c=.o)
# TEST_OBJECTS := $(TEST_SRC:.c=.o)

# all: test_example

# test_example: $(UNITY_OBJECTS) $(TEST_OBJECTS)
# 	$(CC) $(CFLAGS) -o $@ $^

# $(UNITY_OBJECTS): $(UNITY_SRC)
# 	$(CC) $(CFLAGS) -c -o $@ $<

# $(TEST_OBJECTS): $(TEST_SRC)
# 	$(CC) $(CFLAGS) -c -o $@ $<

# clean:
# 	rm -f test_example $(UNITY_OBJECTS) $(TEST_OBJECTS)

# .PHONY: all clean

PROJNAME := UnitTest
CC := gcc
CFLAGS := -Wall -Wextra
INC :=

UNITY_DIR := ../../Unity
UNITY_INC := $(UNITY_DIR)/src
UNITY_INC += $(UNITY_DIR)/extras/memory/src/
UNITY_SRC := $(UNITY_DIR)/src/unity.c
UNITY_OBJECTS := $(patsubst %.c, %.o, $(UNITY_SRC))

SRC_INC := ../../include
SRC := ../../src/ring-buffer/ring_buffer_static.c
SRC_OBJECTS := $(patsubst %.c, %.o, $(SRC))

TEST_INC := ./
TEST_SRC := $(wildcard *.c)
TEST_OBJECTS := $(patsubst %.c, %.o, $(TEST_SRC))

INC += $(UNITY_INC)
INC += $(SRC_INC)
INC += $(TEST_INC)

all: $(PROJNAME)

debug:
	@echo $(CC) $(CFLAGS) $(foreach D, $(INC), -I$(D)) -o $@ $^

$(PROJNAME): $(UNITY_OBJECTS) $(SRC_OBJECTS) $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $(foreach D, $(INC), -I$(D)) -o $@ $^

$(UNITY_OBJECTS): $(UNITY_SRC)
	$(CC) $(CFLAGS) $(foreach D, $(INC), -I$(D)) -c -o $@ $^

$(SRC_OBJECTS): $(SRC)
	$(CC) $(CFLAGS) $(foreach D, $(INC), -I$(D)) -c -o $@ $^

$(TEST_OBJECTS): $(TEST_SRC)
	$(CC) $(CFLAGS) $(foreach D, $(INC), -I$(D)) -c -o $@ $^

# clean:
# 	rm -f test_example $(UNITY_OBJECTS) $(SRC_OBJECTS) $(TEST_OBJECTS)

# .PHONY: all clean
# Compile in Linux environment. Do not use Windows.

# # Build Directory where all artifiacts are placed in. On Windows you need \ instead of /........
# WINDOWS_BACKSLASH:=\\

# ifeq ($(OSTYPE),cygwin)
# 	CLEANUP=rm -f
# 	MKDIR=mkdir -p
# 	TARGET_EXTENSION=out
# 	BUILD_DIR:=./builds/
# else ifeq ($(OS),Windows_NT)
# 	CLEANUP=del /F /Q
# 	MKDIR=mkdir
# 	TARGET_EXTENSION=exe
# 	BUILD_DIR:=.$(WINDOWS_BACKSLASH)builds$(WINDOWS_BACKSLASH)
# else
# 	CLEANUP=rm -f
# 	MKDIR=mkdir -p
# 	TARGET_EXTENSION=out
# 	BUILD_DIR:=./builds/
# endif

CLEANUP:=rm -f
MKDIR:=mkdir
TARGET_EXTENSION=out
BUILD_DIR:=./builds


# Unity
UNITY_INC_DIR=../Unity/src
UNITY_INC_DIR+=../Unity/extras/memory/src
UNITY_SRC_DIR=../Unity/src
UNITY_SRC_DIR+=../Unity/extras/memory/src
UNITY_SRC_FILES:=$(foreach dir, $(UNITY_SRC_DIR), $(wildcard $(dir)/*.c))
UNITY_OBJ_FILES:=$(patsubst %.c,$(BUILD_DIR)/%.o, $(notdir $(UNITY_SRC_FILES)))


# Classes Under Test.
CLASSES_INC_DIR:=../include
CLASSES_SRC_DIR:=../src
CLASSES_SRC_FILES:=$(wildcard $(CLASSES_SRC_DIR)/**/*.c)
CLASSES_OBJ_FILES:=$(patsubst %.c,$(BUILD_DIR)/%.o, $(notdir $(CLASSES_SRC_FILES)))


# Unit Tests
UNIT_TESTS_INC_DIR:=./src
UNIT_TESTS_SRC_DIR:=./src
UNIT_TESTS_SRC_FILES:=$(wildcard $(UNIT_TESTS_SRC_DIR)/*.c)
UNIT_TESTS_OBJ_FILES:=$(patsubst %.c,$(BUILD_DIR)/%.o, $(notdir $(UNIT_TESTS_SRC_FILES)))
UNIT_TESTS_EXECUTABLES:=$(patsubst %.c,$(BUILD_DIR)/%.$(TARGET_EXTENSION), $(notdir $(UNIT_TESTS_SRC_FILES)))


# All Include Paths
ALL_INC=$(UNITY_INC_DIR)
ALL_INC+=$(CLASSES_INC_DIR)
ALL_INC+=$(UNIT_TESTS_INC_DIR)


# To automatically support adding more Class subdirectories (i.e. adding folder to src/), our Makefile 
# Rules discard the stem ($*) returned from Static Patterns and relies on VPATH to search for Source Files.
# First Line = Classes Source Code. Second Line = Unit Tests. Third Line = Unity.
# The Unit Tests and Unity directories remain constant. Note this does not support Class subsubdirectories.
# I.e. Adding src/foo is supported. Adding src/foo/bar is not supported. 
VPATH=$(foreach dir, $(wildcard $(CLASSES_SRC_DIR)/**), $(dir):)
VPATH+=src:
VPATH+=../Unity/src:../Unity/extras/memory/src:../Unity/extras/fixture/src


# Compiler Flags
CC:=gcc
CFLAGS:=-Wall -Wextra -fno-common
# -MP = if header changes force Make to recompile. -MD = create .d dependency file.
DEPFLAGS:=-MP -MD
OPT:=-O0
CSTANDARD:=-std=c99
DEFINES=APPLICATION_UNIT_TEST_


# Include dependency files if they exist
-include $(wildcard $(BUILD_DIR)/*.d)


# Make
all: $(UNIT_TESTS_EXECUTABLES)

# Unit Test executables depend on its .o. Note that executable and .o must be in same Build Directory.
$(UNIT_TESTS_EXECUTABLES) : %.$(TARGET_EXTENSION) : %.o | $(BUILD_DIR)
	$(CC) -o $@ $< $(CLASSES_OBJ_FILES) $(UNITY_OBJ_FILES) $(foreach dir,$(ALL_INC),-I$(dir))

# Unit Test .o depends on its .c, Source .o's, and Unity .o's. Secondary Expansion results in
# just .c File Name. Make automatically searches VPATHS for correct Source File Path.
.SECONDEXPANSION:
$(UNIT_TESTS_OBJ_FILES): %.o: $$(notdir %).c $(CLASSES_OBJ_FILES) $(UNITY_OBJ_FILES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(OPT) $(CSTANDARD) $(foreach dir,$(ALL_INC),-I$(dir)) $(foreach define,$(DEFINES),-D$(define)) -c $< -o $@

# Source .o's depend on their .c's
$(CLASSES_OBJ_FILES) : %.o : $$(notdir %).c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(OPT) $(CSTANDARD) $(foreach dir,$(CLASSES_INC_DIR),-I$(dir)) $(foreach define,$(DEFINES),-D$(define)) -c $< -o $@

# Unity .o's depend on their .c's
$(UNITY_OBJ_FILES) : %.o : $$(notdir %).c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(OPT) $(CSTANDARD) $(foreach dir,$(UNITY_INC_DIR),-I$(dir)) $(foreach define,$(DEFINES),-D$(define)) -c $< -o $@

$(BUILD_DIR):
	$(MKDIR) $(BUILD_DIR)

debug:
	@echo $(VPATH)
	@echo $(UNITY_OBJ_FILES)
	@echo $(CLASSES_OBJ_FILES)
	@echo $(UNIT_TESTS_OBJ_FILES)
	@echo $(UNIT_TESTS_EXECUTABLES)

.PHONY: clean
clean: $(BUILD_DIR)
	$(CLEANUP) $(wildcard $(BUILD_DIR)/*.d)
	$(CLEANUP) $(wildcard $(BUILD_DIR)/*.o)



# TODO: #define ENABLE_PRINTF in test1. Do not in test2. In both tests call a printf function.
# Verify makefile is working correctly. test1 should compile. test2 should not compile.
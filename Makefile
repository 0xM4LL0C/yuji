SHELL := /bin/sh

CC = gcc
SRC_DIR := src
OBJ_DIR := .build/obj
BUILD_DIR := .build
BIN_NAME := yuji
BIN_PATH := $(BUILD_DIR)/$(BIN_NAME)

SOURCES := $(shell find $(SRC_DIR) -name '*.c')
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

CFLAGS  ?= -Iinclude -std=gnu99 -Wall -Wextra -Wshadow -Wconversion
LDFLAGS ?=

.PHONY: all debug release build clean test rebuild

all: debug

debug:
	@mkdir -p .build
	bear --output .build/compile_commands.json -- $(MAKE) debug-build

debug-build: CFLAGS += -g3 -DYUJI_DEBUG
debug-build: LDFLAGS += -fsanitize=address,leak,undefined -ftrapv
debug-build: build

release: CFLAGS += -O2
release: build

build: $(BIN_PATH)

$(BIN_PATH): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

rebuild:
	rm -f $(BIN_PATH)
	rm -f $(OBJECTS)
	$(MAKE)

test: debug
	$(BIN_PATH) test.yuji

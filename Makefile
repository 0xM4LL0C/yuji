SHELL := /bin/sh

CC = gcc
CFLAGS = -Iinclude -std=gnu99 -Wall -Wextra -Wshadow -Wconversion -g3
LDFLAGS = -fsanitize=address,leak,undefined -ftrapv

SRC_DIR := src
OBJ_DIR := .build/obj
BUILD_DIR := .build
BIN_NAME := yuji
BIN_PATH := $(BUILD_DIR)/$(BIN_NAME)

SOURCES := $(shell find $(SRC_DIR) -name '*.c')
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

.PHONY: all build clean run rebuild

all:
	@mkdir -p ./.build
	bear --output ./.build/compile_commands.json -- $(MAKE) -j$$(nproc) build

build: $(BIN_PATH) $(OBJECTS)

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
	$(MAKE) build

run: build
	$(BIN_PATH) test.yuji

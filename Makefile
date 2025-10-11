SHELL := /bin/sh

YUJI_VERSION_MAJOR ?= 0
YUJI_VERSION_MINOR ?= 2
YUJI_VERSION_PATCH ?= 0
YUJI_VERSION_STRING := $(YUJI_VERSION_MAJOR).$(YUJI_VERSION_MINOR).$(YUJI_VERSION_PATCH)

CC = gcc
SRC_DIR := src
OBJ_DIR := .build/obj
BUILD_DIR := .build
BIN_NAME := yuji
BIN_PATH := $(BUILD_DIR)/$(BIN_NAME)

SOURCES := $(shell find $(SRC_DIR) -name '*.c')
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

CFLAGS  ?= -Iinclude -std=gnu99 -Wall -Wextra -Wshadow -Wconversion \
	-DYUJI_VERSION_MAJOR=$(YUJI_VERSION_MAJOR) \
	-DYUJI_VERSION_MINOR=$(YUJI_VERSION_MINOR) \
	-DYUJI_VERSION_PATCH=$(YUJI_VERSION_PATCH) \
	-DYUJI_VERSION_STRING="\"$(YUJI_VERSION_STRING)\""
LDFLAGS ?= -lm

UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

ifeq ($(UNAME_S),Darwin)
    ifeq ($(UNAME_M),arm64)
        SANITIZERS = address,undefined
    else
        SANITIZERS = address,leak,undefined
    endif
else
    SANITIZERS = address,leak,undefined
endif

PREFIX ?= /usr/local
BIN_INSTALL_PATH := $(PREFIX)/bin/$(BIN_NAME)

.PHONY: all debug release build clean test rebuild install uninstall

all: debug

debug:
	@mkdir -p $(BUILD_DIR)
	bear --output $(BUILD_DIR)/compile_commands.json -- $(MAKE) debug-build

debug-build: CFLAGS += -g3 -DYUJI_DEBUG
debug-build: LDFLAGS += -fsanitize=$(SANITIZERS) -ftrapv
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

install: $(BIN_PATH)
	@echo "Installing $(BIN_NAME) to $(BIN_INSTALL_PATH)..."
	@mkdir -p $(dir $(BIN_INSTALL_PATH))
	@cp $(BIN_PATH) $(BIN_INSTALL_PATH)
	@chmod +x $(BIN_INSTALL_PATH)
	@echo "Installed."

uninstall:
	@echo "Removing $(BIN_INSTALL_PATH)..."
	@rm -f $(BIN_INSTALL_PATH)
	@echo "Uninstalled."

benchmark: clean release
	hyperfine -N \
        "./.build/yuji benchmark/factorial.yuji" \
        "python3 benchmark/factorial.py" \
        "lua benchmark/factorial.lua"

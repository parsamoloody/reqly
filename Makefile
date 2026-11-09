CC ?= gcc
CFLAGS ?= -Wall -Wextra -Wpedantic -std=c11

GTK_CFLAGS := $(shell pkg-config --cflags gtk4 2>/dev/null)
GTK_LIBS := $(shell pkg-config --libs gtk4 2>/dev/null)

INCLUDES = -Iinclude
ALL_CFLAGS = $(CFLAGS) $(INCLUDES) $(GTK_CFLAGS) -MMD -MP

BUILD_DIR = build
SRC_DIR = src
TARGET = $(BUILD_DIR)/reqly

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

.PHONY: all run clean check-deps

all: check-deps $(TARGET)

check-deps:
	@which pkg-config >/dev/null 2>&1 || { echo "Error: pkg-config is not installed. Please install pkg-config and libgtk-4-dev."; exit 1; }
	@pkg-config --exists gtk4 || { echo "Error: GTK 4 development libraries not found. Please install libgtk-4-dev."; exit 1; }

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJS) -o $@ $(GTK_LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(ALL_CFLAGS) -c $< -o $@

-include $(DEPS)

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)


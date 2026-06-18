CC = gcc
CFLAGS = -Wall -Wextra -std=c11
SRC_DIR = src
OUT_DIR = out
TARGET = $(OUT_DIR)/network_sim

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OUT_DIR)/%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) | $(OUT_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

clean:
	rm -rf $(OUT_DIR)

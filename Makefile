CC	  := gcc
CFLAGS  := -g -O0 -Wall -w
SRC_DIR := src
BUILD   := build
OUT	 := $(BUILD)/aym

C_SRCS  := $(wildcard $(SRC_DIR)/*.c)
CPP_SRCS:= $(wildcard $(SRC_DIR)/*.cpp)

C_OBJS   := $(patsubst $(SRC_DIR)/%.c, $(BUILD)/%.o, $(C_SRCS))
CPP_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD)/%.o, $(CPP_SRCS))
OBJS	 := $(C_OBJS) $(CPP_OBJS)

.PHONY: all clean
all: $(OUT)

$(OUT): $(OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./$(OUT)

clean:
	rm -rf $(BUILD)

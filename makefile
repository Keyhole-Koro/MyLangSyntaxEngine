CC = gcc
CFLAGS = -Wall -Wextra

SRC = $(shell find src -name '*.c')
INC_DIRS = $(shell find inc -type d)
INCLUDE = $(foreach dir,$(INC_DIRS),-I$(dir))

OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
OBJ = $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(SRC))

BUILD_DIR = build
TARGET = $(BIN_DIR)/output

all: $(OBJ_DIR) $(BIN_DIR) $(TARGET)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf $(OBJ) $(TARGET) $(BUILD_DIR)

valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

run: $(BIN_DIR) $(TARGET)
	./$(TARGET)

.PHONY: all clean valgrind run

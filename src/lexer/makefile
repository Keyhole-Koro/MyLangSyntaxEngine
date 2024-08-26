CC = clang
LD = lld
LIBS = -lpcre
CFLAGS = -Wall -Wextra -std=c99 $(LIBS)
LDFLAGS = -fuse-ld=$(LD)
EXECUTABLE = output
SRC = $(wildcard ./*.c)
INCLUDE_DIR = ./../../inc
BUILD_DIR = ./build

INC_FILES := $(wildcard ./inc/*.h)

$(shell mkdir -p $(BUILD_DIR))

all: $(BUILD_DIR)/$(EXECUTABLE)

$(BUILD_DIR)/$(EXECUTABLE): $(SRC)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(BUILD_DIR)/$(EXECUTABLE)

run: $(BUILD_DIR)/$(EXECUTABLE)
	./$(BUILD_DIR)/$(EXECUTABLE)

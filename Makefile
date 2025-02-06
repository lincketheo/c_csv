CC       := gcc
CFLAGS   := -Wall -std=c99 -I$(HOME)/.local/include
LDFLAGS  := -L$(HOME)/.local/lib
LIBS     := -lcunit

SRC      := csv.c
OBJ      := $(SRC:.c=.o)
TARGET   := libcsv.a

TEST_SRC := test.c
TEST_BIN := test_csv
MAIN_SRC := main.c
MAIN_BIN := main

BUILD_DIR := build
BIN_DIR   := $(BUILD_DIR)/bin
LIB_DIR   := $(BUILD_DIR)/lib

$(shell mkdir -p $(BIN_DIR) $(LIB_DIR))

.PHONY: all clean format test

all: $(LIB_DIR)/$(TARGET) $(BIN_DIR)/$(MAIN_BIN)

$(LIB_DIR)/$(TARGET): $(OBJ)
	ar rcs $@ $^

$(BIN_DIR)/$(MAIN_BIN): $(MAIN_SRC) $(LIB_DIR)/$(TARGET)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/$(TEST_BIN): $(TEST_SRC) $(LIB_DIR)/$(TARGET)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

format:
	clang-format -i *.c *.h

test: $(BIN_DIR)/$(TEST_BIN)
	$(BIN_DIR)/$(TEST_BIN)

clean:
	rm -rf $(BUILD_DIR) *.o


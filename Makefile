# Define the build directory
BUILD_DIR := build
SRC_DIR := src
# List of source files
TEST_SRC := $(wildcard $(SRC_DIR)/tests/*.c)
CLIENT_SRC := $(wildcard $(SRC_DIR)/client/*.c)
SERVER_SRC := $(wildcard $(SRC_DIR)/server/*.c)
COMMON_SRC := $(filter-out $(TEST_SRC) $(CLIENT_SRC) $(SERVER_SRC), $(wildcard $(SRC_DIR)/*.c))

# Object files
SERVER_OBJ := $(patsubst $(SRC_DIR)/server/%.c, $(BUILD_DIR)/%.o, $(SERVER_SRC))
CLIENT_OBJ := $(patsubst $(SRC_DIR)/client/%.c, $(BUILD_DIR)/%.o, $(CLIENT_SRC))
COMMON_OBJ := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(COMMON_SRC))
TEST_OBJ := $(patsubst $(SRC_DIR)/tests/%.c, $(BUILD_DIR)/%.o, $(TEST_SRC))
# Compiler options
CC := g++
CFLAGS := -Wall -O2
DBGFLAGS := -g

# Output executable
CLIENT := $(BUILD_DIR)/client
SERVER := $(BUILD_DIR)/server
TEST := $(BUILD_DIR)/tests
DEBUG := $(BUILD_DIR)/debug

# Object file compilation rule
$(BUILD_DIR)/%.o: $(SRC_DIR)/**/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Create the build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Debugging target
$(DEBUG): $(TEST_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) $(DBGFLAGS) -o $(DEBUG) $(TEST_OBJ) $(COMMON_OBJ)

# Clean rule
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Default target
.PHONY: client
client: $(CLIENT_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_OBJ) $(COMMON_OBJ)

.PHONY: server
server: $(SERVER_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_OBJ) $(COMMON_OBJ)

.PHONY: debug
debug: $(DEBUG)

.PHONY: all
all: client server

.PHONY: print
print:
	@echo $(COMMON_SRC)

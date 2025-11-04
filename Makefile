# Directories
BUILD_DIR := build
SRC_DIR := src
INC_DIR := include
TEST_DIR := tests

# Files
SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
TESTS := $(shell find $(TEST_DIR) -name "*.cpp")
OBJS := $(foreach obj, $(SRCS:%.cpp=%.o), $(BUILD_DIR)/$(obj))
EXES := $(foreach exe, $(TESTS:%.cpp=%.exe), $(BUILD_DIR)/$(notdir $(exe))) # Create exe for each test

# Compiler settings
CC := g++
CFLAGS := -I$(INC_DIR)

# Targets
all: $(EXES)

$(EXES): $(BUILD_DIR)/%.exe: $(TEST_DIR)/%.cpp $(OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@ $(DEPS)

$(BUILD_DIR)/sim.exe: $(TEST_DIR)/sim.cpp $(OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@ $(DEPS)

$(OBJS): $(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ $(DEPS)


clean:
	rm -r $(BUILD_DIR)
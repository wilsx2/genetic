# Directories
BUILD_DIR := build
SRC_DIR := src
INC_DIR := include
EX_DIR := examples

# Files
EXAMPLES := $(shell find $(EX_DIR) -name "*.cpp")
OBJS := $(foreach obj, $(SRCS:%.cpp=%.o), $(BUILD_DIR)/$(obj))
EXES := $(foreach exe, $(EXAMPLES:%.cpp=%.exe), $(BUILD_DIR)/$(notdir $(exe))) # Create exe for each test

# Compiler settings
CC := g++
CFLAGS := -std=c++20 -I$(INC_DIR)

# Targets
all: $(EXES)

$(EXES): $(BUILD_DIR)/%.exe: $(EX_DIR)/%.cpp $(OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@ $(DEPS)

$(OBJS): $(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ $(DEPS)


clean:
	rm -r $(BUILD_DIR)
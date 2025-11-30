# Directories
BUILD_DIR := build
SRC_DIR := src
INC_DIR := include
EX_DIR := examples

# Files
EXAMPLES := $(shell find $(EX_DIR) -name "*.cpp")
EXES := $(foreach exe, $(EXAMPLES:%.cpp=%.exe), $(BUILD_DIR)/$(notdir $(exe))) # Create exe for each example

# Compiler settings
CC := g++
CFLAGS := -std=c++20 -I$(INC_DIR)
DEPS := -lsfml-graphics -lsfml-window -lsfml-system

# Targets
all: $(EXES)

$(EXES): $(BUILD_DIR)/%.exe: $(EX_DIR)/%.cpp $(OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@ $(DEPS) $(DEPS)


clean:
	rm -r $(BUILD_DIR)
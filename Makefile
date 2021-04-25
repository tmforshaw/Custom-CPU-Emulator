PROJECT_NAME = CUSTOM_CPU

BUILD_DIR := bin
RESOURCE_DIR := resources
OBJ_DIR := lib
SRC_DIR := src
INCLUDE_DIR := include

ASM_EXT := cus_asm

CC = clang++
C_FLAGS = -std=c++2a -I$(INCLUDE_DIR) -Wall # -O3
LINK_FLAGS = 

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard, $(SRC_DIR),*.cpp)
RES_SRC = $(call rwildcard, $(RESOURCE_DIR), *.$(ASM_EXT))

OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC)) # SRC objects

# Build the project
.PHONY: build
build: $(OBJS) $(BUILD_DIR)/$(PROJECT_NAME).bin
	@echo !-- Built $(PROJECT_NAME).bin --!

# Create the necessary folders
.PHONY: setup
setup:
	@echo !-- Setting Up Environment --!
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OBJ_DIR)

# Remove unneeded files
.PHONY: clean
clean:
	@echo !-- Cleaning Folders --!
	@rm -rf $(BUILD_DIR)/*
	@rm -rf $(OBJ_DIR)/*

# Remove unneeded files and dependency objects
.PHONY: full-clean
full-clean:
	@echo !-- Cleaning Folders - Including Dependencies --!
	@rm -rf $(BUILD_DIR)/*
	@rm -rf $(OBJ_DIR)/*

# Compile the individual cpp files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo !-- Compiling $^ --!
	@mkdir -p $(@D)
	@$(CC) -c $^ -o $@ $(C_FLAGS)

# Link the files together
$(BUILD_DIR)/$(PROJECT_NAME).bin: $(OBJS)
	@echo !-- Linking $^ --!
	@$(CC) -o $@ $^ $(LINK_FLAGS)

# Run the project as an executable
.PHONY: run
run: build
	@echo !-- Running --!
	@echo # Console padding
	@./$(BUILD_DIR)/$(PROJECT_NAME).bin $(RES_SRC)

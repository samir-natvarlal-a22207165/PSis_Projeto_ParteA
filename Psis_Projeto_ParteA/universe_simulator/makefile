# Makefile for Space Trash Simulator - Part A
# Works on macOS and WSL (Linux)

# Detect OS
UNAME_S := $(shell uname -s)

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g -O2

# Libraries
LIBS = -lconfig

# Detect platform and adjust library paths
ifeq ($(UNAME_S),Darwin)
    # macOS
    $(info Detected macOS)
    # Add Homebrew paths if needed
    ifneq ($(wildcard /opt/homebrew/include),)
        CFLAGS += -I/opt/homebrew/include
        LDFLAGS += -L/opt/homebrew/lib
    endif
    ifneq ($(wildcard /usr/local/include),)
        CFLAGS += -I/usr/local/include
        LDFLAGS += -L/usr/local/lib
    endif
else ifeq ($(UNAME_S),Linux)
    # Linux (WSL)
    $(info Detected Linux/WSL)
    CFLAGS += -I/usr/include
    LDFLAGS += -L/usr/lib
endif

# Source files
CONFIG_SRCS = config.c test_config.c
CONFIG_OBJS = $(CONFIG_SRCS:.c=.o)

# Targets
.PHONY: all clean test help

all: test_config

# Test configuration program
test_config: config.o test_config.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	@echo "Built test_config successfully for $(UNAME_S)"

# Pattern rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dependencies
config.o: config.c config.h
test_config.o: test_config.c config.h

# Run test
test: test_config
	@echo "Running configuration test..."
	./test_config

# Clean
clean:
	rm -f *.o test_config
	@echo "Cleaned build files"

# Help
help:
	@echo "Space Trash Simulator - Makefile"
	@echo "================================"
	@echo "Targets:"
	@echo "  all          - Build all programs (default)"
	@echo "  test_config  - Build configuration test program"
	@echo "  test         - Build and run configuration test"
	@echo "  clean        - Remove compiled files"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Current platform: $(UNAME_S)"
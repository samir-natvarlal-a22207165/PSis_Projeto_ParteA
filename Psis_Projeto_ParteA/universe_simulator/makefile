# Makefile for Space Trash Simulator - Part A
# Works on macOS and WSL (Linux)

# Detect OS
UNAME_S := $(shell uname -s)

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g -O2

# Libraries
LIBS_CONFIG = -lconfig
LIBS_SDL = -lSDL2 -lSDL2_ttf

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
    # macOS specific SDL2 framework path
    CFLAGS += -I/opt/homebrew/include/SDL2 -I/usr/local/include/SDL2
else ifeq ($(UNAME_S),Linux)
    # Linux (WSL)
    $(info Detected Linux/WSL)
    CFLAGS += -I/usr/include -I/usr/include/SDL2
    LDFLAGS += -L/usr/lib
endif

# Source files
CONFIG_SRCS = config.c
DISPLAY_SRCS = display.c
UNIVERSE_DATA_SRCS = universe-data.c
PHYSICS_RULES_SRCS = physics-rules.c
SIMULATOR_SRCS = universe-simulator.c

# Object files
CONFIG_OBJS = $(CONFIG_SRCS:.c=.o)
DISPLAY_OBJS = $(DISPLAY_SRCS:.c=.o)
UNIVERSE_DATA_OBJS = $(UNIVERSE_DATA_SRCS:.c=.o)
PHYSICS_RULES_OBJS = $(PHYSICS_RULES_SRCS:.c=.o)
SIMULATOR_OBJS = $(SIMULATOR_SRCS:.c=.o)

ALL_OBJS = $(CONFIG_OBJS) $(DISPLAY_OBJS) $(UNIVERSE_DATA_OBJS) $(PHYSICS_RULES_OBJS) $(SIMULATOR_OBJS)

# Targets
.PHONY: all clean test help simulator test-data test-physics

all: universe-simulator

# Universe simulator
universe-simulator: $(CONFIG_OBJS) $(DISPLAY_OBJS) $(UNIVERSE_DATA_OBJS) $(PHYSICS_RULES_OBJS) $(SIMULATOR_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS_CONFIG) $(LIBS_SDL) -lm
	@echo "Built universe-simulator successfully for $(UNAME_S)"

simulator: universe-simulator

# Test configuration program (from previous step)
test_config: config.o test_config.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS_CONFIG)
	@echo "Built test_config successfully for $(UNAME_S)"

# Test universe data structures
test_universe_data: config.o universe-data.o test_universe_data.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS_CONFIG) -lm
	@echo "Built test_universe_data successfully for $(UNAME_S)"

# Test physics rules
test_physics: config.o universe-data.o physics-rules.o test_physics.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS_CONFIG) -lm
	@echo "Built test_physics successfully for $(UNAME_S)"

# Pattern rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dependencies
config.o: config.c config.h
display.o: display.c display.h config.h
universe-data.o: universe-data.c universe-data.h config.h
physics-rules.o: physics-rules.c physics-rules.h universe-data.h
universe-simulator.o: universe-simulator.c config.h display.h universe-data.h physics-rules.h
test_config.o: test_config.c config.h
test_universe_data.o: test_universe_data.c universe-data.h config.h
test_physics.o: test_physics.c universe-data.h physics-rules.h config.h

# Run simulator
run: universe-simulator
	@echo "Running universe simulator..."
	./universe-simulator

# Run test
test: test_config
	@echo "Running configuration test..."
	./test_config

# Run data structure tests
test-data: test_universe_data
	@echo "Running universe data structure tests..."
	./test_universe_data

# Run physics tests
test-physics: test_physics
	@echo "Running physics rules tests..."
	./test_physics

# Clean
clean:
	rm -f $(ALL_OBJS) test_config.o test_universe_data.o test_physics.o universe-simulator test_config test_universe_data test_physics
	@echo "Cleaned build files"

# Help
help:
	@echo "Space Trash Simulator - Makefile"
	@echo "================================"
	@echo "Targets:"
	@echo "  all              - Build universe-simulator (default)"
	@echo "  universe-simulator - Build the universe simulator"
	@echo "  simulator        - Alias for universe-simulator"
	@echo "  test_config      - Build configuration test program"
	@echo "  test_universe_data - Build data structure test program"
	@echo "  test_physics     - Build physics test program"
	@echo "  run              - Build and run universe simulator"
	@echo "  test             - Build and run configuration test"
	@echo "  test-data        - Build and run data structure tests"
	@echo "  test-physics     - Build and run physics tests"
	@echo "  clean            - Remove compiled files"
	@echo "  help             - Show this help message"
	@echo ""
	@echo "Current platform: $(UNAME_S)"
	@echo ""
	@echo "Required libraries:"
	@echo "  - libconfig (for configuration files)"
	@echo "  - SDL2 (for graphics)"
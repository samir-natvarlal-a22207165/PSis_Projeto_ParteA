# Makefile for Universe Server/Client
# Works on macOS and WSL (Linux)

# Detect OS
UNAME_S := $(shell uname -s)

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g -O2

# Libraries
LIBS_SDL = -lSDL2 -lSDL2_image
LIBS_CONFIG = -lconfig
LIBS_ZMQ = -lzmq
LIBS_PROTO = -lprotobuf-c

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

    # macOS specific SDL2 path
    CFLAGS += -I/opt/homebrew/include/SDL2 -I/usr/local/include/SDL2

else ifeq ($(UNAME_S),Linux)
    # Linux (WSL)
    $(info Detected Linux/WSL)
    CFLAGS += -I/usr/include -I/usr/include/SDL2
    LDFLAGS += -L/usr/lib
endif

# ============================================================
# Source files
# ============================================================

# Shared communication files
PROTO_SRCS = letter-movements.pb-c.c
COMM_SRCS  = zmq-comm.c

# Server-only files
SERVER_SRCS = universe_server.c \
              config.c \
              display.c \
              universe-data.c

# Client-only files
CLIENT_SRCS = universe_client.c

# ============================================================
# Object files
# ============================================================

PROTO_OBJS  = $(PROTO_SRCS:.c=.o)
COMM_OBJS   = $(COMM_SRCS:.c=.o)

SERVER_OBJS = $(SERVER_SRCS:.c=.o)
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)

# Shared objects for both executables
COMMON_OBJS = $(PROTO_OBJS) $(COMM_OBJS)

# ============================================================
# Targets
# ============================================================

.PHONY: all clean help server client run-server run-client

# Default target builds both
all: server client

# ------------------------------------------------------------
# Build the server
# ------------------------------------------------------------
server: $(SERVER_OBJS) $(COMMON_OBJS)
	$(CC) $(LDFLAGS) -o universe_server $^ \
	    $(LIBS_CONFIG) $(LIBS_ZMQ) $(LIBS_PROTO) -lm
	@echo "Built universe_server successfully for $(UNAME_S)"

# ------------------------------------------------------------
# Build the client
# ------------------------------------------------------------
client: $(CLIENT_OBJS) $(COMMON_OBJS)
	$(CC) $(LDFLAGS) -o universe_client $^ \
	    $(LIBS_SDL) $(LIBS_ZMQ) $(LIBS_PROTO) -lm
	@echo "Built universe_client successfully for $(UNAME_S)"

# Pattern rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ------------------------------------------------------------
# File dependencies (kept same format as original)
# ------------------------------------------------------------

letter-movements.pb-c.o: letter-movements.pb-c.c letter-movements.pb-c.h
zmq-comm.o: zmq-comm.c zmq-comm.h

universe_server.o: universe_server.c config.h display.h universe-data.h zmq-comm.h
config.o: config.c config.h
display.o: display.c display.h config.h
universe-data.o: universe-data.c universe-data.h config.h

universe_client.o: universe_client.c zmq-comm.h

# ------------------------------------------------------------
# Run commands
# ------------------------------------------------------------
run-server: server
	@echo "Running universe server..."
	./universe_server

run-client: client
	@echo "Running universe client..."
	./universe_client

# ------------------------------------------------------------
# Clean
# ------------------------------------------------------------
clean:
	rm -f *.o universe_server universe_client
	@echo "Cleaned build files"

# ------------------------------------------------------------
# Help
# ------------------------------------------------------------
help:
	@echo "Universe Server/Client Makefile"
	@echo "================================"
	@echo "Targets:"
	@echo "  all          - Build server and client (default)"
	@echo "  server       - Build the server"
	@echo "  client       - Build the client"
	@echo "  run-server   - Run the server"
	@echo "  run-client   - Run the client"
	@echo "  clean        - Remove compiled files"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Current platform: $(UNAME_S)"
	@echo ""
	@echo "Required libraries:"
	@echo "  - SDL2 and SDL2_image (client)"
	@echo "  - libconfig (server)"
	@echo "  - ZeroMQ"
	@echo "  - protobuf-c"

CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L
LDFLAGS = -pthread -lrt

TARGET = aegis

SRC = src/main.c \
      src/shell.c \
      src/parser.c \
      src/process.c \
      src/signals.c \
      src/pipes.c \
      src/jobs.c \
      src/threads.c \
      src/monitor.c \
      src/builtin.c \
      src/ipc.c \
      src/shared_memory.c \
      src/sync.c \
      src/condition.c \
      src/message_queue.c \
      src/memory.c \
      src/virtual_memory.c \
      src/process_monitor.c \
      src/system_info.c \
      src/filesystem.c \
      src/diagnostics.c \
      src/help.c \
      src/inspect.c

OBJ = $(SRC:.c=.o)

INCLUDES = -Iinclude

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -pthread -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: clean

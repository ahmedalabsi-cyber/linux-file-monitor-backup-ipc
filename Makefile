CC = gcc
CFLAGS = -Wall -g
TARGET = monitor
SRCS = src/main.c src/scanner.c src/backup.c src/file_info.c src/logger.c

all:
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

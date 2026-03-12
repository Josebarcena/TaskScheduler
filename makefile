CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -pthread
TARGET = scheduler

SRC = main.c queue.c scheduler.c test.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c task.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
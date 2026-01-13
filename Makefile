CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
SRC = src/core/storage.c src/cli/main.c
OBJ = $(SRC:.c=.o)
TARGET = kivadb_cli

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET) *.kiva
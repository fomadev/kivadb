CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
# Liste de tous les fichiers sources
SRC = src/core/storage.c src/core/index.c src/core/transaction.c src/cli/main.c
OBJ = $(SRC:.c=.o)
TARGET = kivadb_cli

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Remplacer la section clean par ceci :
clean:
	@if [ -f kivadb_cli.exe ]; then rm -f src/core/*.o src/cli/*.o kivadb_cli.exe; else del /f /s *.o kivadb_cli.exe; fi
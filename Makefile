CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude

# Liste des objets nécessaires
CORE_OBJ = src/core/storage.o src/core/index.o src/core/transaction.o
CLI_OBJ = src/cli/main.o
STRESS_OBJ = src/cli/stress_test.o

all: kivadb_cli stress_test

# Compilation des fichiers Core
src/core/storage.o: src/core/storage.c
	$(CC) $(CFLAGS) -c src/core/storage.c -o src/core/storage.o

src/core/index.o: src/core/index.c
	$(CC) $(CFLAGS) -c src/core/index.c -o src/core/index.o

src/core/transaction.o: src/core/transaction.c
	$(CC) $(CFLAGS) -c src/core/transaction.c -o src/core/transaction.o

# Compilation des fichiers CLI
src/cli/main.o: src/cli/main.c
	$(CC) $(CFLAGS) -c src/cli/main.c -o src/cli/main.o

src/cli/stress_test.o: src/cli/stress_test.c
	$(CC) $(CFLAGS) -c src/cli/stress_test.c -o src/cli/stress_test.o

# Liaison (Linking)
kivadb_cli: $(CORE_OBJ) src/cli/main.o
	$(CC) $(CORE_OBJ) src/cli/main.o -o kivadb_cli

stress_test: $(CORE_OBJ) src/cli/stress_test.o
	$(CC) $(CORE_OBJ) src/cli/stress_test.o -o stress_test

clean:
	rm -f src/core/*.o src/cli/*.o kivadb_cli.exe stress_test.exe
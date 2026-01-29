CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude

# Liste des objets
CORE_OBJ = src/core/storage.o src/core/index.o src/core/transaction.o
CLI_OBJ = src/cli/main.o
STRESS_OBJ = src/cli/stress_test.o

# Cibles principales
all: kivadb stress_test

# Règle pour l'exécutable principal
kivadb: $(CORE_OBJ) $(CLI_OBJ)
	$(CC) $(CORE_OBJ) $(CLI_OBJ) -o kivadb

# Règle pour l'outil de stress test
stress_test: $(CORE_OBJ) $(STRESS_OBJ)
	$(CC) $(CORE_OBJ) $(STRESS_OBJ) -o stress_test

# Compilation des fichiers .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage robuste
clean:
	@echo Cleaning files...
	-rm -f src/core/*.o src/cli/*.o kivadb kivadb.exe stress_test stress_test.exe
	@echo Done.

# Reconstruire tout proprement
re: clean all
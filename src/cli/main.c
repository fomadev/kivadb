#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/kivadb.h"

void print_help() {
    printf("Commands: \n");
    printf("  set <key> <value>  : Store a pair\n");
    printf("  get <key>          : Retrieve a value\n");
    printf("  del <key>          : Remove a key\n");
    printf("  compact            : Clean the database file\n");
    printf("  exit               : Close KivaDB and leave\n");
}

int main() {
    KivaDB* db = kiva_open("test_store.kiva");
    if (!db) {
        printf("Error: Could not open database.\n");
        return 1;
    }

    char cmd[256], key[128], val[128];
    printf("KivaDB Shell v1.1.0 (Type 'help' for commands)\n");

    while (1) {
        printf("kiva> ");
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) break;

        // On retire le \n à la fin de la commande
        cmd[strcspn(cmd, "\n")] = 0;

        if (strncmp(cmd, "set ", 4) == 0) {
            sscanf(cmd + 4, "%s %s", key, val);
            kiva_set(db, key, val);
            printf("OK\n");
        } 
        else if (strncmp(cmd, "get ", 4) == 0) {
            sscanf(cmd + 4, "%s", key);
            char* res = kiva_get(db, key);
            if (res) {
                printf("\"%s\"\n", res);
                free(res);
            } else {
                printf("(nil)\n");
            }
        } 
        else if (strncmp(cmd, "del ", 4) == 0) {
            sscanf(cmd + 4, "%s", key);
            kiva_delete(db, key);
            printf("OK\n");
        }
        else if (strcmp(cmd, "compact") == 0) {
            kiva_compact(db);
        }
        else if (strcmp(cmd, "help") == 0) {
            print_help();
        }
        else if (strcmp(cmd, "exit") == 0) {
            break;
        }
        else if (strlen(cmd) > 0) {
            printf("Unknown command. Type 'help'.\n");
        }
    }

    kiva_close(db);
    printf("Bye!\n");
    return 0;
}
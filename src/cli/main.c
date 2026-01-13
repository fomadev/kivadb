#include <stdio.h>
#include <stdlib.h>
#include "../../include/kivadb.h"

int main() {
    printf("--- KivaDB v1.0.0 Test Suite ---\n");

    KivaDB* db = kiva_open("test_store.kiva");
    if (!db) return 1;

    // Test SET
    printf("Setting [test_key] -> [Hello Kiva]...\n");
    kiva_set(db, "test_key", "Hello Kiva");

    // Test DELETE
    printf("Deleting [test_key]...\n");
    kiva_delete(db, "test_key");

    char* val = kiva_get(db, "test_key");
    if (val == NULL) {
        printf("Success: [test_key] is no longer in memory.\n");
    }

    kiva_close(db);

    // Test PERSISTENCE de la suppression
    printf("\n--- Testing Persistence of Deletion ---\n");
    db = kiva_open("test_store.kiva");
    char* val_restart = kiva_get(db, "test_key");
    
    if (val_restart == NULL) {
        printf("Success: [test_key] is still gone after restart!\n");
    } else {
        printf("Failure: [test_key] was found: %s\n", val_restart);
        free(val_restart);
    }

    kiva_close(db);
    return 0;
}
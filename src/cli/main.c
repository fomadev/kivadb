#include <stdio.h>
#include <stdlib.h>
#include "../../include/kivadb.h"

int main() {
    printf("--- KivaDB v1.0.0 Test Suite ---\n");

    // 1. Ouvrir la base de données
    KivaDB* db = kiva_open("test_store.kiva");
    if (!db) {
        fprintf(stderr, "Error: Could not open the database file.\n");
        return 1;
    }

    // 2. Écrire une donnée
    printf("Setting [session_id] -> [ABC-123]...\n");
    kiva_set(db, "session_id", "ABC-123");

    // 3. Lire la donnée immédiatement
    char* val = kiva_get(db, "session_id");
    if (val) {
        printf("Retrieved value: %s\n", val);
        free(val);
    }

    // 4. Fermer la base
    kiva_close(db);
    printf("Database closed.\n");

    // 5. Ré-ouvrir pour tester la PERSISTANCE
    printf("\n--- Testing Persistence (Re-opening) ---\n");
    db = kiva_open("test_store.kiva");
    
    char* persisted_val = kiva_get(db, "session_id");
    if (persisted_val) {
        printf("Success! Value recovered after restart: %s\n", persisted_val);
        free(persisted_val);
    } else {
        printf("Failure: The value was lost.\n");
    }

    kiva_close(db);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../include/kivadb.h"

#define NUM_ENTRIES 10000

int main() {
    KivaDB* db = kiva_open("stress_test.kiva");
    if (!db) return 1;

    printf("Starting Stress Test: Inserting %d entries...\n", NUM_ENTRIES);

    clock_t start = clock();

    for (int i = 0; i < NUM_ENTRIES; i++) {
        char key[32];
        char val[64];
        sprintf(key, "key_%d", i);
        sprintf(val, "value_data_for_%d", i);
        
        kiva_set(db, key, val);
        
        if (i % 2000 == 0 && i > 0) {
            printf("Progress: %d entries written...\n", i);
        }
    }

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\n--- Results ---\n");
    printf("Total time: %.4f seconds\n", time_spent);
    printf("Speed: %.2f ops/sec\n", NUM_ENTRIES / time_spent);

    // Vérification rapide d'une clé au milieu
    char* check = kiva_get(db, "key_5000");
    printf("Verification (key_5000): %s\n", check ? check : "NOT FOUND");
    if (check) free(check);

    kiva_close(db);
    return 0;
}
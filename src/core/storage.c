#include <stdlib.h>
#include <string.h>
#include "kivadb_internal.h"

// Algorithme de hachage DJB2 : rapide et efficace pour les chaînes de caractères
unsigned long hash_function(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % HASH_SIZE;
}

KivaDB* kiva_open(const char* path) {
    KivaDB* db = malloc(sizeof(KivaDB));
    if (!db) return NULL;

    db->path = strdup(path);
    // Mode "ab+" : ouvre en lecture/écriture, crée si inexistant, écrit à la fin
    db->file = fopen(path, "ab+");
    
    if (!db->file) {
        free(db->path);
        free(db);
        return NULL;
    }

    // Initialiser l'index (table de hachage) à NULL
    for (int i = 0; i < HASH_SIZE; i++) {
        db->index[i] = NULL;
    }

    // TODO: v1.0.0 - Scanner le fichier existant pour reconstruire l'index
    // Pour l'instant, nous retournons une DB prête pour de nouvelles écritures.
    
    return db;
}
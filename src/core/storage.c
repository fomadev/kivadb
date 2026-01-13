#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "kivadb_internal.h"
#include "../../include/kivadb.h"

// --- UTILITAIRES INTERNES ---

// Algorithme de hachage DJB2
unsigned long hash_function(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % HASH_SIZE;
}

// --- API PUBLIQUE ---

KivaDB* kiva_open(const char* path) {
    KivaDB* db = malloc(sizeof(KivaDB));
    if (!db) return NULL;

    db->path = strdup(path);
    // "ab+" permet de lire partout et d'écrire toujours à la fin
    db->file = fopen(path, "ab+");
    
    if (!db->file) {
        free(db->path);
        free(db);
        return NULL;
    }

    for (int i = 0; i < HASH_SIZE; i++) {
        db->index[i] = NULL;
    }

    // Note : Dans une version complète, on ajouterait ici une fonction 
    // kiva_load_index(db) pour relire le fichier et remplir la RAM.
    
    return db;
}

KivaStatus kiva_set(KivaDB* db, const char* key, const char* value) {
    if (!db || !key || !value) return FILE_ERR_WRITE;

    uint32_t k_size = (uint32_t)strlen(key);
    uint32_t v_size = (uint32_t)strlen(value);

    // 1. Positionner le curseur à la fin pour l'écriture
    fseek(db->file, 0, SEEK_END);
    long offset_start = ftell(db->file);

    // 2. Écrire l'entrée : [KSize][VSize][Key][Value]
    fwrite(&k_size, sizeof(uint32_t), 1, db->file);
    fwrite(&v_size, sizeof(uint32_t), 1, db->file);
    fwrite(key, 1, k_size, db->file);
    fwrite(value, 1, v_size, db->file);
    fflush(db->file);

    // 3. Calculer l'offset exact où commence la VALEUR
    long value_offset = offset_start + (sizeof(uint32_t) * 2) + k_size;

    // 4. Mettre à jour l'index en mémoire
    unsigned long h = hash_function(key);
    HashNode* node = db->index[h];

    while (node) {
        if (strcmp(node->key, key) == 0) {
            node->entry.offset = value_offset;
            node->entry.v_size = v_size;
            return KIVA_OK;
        }
        node = node->next;
    }

    // Nouvelle clé : insertion en tête de liste (collision chaining)
    HashNode* new_node = malloc(sizeof(HashNode));
    if (!new_node) return KIVA_ERR_MALLOC;
    new_node->key = strdup(key);
    new_node->entry.offset = value_offset;
    new_node->entry.v_size = v_size;
    new_node->next = db->index[h];
    db->index[h] = new_node;

    return KIVA_OK;
}

char* kiva_get(KivaDB* db, const char* key) {
    if (!db || !key) return NULL;

    unsigned long h = hash_function(key);
    HashNode* node = db->index[h];

    // Chercher la clé dans l'index
    while (node) {
        if (strcmp(node->key, key) == 0) {
            // Allouer de la mémoire pour la valeur (+1 pour le \0)
            char* value = malloc(node->entry.v_size + 1);
            if (!value) return NULL;

            // Aller lire directement au bon endroit sur le disque
            fseek(db->file, node->entry.offset, SEEK_SET);
            fread(value, 1, node->entry.v_size, db->file);
            value[node->entry.v_size] = '\0'; // Fin de chaîne C

            return value;
        }
        node = node->next;
    }

    return NULL; // Clé non trouvée
}

void kiva_close(KivaDB* db) {
    if (!db) return;

    // Libérer l'index en mémoire
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode* node = db->index[i];
        while (node) {
            HashNode* tmp = node;
            node = node->next;
            free(tmp->key);
            free(tmp);
        }
    }

    fclose(db->file);
    free(db->path);
    free(db);
}
#include "kivadb_internal.h"
#include <string.h>
#include <stdlib.h>

// Algorithme de hachage
unsigned long hash_function(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % HASH_SIZE;
}

// Supprimer une clé de l'index RAM
void index_remove(KivaDB* db, const char* key) {
    unsigned long h = hash_function(key);
    HashNode* node = db->index[h];
    HashNode* prev = NULL;

    while (node) {
        if (strcmp(node->key, key) == 0) {
            if (prev) prev->next = node->next;
            else db->index[h] = node->next;
            free(node->key);
            free(node);
            return;
        }
        prev = node;
        node = node->next;
    }
}

// Ajouter ou mettre à jour une clé dans l'index RAM
void index_set(KivaDB* db, const char* key, long offset, uint32_t v_size) {
    unsigned long h = hash_function(key);
    HashNode* node = db->index[h];

    while (node) {
        if (strcmp(node->key, key) == 0) {
            node->entry.offset = offset;
            node->entry.v_size = v_size;
            return;
        }
        node = node->next;
    }

    HashNode* new_node = malloc(sizeof(HashNode));
    if (new_node) {
        new_node->key = strdup(key);
        new_node->entry.offset = offset;
        new_node->entry.v_size = v_size;
        new_node->next = db->index[h];
        db->index[h] = new_node;
    }
}

void index_scan(KivaDB* db) {
    printf("--- Current Keys in Database ---\n");
    int count = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode* node = db->index[i];
        while (node) {
            // On affiche la clé et la taille de sa valeur sur le disque
            printf("  -> %s (%u bytes)\n", node->key, node->entry.v_size);
            node = node->next;
            count++;
        }
    }
    printf("Total: %d keys found.\n", count);
}
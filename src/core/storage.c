#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// On inclut les fichiers du projet
#include "../../include/kivadb.h"
#include "kivadb_internal.h"

// --- INTERNAL UTILS ---

unsigned long hash_function(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % HASH_SIZE;
}

// Rebuilds the RAM index from the disk file
static void kiva_load_index(KivaDB* db) {
    fseek(db->file, 0, SEEK_SET);
    uint32_t k_size, v_size;

    while (fread(&k_size, sizeof(uint32_t), 1, db->file) == 1) {
        if (fread(&v_size, sizeof(uint32_t), 1, db->file) != 1) break;
        
        char* key = malloc(k_size + 1);
        if (!key) return;
        
        fread(key, 1, k_size, db->file);
        key[k_size] = '\0';

        long value_offset = ftell(db->file);
        
        unsigned long h = hash_function(key);
        HashNode* node = db->index[h];
        int found = 0;
        
        while (node) {
            if (strcmp(node->key, key) == 0) {
                node->entry.offset = value_offset;
                node->entry.v_size = v_size;
                found = 1;
                break;
            }
            node = node->next;
        }

        if (!found) {
            HashNode* new_node = malloc(sizeof(HashNode));
            if (new_node) {
                new_node->key = key; 
                new_node->entry.offset = value_offset;
                new_node->entry.v_size = v_size;
                new_node->next = db->index[h];
                db->index[h] = new_node;
            } else {
                free(key);
            }
        } else {
            free(key);
        }

        fseek(db->file, v_size, SEEK_CUR);
    }
}

// --- PUBLIC API ---

KivaDB* kiva_open(const char* path) {
    KivaDB* db = malloc(sizeof(KivaDB));
    if (!db) return NULL;

    db->path = strdup(path);
    db->file = fopen(path, "ab+");
    if (!db->file) {
        free(db->path);
        free(db);
        return NULL;
    }

    for (int i = 0; i < HASH_SIZE; i++) db->index[i] = NULL;

    kiva_load_index(db);
    
    return db;
}

KivaStatus kiva_set(KivaDB* db, const char* key, const char* value) {
    if (!db || !key || !value) return FILE_ERR_WRITE;

    uint32_t k_size = (uint32_t)strlen(key);
    uint32_t v_size = (uint32_t)strlen(value);

    fseek(db->file, 0, SEEK_END);
    long offset_start = ftell(db->file);

    fwrite(&k_size, sizeof(uint32_t), 1, db->file);
    fwrite(&v_size, sizeof(uint32_t), 1, db->file);
    fwrite(key, 1, k_size, db->file);
    fwrite(value, 1, v_size, db->file);
    fflush(db->file);

    long value_offset = offset_start + (sizeof(uint32_t) * 2) + k_size;
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

    while (node) {
        if (strcmp(node->key, key) == 0) {
            char* value = malloc(node->entry.v_size + 1);
            if (!value) return NULL;
            fseek(db->file, node->entry.offset, SEEK_SET);
            fread(value, 1, node->entry.v_size, db->file);
            value[node->entry.v_size] = '\0';
            return value;
        }
        node = node->next;
    }
    return NULL;
}

void kiva_close(KivaDB* db) {
    if (!db) return;
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
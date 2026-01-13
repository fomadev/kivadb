#ifndef KIVADB_INTERNAL_H
#define KIVADB_INTERNAL_H

#include <stdio.h>
#include <stdint.h>
#include "../../include/kivadb.h" // Vérifie que ce chemin est correct

#define HASH_SIZE 1024

typedef struct {
    long offset;      
    uint32_t v_size;  
} KeyDirEntry;

typedef struct HashNode {
    char* key;
    KeyDirEntry entry;
    struct HashNode* next;
} HashNode;

struct KivaDB {
    FILE* file;
    char* path;
    HashNode* index[HASH_SIZE]; 
};

unsigned long hash_function(const char* str);

#endif
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../../include/kivadb.h"
#include "kivadb_internal.h"

static KivaType detect_type(const char* value) {
    if (strcmp(value, "true") == 0 || strcmp(value, "false") == 0) {
        return TYPE_BOOLEAN;
    }
    
    // Vérifie si c'est un nombre (tous les caractères sont des chiffres)
    char* endptr;
    strtol(value, &endptr, 10);
    if (*value != '\0' && *endptr == '\0') {
        return TYPE_NUMBER;
    }
    return TYPE_STRING;
}

// Chargement au démarrage
static void kiva_load_index(KivaDB* db) {
    fseek(db->file, 0, SEEK_SET);
    uint32_t k_size, v_size;
    uint8_t type_raw; // Pour lire le type sur 1 octet

    while (fread(&k_size, sizeof(uint32_t), 1, db->file) == 1) {
        if (fread(&v_size, sizeof(uint32_t), 1, db->file) != 1) break;
        if (fread(&type_raw, sizeof(uint8_t), 1, db->file) != 1) break; // Lire le type

        char* key = malloc(k_size + 1);
        fread(key, 1, k_size, db->file);
        key[k_size] = '\0';

        if (v_size == 0) {
            index_remove(db, key);
            free(key);
        } else {
            // On stocke le type lu dans l'index
            index_set(db, key, ftell(db->file), v_size, (KivaType)type_raw);
            fseek(db->file, v_size, SEEK_CUR);
            free(key);
        }
    }
}

KivaDB* kiva_open(const char* path) {
    KivaDB* db = calloc(1, sizeof(KivaDB));
    if (!db) return NULL;
    db->path = strdup(path);
    
    // On ouvre le fichier
    db->file = fopen(path, "ab+");
    if (!db->file || kiva_lock_file(db->file) == -1) {
        if (db->file) fclose(db->file);
        free(db->path); free(db);
        return NULL;
    }

    // --- OPTIMISATION : AJOUT DU BUFFER ---
    // On crée un buffer de 64 Ko (65536 octets)
    // Cela regroupe les petites écritures en une seule grosse écriture disque
    setvbuf(db->file, NULL, _IOFBF, 65536);

    kiva_load_index(db);
    return db;
}

KivaStatus kiva_set(KivaDB* db, const char* key, const char* value) {
    uint32_t k_size = strlen(key), v_size = strlen(value);
    uint8_t type = (uint8_t)detect_type(value); // Détecter le type

    fseek(db->file, 0, SEEK_END);
    long pos = ftell(db->file);

    fwrite(&k_size, sizeof(uint32_t), 1, db->file);
    fwrite(&v_size, sizeof(uint32_t), 1, db->file);
    fwrite(&type, sizeof(uint8_t), 1, db->file); // ÉCRIRE LE TYPE SUR DISQUE
    fwrite(key, 1, k_size, db->file);
    fwrite(value, 1, v_size, db->file);
    fflush(db->file);

    // Mettre à jour l'index avec le type
    index_set(db, key, pos + (sizeof(uint32_t) * 2) + sizeof(uint8_t) + k_size, v_size, (KivaType)type);
    return KIVA_OK;
}

char* kiva_get(KivaDB* db, const char* key) {
    unsigned long h = hash_function(key);
    HashNode* node = db->index[h];
    while (node) {
        if (strcmp(node->key, key) == 0) {
            char* val = malloc(node->entry.v_size + 1);
            fseek(db->file, node->entry.offset, SEEK_SET);
            fread(val, 1, node->entry.v_size, db->file);
            val[node->entry.v_size] = '\0';
            return val;
        }
        node = node->next;
    }
    return NULL;
}

KivaStatus kiva_delete(KivaDB* db, const char* key) {
    // ÉTAPE 1 : Vérifier si la clé existe dans l'index (Hash Map)
    unsigned long h = hash_function(key);
    HashNode* node = db->index[h];
    int found = 0;

    while (node) {
        if (strcmp(node->key, key) == 0) {
            found = 1;
            break;
        }
        node = node->next;
    }

    // ÉTAPE 2 : Si la clé n'existe pas, on s'arrête ici
    if (!found) {
        return KIVA_ERR_NOT_FOUND; // Ou ton code d'erreur correspondant
    }

    // ÉTAPE 3 : La clé existe, on procède à la suppression logique
    uint32_t k_size = strlen(key), v_size = 0; // v_size = 0 est notre "Tombstone"
    fseek(db->file, 0, SEEK_END);
    
    fwrite(&k_size, sizeof(uint32_t), 1, db->file);
    fwrite(&v_size, sizeof(uint32_t), 1, db->file);
    fwrite(key, 1, k_size, db->file);
    fflush(db->file);

    // ÉTAPE 4 : Retirer de la mémoire vive
    index_remove(db, key);
    
    return KIVA_OK;
}

void kiva_close(KivaDB* db) {
    if (!db) return;
    kiva_unlock_file(db->file);
    // (Ajouter ici une boucle pour libérer les HashNodes si besoin)
    fclose(db->file);
    free(db->path); free(db);
}

KivaStatus kiva_compact(KivaDB* db) {
    if (!db) return KIVA_ERR_NOT_FOUND;

    char temp_path[256];
    snprintf(temp_path, sizeof(temp_path), "%s.tmp", db->path);
    FILE* temp_file = fopen(temp_path, "wb");
    if (!temp_file) return FILE_ERR_WRITE;

    printf("Compacting database...\n");

    // Parcourir toute la Hash Map
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode* node = db->index[i];
        while (node) {
            // Lire la valeur actuelle dans l'ancien fichier
            char* val = malloc(node->entry.v_size + 1);
            fseek(db->file, node->entry.offset, SEEK_SET);
            fread(val, 1, node->entry.v_size, db->file);

            // Écrire dans le nouveau fichier (format binaire classique)
            uint32_t k_size = (uint32_t)strlen(node->key);
            uint32_t v_size = node->entry.v_size;
            
            long new_offset_start = ftell(temp_file);
            fwrite(&k_size, sizeof(uint32_t), 1, temp_file);
            fwrite(&v_size, sizeof(uint32_t), 1, temp_file);
            fwrite(node->key, 1, k_size, temp_file);
            fwrite(val, 1, v_size, temp_file);

            // Mettre à jour l'index en RAM avec la nouvelle position
            node->entry.offset = new_offset_start + (sizeof(uint32_t) * 2) + k_size;

            free(val);
            node = node->next;
        }
    }

    // Fermer et remplacer les fichiers
    fclose(db->file);
    fclose(temp_file);

    remove(db->path);           // Supprime l'ancien gros fichier
    rename(temp_path, db->path); // Renomme le petit fichier propre

    // Réouvrir le fichier proprement
    db->file = fopen(db->path, "ab+");
    kiva_lock_file(db->file);

    printf("Compaction finished successfully.\n");
    return KIVA_OK;
}

long kiva_get_file_size(const char* path) {
    FILE* fp = fopen(path, "rb");
    if (!fp) return 0;
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    return size;
}

const char* kiva_typeof(KivaDB* db, const char* key) {
    unsigned long h = hash_function(key);
    HashNode* node = db->index[h];
    while (node) {
        if (strcmp(node->key, key) == 0) {
            switch(node->entry.type) {
                case TYPE_STRING:  return "string";
                case TYPE_NUMBER:  return "number";
                case TYPE_BOOLEAN: return "boolean";
                default:           return "unknown";
            }
        }
        node = node->next;
    }
    return "undefined";
}
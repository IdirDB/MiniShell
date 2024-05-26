#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashTable.h"


// Une fonction de hachage
unsigned int hash(const char *key, int table_size) {
    unsigned int hash_value = 0;
    while (*key) {
        hash_value += (unsigned char)(*key);
        key++;
    }
    return hash_value % table_size;
}

// Initialiser la table de hachage
HashTable* create_table(int size) {
    HashTable *table = malloc(sizeof(HashTable));
    table->size = size;
    table->buckets = malloc(sizeof(Entry*) * size);

    for (int i = 0; i < size; i++) {
        table->buckets[i] = NULL;
    }

    return table;
}

//Ajouter des éléments à la table de hachage
void insert(HashTable *table, const char *key, const char *value) {
    unsigned int index = hash(key, table->size);
    Entry *new_entry = malloc(sizeof(Entry));
    new_entry->key = strdup(key);
    new_entry->value = strdup(value);
    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;
}

//Rechercher des éléments dans la table de hachage
char* search(HashTable *table, const char *key) {
    unsigned int index = hash(key, table->size);
    Entry *entry = table->buckets[index];

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL; // Clé non trouvée
}

//Supprimer des éléments de la table de hachage
void delete(HashTable *table, const char *key) {
    unsigned int index = hash(key, table->size);
    Entry *entry = table->buckets[index];
    Entry *prev = NULL;

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                table->buckets[index] = entry->next;
            }
            free(entry->key);
            free(entry->value);
            free(entry);
            return;
        }
        prev = entry;
        entry = entry->next;
    }
}

//Libérer la mémoire
void free_table(HashTable *table) {
    for (int i = 0; i < table->size; i++) {
        Entry *entry = table->buckets[i];
        while (entry) {
            Entry *temp = entry;
            entry = entry->next;
            free(temp->key);
            free(temp->value);
            free(temp);
        }
    }
    free(table->buckets);
    free(table);
}



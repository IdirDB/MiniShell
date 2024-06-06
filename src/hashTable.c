#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashTable.h"

/**
 * @brief Computes the hash value for a given key.
 * 
 * @param key The key to hash.
 * @param table_size The size of the hash table.
 * @return unsigned int The computed hash value.
 */
unsigned int hash(const char *key, int table_size) {
    unsigned int hash_value = 0;
    while (*key) {
        hash_value += (unsigned char)(*key);
        key++;
    }
    return hash_value % table_size;
}

/**
 * @brief Initializes a new hash table.
 * 
 * @param size The size of the hash table.
 * @return HashTable* Pointer to the newly created hash table.
 */
HashTable* create_table(int size) {
    HashTable *table = malloc(sizeof(HashTable));
    if (table == NULL) {
        perror("Failed to allocate memory for hash table");
        exit(EXIT_FAILURE);
    }

    table->size = size;
    table->buckets = malloc(sizeof(Entry*) * size);
    if (table->buckets == NULL) {
        perror("Failed to allocate memory for hash table buckets");
        free(table);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < size; i++) {
        table->buckets[i] = NULL;
    }

    return table;
}

/**
 * @brief Inserts a key-value pair into the hash table.
 * 
 * @param table The hash table.
 * @param key The key to insert.
 * @param value The value associated with the key.
 */
void insert(HashTable *table, const char *key, const char *value) {
    unsigned int index = hash(key, table->size);
    Entry *new_entry = malloc(sizeof(Entry));
    if (new_entry == NULL) {
        perror("Failed to allocate memory for new entry");
        return;
    }

    new_entry->key = strdup(key);
    if (new_entry->key == NULL) {
        perror("Failed to duplicate key string");
        free(new_entry);
        return;
    }

    new_entry->value = strdup(value);
    if (new_entry->value == NULL) {
        perror("Failed to duplicate value string");
        free(new_entry->key);
        free(new_entry);
        return;
    }

    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;
}

/**
 * @brief Searches for a value associated with a given key in the hash table.
 * 
 * @param table The hash table.
 * @param key The key to search for.
 * @return char* The value associated with the key, or NULL if the key is not found.
 */
char* search(HashTable *table, const char *key) {
    unsigned int index = hash(key, table->size);
    Entry *entry = table->buckets[index];

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL; // Key not found
}

/**
 * @brief Deletes a key-value pair from the hash table.
 * 
 * @param table The hash table.
 * @param key The key to delete.
 */
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

/**
 * @brief Frees the memory allocated for the hash table.
 * 
 * @param table The hash table to free.
 */
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


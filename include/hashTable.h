#ifndef HASHTABLE_H
#define HASHTABLE_H

// Structure pour les paires clé-valeur
typedef struct Entry {
    char *key;
    char *value;
    struct Entry *next; // Pointeur pour gérer les collisions par chaînage
} Entry;

// Structure pour la table de hachage
typedef struct HashTable{
    Entry **buckets; // Tableau de pointeurs vers des entrées
    int size;        // Taille du tableau
} HashTable;

unsigned int hash(const char *key, int table_size);
HashTable* create_table(int size);
void insert(HashTable *table, const char *key, const char *value);
char* search(HashTable *table, const char *key);
void delete(HashTable *table, const char *key);
void free_table(HashTable *table);
void print_table(HashTable *table);

#endif

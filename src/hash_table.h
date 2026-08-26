#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#define HT_INITIAL_BASE_SIZE ((size_t)53)
#define HT_PRIME_1 151
#define HT_PRIME_2 163

#include <stddef.h>

typedef struct {
	char* key;
	char* value;
} ht_item;

typedef struct {
	size_t size;
	size_t count;
	size_t base_size;
	ht_item** items;
} ht_hash_table;

ht_hash_table* ht_new(void);
void 	       ht_del_hash_table(ht_hash_table* ht);

void   ht_insert(ht_hash_table* ht, const char* key, const char* value);
char*  ht_search(const ht_hash_table* ht, const char* key);
void   ht_delete(ht_hash_table* ht, const char* key);


size_t ht_count(const ht_hash_table* ht);
size_t ht_capacity(const ht_hash_table* ht);


char** ht_keys(const ht_hash_table* ht, size_t* out_len);

void ht_print(const ht_hash_table* ht);

#endif

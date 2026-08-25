#ifndef HASH_TABLE_H
#define HASH_TABLE_H
#define HT_INITIAL_BASE_SIZE 53
#define HT_PRIME_1 151
#define HT_PRIME_2 163

typedef struct {
	char* key;
	char* value;
} ht_item;

typedef struct {
	int size;
	int count;
	int base_size;
	ht_item** items;
} ht_hash_table;

ht_hash_table* ht_new(void);
void ht_del_hash_table(ht_hash_table* ht);
void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* ht, const char* key);

#endif


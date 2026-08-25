#include <stdlib.h>
#include <string.h>

#include "hash_table.h"
#include "prime.h"

static ht_item HT_DELETED_ITEM = {NULL, NULL};

static ht_item* ht_new_item(const char* k, const char* v) {
	ht_item* i = malloc(sizeof(ht_item));

	if (!i) return NULL;

	i->key = strdup(k);
	i->value = strdup(v);

	if (!i->key || !i->value) {
		free(i->key); free(i->value);
		free(i);
		return NULL;
	}

	return i;
}

static ht_hash_table* ht_new_sized(const int base_size) {
	ht_hash_table* ht = malloc(sizeof(ht_hash_table));
	if (!ht) return NULL;
	ht->base_size = base_size;
	
	ht->size = next_prime(base_size);

	ht->count = 0;
	ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
	if (!ht->items) {
		free(ht);
		return NULL;
	}
	return ht;
}

ht_hash_table* ht_new(void) {
	return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

static void ht_del_item(ht_item* i) {
	free(i->key);
	free(i->value);
	free(i);
}

void ht_del_hash_table(ht_hash_table* ht) {
	for (size_t i = 0; i < ht->size; i++){
		ht_item* item = ht->items[i];
		if(item != NULL && item != &HT_DELETED_ITEM) ht_del_item(item);
	}
	free(ht->items);
	free(ht);
}
static int ht_hash(const char* s, const int a, const int m) {
	long hash = 0;
	const int len_s = strlen(s);
	for(int i = 0; i < len_s; i++) {
		hash = (hash * a + s[i]) % m;
	}
	return (int)hash;
}

static int ht_get_hash(
		const char* s, const int num_buckets, const int attempt
) {
	const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
	const int hash_b = 1 + (ht_hash(s, HT_PRIME_2, num_buckets-1));
	return (hash_a + (attempt*hash_b)) % num_buckets;
}

static void ht_resize(ht_hash_table* ht, const int base_size) {
	if(base_size < HT_INITIAL_BASE_SIZE) return;

	ht_hash_table* new_ht = ht_new_sized(base_size);
	if(!new_ht) return;
	for(size_t i = 0; i < ht->size; i++){
		ht_item* item = ht->items[i];
		if(item != NULL && item != &HT_DELETED_ITEM) ht_insert(new_ht, item->key, item->value);
	}

	ht->base_size = new_ht->base_size;
	ht->count = new_ht->count;

	const int tmp_size = ht->size;
	ht->size = new_ht->size;
	new_ht->size = tmp_size;

	ht_item** tmp_items = ht->items;
	ht->items = new_ht->items;
	new_ht->items = tmp_items;

	ht_del_hash_table(new_ht);
}

static void ht_resize_up(ht_hash_table* ht) {
	const int new_size = ht->base_size * 2;
	ht_resize(ht, new_size);
}

static void ht_resize_down(ht_hash_table* ht) {
	const int new_size = ht->base_size / 2;
	ht_resize(ht, new_size);
}

void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
	const int load = (int)((ht->count*100LL) / ht->size);
	if(load > 70) ht_resize_up(ht);
	ht_item* new_item = ht_new_item(key, value);

	if (!new_item) return;
	int index = ht_get_hash(key, ht->size, 0);
	ht_item* cur_item = ht->items[index];
	int i = 1; 
	while(cur_item != NULL && cur_item != &HT_DELETED_ITEM) {
		if(strcmp(cur_item->key, key) == 0) {
			ht_del_item(cur_item);
			ht->items[index] = new_item;
			return;
		}
		index = ht_get_hash(key, ht->size, i++);	
		cur_item = ht->items[index];
	}
	ht->items[index] = new_item;
	ht->count++;


}

char* ht_search(const ht_hash_table* ht, const char* key) {
	int index = ht_get_hash(key, ht->size, 0);
	ht_item* cur_item = ht->items[index];
	int i = 1;
	while(cur_item != NULL) {
		if(cur_item != &HT_DELETED_ITEM && strcmp(cur_item->key, key) == 0) return cur_item->value;
		index = ht_get_hash(key, ht->size, i++);
		cur_item = ht->items[index];
	}
	return NULL;

}

void ht_delete(ht_hash_table* ht, const char* key) {
	int index = ht_get_hash(key, ht->size, 0);
	ht_item* cur_item = ht->items[index];
	int i = 1;
	while(cur_item != NULL) {
		if(cur_item != &HT_DELETED_ITEM) {
			if(strcmp(cur_item->key, key) == 0) {
				ht_del_item(cur_item);
				ht->items[index] = &HT_DELETED_ITEM;
				ht->count--;
				break;
			}
		}
		index = ht_get_hash(key, ht->size, i++);
		cur_item = ht->items[index];
	}
	const int load = (int)((ht->count*100LL) / ht->size);
	if(load < 10) ht_resize_down(ht);

}




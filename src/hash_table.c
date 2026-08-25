#define _POSIX_C_SOURCE 200809L
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
		free(i->key);
		free(i->value);
		free(i);
		return NULL;
	}

	return i;
}

static ht_hash_table* ht_new_sized(const size_t base_size) {
	ht_hash_table* ht = malloc(sizeof(ht_hash_table));
	if (!ht) return NULL;
	ht->base_size = base_size;

	ht->size = next_prime(base_size);

	ht->count = 0;
	ht->items = calloc(ht->size, sizeof(ht_item*));
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
	if (ht == NULL) return;
	for (size_t i = 0; i < ht->size; i++) {
		ht_item* item = ht->items[i];
		if (item != NULL && item != &HT_DELETED_ITEM) ht_del_item(item);
	}
	free(ht->items);
	free(ht);
}

static int ht_hash(const char* s, const int a, const size_t m) {
	long hash = 0;
	const size_t len_s = strlen(s);
	for (size_t i = 0; i < len_s; i++) {
		hash = ((long)(hash * a) + (unsigned char)s[i]) % (long)m;
	}
	return (int)hash;
}

static int ht_get_hash(
		const char* s, const size_t num_buckets, const int attempt
) {
	const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
	const int hash_b = 1 + ht_hash(s, HT_PRIME_2, num_buckets - 1);
	return (int)(((size_t)hash_a + ((size_t)attempt * (size_t)hash_b)) % num_buckets);
}

static void ht_resize(ht_hash_table* ht, const size_t base_size) {
	if (base_size < HT_INITIAL_BASE_SIZE) return;

	ht_hash_table* new_ht = ht_new_sized(base_size);
	if (!new_ht) return;
	for (size_t i = 0; i < ht->size; i++) {
		ht_item* item = ht->items[i];
		if (item != NULL && item != &HT_DELETED_ITEM)
			ht_insert(new_ht, item->key, item->value);
	}

	ht->base_size = new_ht->base_size;
	ht->count = new_ht->count;

	const size_t tmp_size = ht->size;
	ht->size = new_ht->size;
	new_ht->size = tmp_size;

	ht_item** tmp_items = ht->items;
	ht->items = new_ht->items;
	new_ht->items = tmp_items;

	ht_del_hash_table(new_ht);
}

static void ht_resize_up(ht_hash_table* ht) {
	const size_t new_size = ht->base_size * 2;
	ht_resize(ht, new_size);
}

static void ht_resize_down(ht_hash_table* ht) {
	const size_t new_size = ht->base_size / 2;
	ht_resize(ht, new_size);
}

void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
	const int load = (int)((ht->count * 100LL) / ht->size);
	if (load > 70) ht_resize_up(ht);
	ht_item* new_item = ht_new_item(key, value);

	if (!new_item) return;
	int index = ht_get_hash(key, ht->size, 0);
	ht_item* cur_item = ht->items[index];
	int i = 1;
	while (cur_item != NULL && cur_item != &HT_DELETED_ITEM) {
		if (strcmp(cur_item->key, key) == 0) {
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
	while (cur_item != NULL) {
		if (cur_item != &HT_DELETED_ITEM && strcmp(cur_item->key, key) == 0)
			return cur_item->value;
		index = ht_get_hash(key, ht->size, i++);
		cur_item = ht->items[index];
	}
	return NULL;
}

void ht_delete(ht_hash_table* ht, const char* key) {
	int index = ht_get_hash(key, ht->size, 0);
	ht_item* cur_item = ht->items[index];
	int i = 1;
	while (cur_item != NULL) {
		if (cur_item != &HT_DELETED_ITEM) {
			if (strcmp(cur_item->key, key) == 0) {
				ht_del_item(cur_item);
				ht->items[index] = &HT_DELETED_ITEM;
				ht->count--;
				break;
			}
		}
		index = ht_get_hash(key, ht->size, i++);
		cur_item = ht->items[index];
	}
	const int load = (int)((ht->count * 100LL) / ht->size);
	if (load < 10) ht_resize_down(ht);
}

size_t ht_count(const ht_hash_table* ht) {
	return ht->count;
}

size_t ht_capacity(const ht_hash_table* ht) {
	return ht->size;
}

char** ht_keys(const ht_hash_table* ht, size_t* out_len) {
	if (ht->count == 0) {
		*out_len = 0;
		return NULL;
	}
	char** keys = malloc(ht->count * sizeof(char*));
	if (!keys) {
		*out_len = 0;
		return NULL;
	}
	size_t k = 0;
	for (size_t i = 0; i < ht->size; i++) {
		ht_item* item = ht->items[i];
		if (item != NULL && item != &HT_DELETED_ITEM) {
			keys[k++] = item->key;
		}
	}
	*out_len = k;
	return keys;
}

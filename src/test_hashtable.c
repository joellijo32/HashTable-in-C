#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "hash_table.h"

static void test_create_destroy(void) {
	ht_hash_table* ht = ht_new();
	assert(ht != NULL);
	assert(ht_count(ht) == 0);
	assert(ht_capacity(ht) >= HT_INITIAL_BASE_SIZE);
	ht_del_hash_table(ht);
	printf("[PASS] test_create_destroy\n");
}

static void test_insert_search(void) {
	ht_hash_table* ht = ht_new();
	assert(ht != NULL);
	ht_insert(ht, "hello", "world");
	char* val = ht_search(ht, "hello");
	assert(val != NULL);
	assert(strcmp(val, "world") == 0);
	ht_del_hash_table(ht);
	printf("[PASS] test_insert_search\n");
}

static void test_search_missing(void) {
	ht_hash_table* ht = ht_new();
	assert(ht != NULL);
	char* val = ht_search(ht, "ghost");
	assert(val == NULL);
	ht_del_hash_table(ht);
	printf("[PASS] test_search_missing\n");
}

static void test_update(void) {
	ht_hash_table* ht = ht_new();
	assert(ht != NULL);
	ht_insert(ht, "key", "v1");
	assert(ht_count(ht) == 1);
	ht_insert(ht, "key", "v2");
	assert(ht_count(ht) == 1);
	char* val = ht_search(ht, "key");
	assert(val != NULL);
	assert(strcmp(val, "v2") == 0);
	ht_del_hash_table(ht);
	printf("[PASS] test_update\n");
}

static void test_delete(void) {
	ht_hash_table* ht = ht_new();
	assert(ht != NULL);
	ht_insert(ht, "foo", "bar");
	assert(ht_count(ht) == 1);
	ht_delete(ht, "foo");
	assert(ht_count(ht) == 0);
	assert(ht_search(ht, "foo") == NULL);
	ht_del_hash_table(ht);
	printf("[PASS] test_delete\n");
}

static void test_delete_nonexistent(void) {
	ht_hash_table* ht = ht_new();
	assert(ht != NULL);
	ht_insert(ht, "a", "1");
	size_t before = ht_count(ht);
	ht_delete(ht, "zzz_not_here");
	assert(ht_count(ht) == before);
	ht_del_hash_table(ht);
	printf("[PASS] test_delete_nonexistent\n");
}

static void test_multiple_items(void) {
	ht_hash_table* ht = ht_new();
	assert(ht != NULL);
	const char* kv[][2] = {
		{"k0","v0"},{"k1","v1"},{"k2","v2"},{"k3","v3"},{"k4","v4"},
		{"k5","v5"},{"k6","v6"},{"k7","v7"},{"k8","v8"},{"k9","v9"}
	};
	for (int i = 0; i < 10; i++) ht_insert(ht, kv[i][0], kv[i][1]);
	assert(ht_count(ht) == 10);
	for (int i = 0; i < 10; i++) {
		char* v = ht_search(ht, kv[i][0]);
		assert(v != NULL);
		assert(strcmp(v, kv[i][1]) == 0);
	}
	ht_del_hash_table(ht);
	printf("[PASS] test_multiple_items\n");
}

static void test_resize_up(void) {
	ht_hash_table* ht = ht_new();
	assert(ht != NULL);
	char key[32], val[32];
	for (int i = 0; i < 120; i++) {
		snprintf(key, sizeof(key), "rkey_%d", i);
		snprintf(val, sizeof(val), "rval_%d", i);
		ht_insert(ht, key, val);
	}
	assert(ht_count(ht) == 120);
	for (int i = 0; i < 120; i++) {
		snprintf(key, sizeof(key), "rkey_%d", i);
		snprintf(val, sizeof(val), "rval_%d", i);
		char* found = ht_search(ht, key);
		assert(found != NULL);
		assert(strcmp(found, val) == 0);
	}
	ht_del_hash_table(ht);
	printf("[PASS] test_resize_up\n");
}

static void test_resize_down(void) {
	ht_hash_table* ht = ht_new();
	assert(ht != NULL);
	char key[32], val[32];
	for (int i = 0; i < 60; i++) {
		snprintf(key, sizeof(key), "dkey_%d", i);
		snprintf(val, sizeof(val), "dval_%d", i);
		ht_insert(ht, key, val);
	}
	for (int i = 0; i < 55; i++) {
		snprintf(key, sizeof(key), "dkey_%d", i);
		ht_delete(ht, key);
	}
	assert(ht_count(ht) == 5);
	for (int i = 55; i < 60; i++) {
		snprintf(key, sizeof(key), "dkey_%d", i);
		snprintf(val, sizeof(val), "dval_%d", i);
		char* found = ht_search(ht, key);
		assert(found != NULL);
		assert(strcmp(found, val) == 0);
	}
	ht_del_hash_table(ht);
	printf("[PASS] test_resize_down\n");
}

static void test_keys_accessor(void) {
	ht_hash_table* ht = ht_new();
	assert(ht != NULL);
	ht_insert(ht, "alpha", "1");
	ht_insert(ht, "beta",  "2");
	ht_insert(ht, "gamma", "3");

	size_t nkeys = 0;
	char** keys = ht_keys(ht, &nkeys);
	assert(keys != NULL);
	assert(nkeys == 3);

	for (size_t i = 0; i < nkeys; i++) {
		assert(ht_search(ht, keys[i]) != NULL);
	}
	free(keys);
	ht_del_hash_table(ht);
	printf("[PASS] test_keys_accessor\n");
}

static void test_count_capacity(void) {
	ht_hash_table* ht = ht_new();
	assert(ht != NULL);
	assert(ht_count(ht) == 0);
	assert(ht_capacity(ht) > 0);
	ht_insert(ht, "x", "y");
	assert(ht_count(ht) == 1);
	assert(ht_capacity(ht) >= ht_count(ht));
	ht_del_hash_table(ht);
	printf("[PASS] test_count_capacity\n");
}

static void test_double_delete(void) {
	ht_hash_table* ht = ht_new();
	assert(ht != NULL);
	ht_insert(ht, "dup", "val");
	ht_delete(ht, "dup");
	ht_delete(ht, "dup");
	assert(ht_count(ht) == 0);
	ht_del_hash_table(ht);
	printf("[PASS] test_double_delete\n");
}

static void test_empty_string_key(void) {
	ht_hash_table* ht = ht_new();
	assert(ht != NULL);
	ht_insert(ht, "", "empty_key_value");
	char* val = ht_search(ht, "");
	assert(val != NULL);
	assert(strcmp(val, "empty_key_value") == 0);
	ht_delete(ht, "");
	assert(ht_search(ht, "") == NULL);
	ht_del_hash_table(ht);
	printf("[PASS] test_empty_string_key\n");
}


int main(void) {
	printf("Running hash table tests...\n\n");
	test_create_destroy();
	test_insert_search();
	test_search_missing();
	test_update();
	test_delete();
	test_delete_nonexistent();
	test_multiple_items();
	test_resize_up();
	test_resize_down();
	test_keys_accessor();
	test_count_capacity();
	test_double_delete();
	test_empty_string_key();
	printf("\nAll tests passed!\n");
	return 0;
}

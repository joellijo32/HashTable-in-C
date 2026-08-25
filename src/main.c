#include <stdio.h>
#include <stdlib.h>
#include "hash_table.h"

int main(void) {
	ht_hash_table* ht = ht_new();
	if (!ht) {
		fprintf(stderr, "Failed to create hash table\n");
		return 1;
	}

	printf("Hash table created  (capacity=%zu, count=%zu)\n",
	       ht_capacity(ht), ht_count(ht));

	
	ht_insert(ht, "name",    "Joel");
	ht_insert(ht, "lang",    "C");
	ht_insert(ht, "project", "HashTable");
	ht_insert(ht, "version", "1.0");

	printf("After 4 inserts     (capacity=%zu, count=%zu)\n",
	       ht_capacity(ht), ht_count(ht));

	printf("\nDemo Test suite: \n");
	printf("\nSearching keys: \n");
	const char* keys_to_find[] = {"name", "lang", "missing", NULL};
	for (int i = 0; keys_to_find[i] != NULL; i++) {
		char* val = ht_search(ht, keys_to_find[i]);
		if (val)
			printf("  search('%s') -> '%s'\n", keys_to_find[i], val);
		else
			printf("  search('%s') -> (not found)\n", keys_to_find[i]);
	}

	printf("\nUpdation: \n");
	ht_insert(ht, "version", "2.0");
	printf("After update 'version': '%s'\n", ht_search(ht, "version"));

	printf("\nKeys Enumeration: \n");
	size_t nkeys;
	char** all_keys = ht_keys(ht, &nkeys);
	if (all_keys) {
		printf("All keys (%zu total):\n", nkeys);
		for (size_t i = 0; i < nkeys; i++)
			printf("  [%zu] %s\n", i, all_keys[i]);
		free(all_keys);
	}

	printf("\nDeletion: \n");
	ht_delete(ht, "lang");
	printf("After deleting 'lang' (capacity=%zu, count=%zu)\n",
	       ht_capacity(ht), ht_count(ht));

	ht_del_hash_table(ht);
	printf("Hash table destroyed. Done.\n");
	return 0;
}

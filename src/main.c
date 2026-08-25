#include "hash_table.h"
#include <stdio.h>

int main(){
	ht_hash_table* ht = ht_new();
	printf("\nHashTable created of size: %d\n", ht->size);
	ht_del_hash_table(ht);
}

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



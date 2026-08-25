# HashTable in C

A simple, **open-addressed, double-hashed hash table** written in C11.
Supports dynamic resizing, lazy deletion, and a key-enumeration iterator.

## Features

- **Open addressing** with **double hashing** for collision resolution
- **Dynamic resizing**: grows when load > 70%, shrinks when load < 10%
- **Lazy deletion** using a sentinel value to preserve probe chains
- **Duplicate key update**: re-inserting an existing key updates its value in-place
- String → String key-value store
- Zero external dependencies (only standard C library + `libm`)

## Algorithm Details

### Double Hashing

For a key `s` and probe attempt `i`:

```
index = (hash_a(s) + i * hash_b(s)) % num_buckets
```

Where:
- `hash_a(s) = polynomial_hash(s, PRIME_1, num_buckets)`
- `hash_b(s) = 1 + polynomial_hash(s, PRIME_2, num_buckets - 1)`

The `+1` in `hash_b` ensures it is never zero, guaranteeing full-table traversal.

### Resizing

| Load Factor | Action |
|---|---|
| > 70% | Resize up (double `base_size`) |
| < 10% | Resize down (halve `base_size`) |

The actual bucket count is always the **next prime** ≥ `base_size`.

### Lazy Deletion

Deleted slots are marked with a sentinel (`HT_DELETED_ITEM`) so that existing probe chains are not broken. The sentinel is skipped during search but treated as an empty slot during insert.

## Building & Running

Requires **GCC** and **make**.

### Build

```bash
make          # compiles (build/hashtable)
make test     # compiles and runs the test suite (build/test_hashtable)
make clean    # removes all build artifacts
```

### Run

```bash
./build/hashtable
```

Runs a demo exercising **insert**, **search**, **update**, **deletion**, and **key enumeration** on the hash table.

## API Reference

### Lifecycle

| Function | Description |
|---|---|
| `ht_hash_table* ht_new(void)` | Create a new hash table. Returns `NULL` on alloc failure. |
| `void ht_del_hash_table(ht_hash_table* ht)` | Destroy the hash table and free all memory. |

### Core Operations

| Function | Description |
|---|---|
| `void ht_insert(ht_hash_table* ht, const char* key, const char* value)` | Insert or update a key-value pair. |
| `char* ht_search(const ht_hash_table* ht, const char* key)` | Look up a key. Returns `NULL` if not found. The returned string is **owned by the table** |
| `void ht_delete(ht_hash_table* ht, const char* key)` | Delete a key. No-op if key not found. |

### Accessors

| Function | Description |
|---|---|
| `size_t ht_count(const ht_hash_table* ht)` | Number of stored key-value pairs. |
| `size_t ht_capacity(const ht_hash_table* ht)` | Total number of buckets. |
| `char** ht_keys(const ht_hash_table* ht, size_t* out_len)` | Returns a heap-allocated array of key pointers. The array must be `free()`d by the caller; the strings themselves are **owned by the table**. Returns `NULL` if the table is empty or on alloc failure. |

## Usage Example

```c
#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    ht_hash_table* ht = ht_new();

    ht_insert(ht, "name", "Alice");
    ht_insert(ht, "lang", "C");
    ht_insert(ht, "name", "Bob");   // updates existing key

    printf("%s\n", ht_search(ht, "name")); 
    printf("%s\n", ht_search(ht, "lang")); 

    // Enumerate all keys 
    size_t n;
    char** keys = ht_keys(ht, &n);
    for (size_t i = 0; i < n; i++)
        printf("key: %s\n", keys[i]);
    free(keys);   

    ht_delete(ht, "lang");
    ht_del_hash_table(ht);
    return 0;
}
```

## File Structure

```
HashTable-in-C/
├── Makefile
├── README.md
├── build/                 # compiled binaries (git-ignored)
│   ├── hashtable          # demo binary
│   └── test_hashtable     # test binary
└── src/
    ├── hash_table.h       # public API
    ├── hash_table.c       # implementation
    ├── prime.h            # prime number utilities
    ├── prime.c
    ├── main.c             # demo / manual testing entry point
    └── test_hashtable.c   # automated test suite (assert-based)
```

## License

MIT License — free to use and modify.

<p align="center">
  Made by <b>Joel Lijo Mathew</b>
</p>


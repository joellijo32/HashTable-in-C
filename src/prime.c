#include <math.h>
#include "prime.h"

int is_prime(const int x) {
	if (x < 2) return -1;
	if (x < 4) return 1;
	if ((x % 2) == 0) return 0;
	for (int i = 3; i <= (int)floor(sqrt((double)x)); i += 2) {
		if ((x % i) == 0) return 0;
	}
	return 1;
}

size_t next_prime(size_t x) {
	while (is_prime((int)x) != 1) x++;
	return x;
}

#include <stdint.h>

/*
 * Mersenne Twister implementation.
 */
#define N 624
#define M 397
#define A 0x9908b0dfUL
#define U 0x80000000UL
#define L 0x7fffffffUL

static uint64_t x[N];
static int32_t next;

void MTRSeed(uint64_t s)
{
	int32_t i;

	x[0] = s & 0xffffffffUL;

	for (i = 1; i < N; i++) {
		x[i] = (1812433253UL * (x[i-1] ^ (x[i-1] >> 30)) + i);
		x[i] &= 0xffffffffUL;
	}
}

uint64_t MTRRand()
{
	uint64_t y, a;

	if (next == N) {
		next = 0;

		for (int32_t i = 0; i < N - 1; i++) {
			y = (x[i] & U) | (x[i+1] & L);
			a = (y & 0x1UL) ? A : 0x0UL;
			x[i] = x[(i+M) % N] ^ (y >> 1) ^ a;
		}

		y = (x[N-1] & U) | (x[0] & L);
		a = (y & 0x1UL) ? A : 0x0UL;
		x[N-1] = x[M-1] ^ (y >> 1) ^ a;
	}

	y = x[next++];
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	y ^= (y >> 18);

	return y;
}

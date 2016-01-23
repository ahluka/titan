#include <stdint.h>

#define HASHSIZE(n) (1U << (n))
#define HASHMASK(n) (HASHSIZE(n) - 1)

#define MIX(a,b,c) \
{ \
    a -= b; a -= c; a ^= (c >> 13); \
    b -= c; b -= a; b ^= (a << 8); \
    c -= a; c -= b; c ^= (b >> 13); \
    a -= b; a -= c; a ^= (c >> 12); \
    b -= c; b -= a; b ^= (a << 16); \
    c -= a; c -= b; c ^= (b >> 5); \
    a -= b; a -= c; a ^= (c >> 3); \
    b -= c; b -= a; b ^= (a << 10); \
    c -= a; c -= b; c ^= (b >> 15); \
}

uint32_t JenkinsHash(uint8_t *k, uint32_t length, uint32_t initval)
{
    uint32_t a, b;
    uint32_t c = initval;
    uint32_t len = length;

    a = b = 0x9e3779b9;

    while (len >= 12)
    {
        a += (k[0] + ((uint32_t)k[1] << 8) + ((uint32_t)k[2] << 16) + ((uint32_t)k[3] << 24));
        b += (k[4] + ((uint32_t)k[5] << 8) + ((uint32_t)k[6] << 16) + ((uint32_t)k[7] << 24));
        c += (k[8] + ((uint32_t)k[9] << 8) + ((uint32_t)k[10] << 16) + ((uint32_t)k[11] << 24));

        MIX(a, b, c);

        k += 12;
        len -= 12;
    }

    c += length;

    switch (len)
    {
    case 11: c += ((uint32_t)k[10] << 24);
    case 10: c += ((uint32_t)k[9] << 16);
    case 9: c += ((uint32_t)k[8] << 8);
        /* First byte of c reserved for length */
    case 8: b += ((uint32_t)k[7] << 24);
    case 7: b += ((uint32_t)k[6] << 16);
    case 6: b += ((uint32_t)k[5] << 8);
    case 5: b += k[4];
    case 4: a += ((uint32_t)k[3] << 24);
    case 3: a += ((uint32_t)k[2] << 16);
    case 2: a += ((uint32_t)k[1] << 8);
    case 1: a += k[0];
    }

    MIX(a, b, c);

    return c;
}

uint32_t ElfHash(void *data, int32_t len)
{
	uint8_t *p = data;
	uint32_t h = 0, g;

	for (int32_t i = 0; i < len; i++) {
		h = (h << 4) + p[i];
		g = h & 0xf0000000L;

		if (g != 0)
			h ^= g >> 24;

		h &= ~g;
	}

	return h;
}

uint32_t FNVHash(void *data, int32_t len)
{
	uint8_t *p = data;
	uint32_t h = 2166136261;

	for (int i = 0; i < len; i++) {
		h = (h * 16777619) ^ p[i];
	}

	return h;
}

/*
 * Hash
 *
 */
uint32_t Hash(void *data, int32_t len)
{
	return ElfHash(data, len);
}

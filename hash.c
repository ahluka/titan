#include <stdint.h>

uint32_t Hash(void *data, int32_t len)
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

/*
 * mtrand.h
 *	Mersenne Twister PRNG. Now uses dSFMT to quickly generate numbers
 *	using SSE2.
 */
#pragma once

/* Initialise the PRNG state with the given seed.
 * Typically (uint32_t) time(NULL);
 */
void init_random(uint32_t seed);

uint32_t rand_u32();
uint32_t rand_u32_max(uint32_t max);
uint32_t rand_u32_range(uint32_t min, uint32_t max);
double rand_dbl();
float rand_flt();


// void dSFMT_Test();

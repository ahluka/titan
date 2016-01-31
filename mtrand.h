/*
 * mtrand.h
 *	Mersenne Twister PRNG. Now uses dSFMT to quickly generate numbers
 *	using SSE2.
 */
#pragma once

/* Initialise the PRNG state with the given seed.
 * Typically (uint32_t) time(NULL);
 */
void Random_Init(uint32_t seed);

uint32_t Random_u32();
uint32_t Random_u32R(uint32_t max);
uint32_t Random_u32B(uint32_t min, uint32_t max);
double Random_d();
float Random_f();


// void dSFMT_Test();

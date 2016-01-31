#include "base.h"
#include "panic.h"
#define DSFMT_MEXP 521
#include "dSFMT/dSFMT.h"

// TODO: Multiple states for different uses?
static dsfmt_t s_State = {0};

/*
 * dSFMT_Init
 */
void Random_Init(uint32_t seed)
{
	dsfmt_init_gen_rand(&s_State, seed);
}

uint32_t Random_u32()
{
	return dsfmt_genrand_uint32(&s_State);
}

uint32_t Random_u32R(uint32_t max)
{
	return Random_u32() % max;
}

uint32_t Random_u32B(uint32_t min, uint32_t max)
{
	return min + (Random_u32() % (max - min));
}

double Random_d()
{
	return dsfmt_genrand_open_open(&s_State);
}

float Random_f()
{
	return (float) dsfmt_genrand_open_open(&s_State);
}

/*
 * dSFMT_Test
 *	dsfmt_genrand_open_open() - (0,1), never 0 neither 1
 *	dsfmt_genrand_close_open() - [0,1), may be 0, never 1
 *	dsfmt_genrand_open_close() - (0,1], never 0, may be 1
 *	dsfmt_genrand_close1_open2() - [1, 2), may be 1, never 2
 */
// void dSFMT_Test()
// {
// 	Trace("Some random uint32's:");
// 	for (int i = 0; i < 10; i++) {
// 		Trace(Fmt("\t%u", dSFMT_RandR2(50, 150)));
// 	}
//
// 	Trace("Some random doubles:");
// 	for (int i = 0; i < 10; i++) {
// 		Trace(Fmt("\t%f", dSFMT_RandD()));
// 	}
// }

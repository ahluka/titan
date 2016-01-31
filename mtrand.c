#include "base.h"
#include "panic.h"
#define DSFMT_MEXP 521
#include "dSFMT/dSFMT.h"

static dsfmt_t s_State = {0};

/*
 * dSFMT_Init
 */
void dSFMT_Init(uint32_t seed)
{
	dsfmt_init_gen_rand(&s_State, seed);
}

/*
 * dSFMT_Test
 *	dsfmt_genrand_open_open() - (0,1), never 0 neither 1
 *	dsfmt_genrand_close_open() - [0,1), may be 0, never 1
 *	dsfmt_genrand_open_close() - (0,1], never 0, may be 1
 *	dsfmt_genrand_close1_open2() - [1, 2), may be 1, never 2
 */
void dSFMT_Test()
{
	Trace("Some random uint32's:");
	for (int i = 0; i < 10; i++) {
		Trace(Fmt("\t%u", dsfmt_genrand_uint32(&s_State)));
	}

	Trace("Some random doubles:");
	for (int i = 0; i < 10; i++) {
		Trace(Fmt("\t%f", dsfmt_genrand_close_open(&s_State)));
	}
}

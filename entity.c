#include "base.h"
#include "entity.h"
#include "memory.h"
#include "panic.h"

//#define DEBUG_ENT_MGMT

#define MAX_ENTITIES	32
static struct Entity *s_Entities[MAX_ENTITIES] = {NULL};

/*
 * Ent_Init
 *	Allocate the Entity pool and set them all to unused, ready for
 *	Ent_New() to make use of them.
 */
ecode_t Ent_Init()
{
	if (s_Entities[0] != NULL) {
		Trace("already called");
		return EFAIL;
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		s_Entities[i] = MemAlloc(sizeof(struct Entity *));
		s_Entities[i]->inUse = false;
	}

#ifdef DEBUG_ENT_MGMT
	Trace(Fmt("allocated %d entities", MAX_ENTITIES));
#endif

	return EOK;
}

/*
 * Ent_Shutdown
 *	Free the Entity pool and NULL it out.
 */
ecode_t Ent_Shutdown()
{
	if (s_Entities[0] == NULL) {
		Trace("already called or Ent_Init not called");
		return EFAIL;
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		MemFree(s_Entities[i]);
		s_Entities[i] = NULL;
	}

#ifdef DEBUG_ENT_MGMT
	Trace(Fmt("freed %d entities", MAX_ENTITIES));
#endif

	return EOK;
}

/*
 * ZeroEntity
 *	Set all fields in the given Entity to their zero values.
 */
static void ZeroEntity(struct Entity *ent)
{

}

/*
 * Ent_New
 *	Return a pointer to the first unused Entity in the pool.
 *	If we don't find one, Panic().
 */
struct Entity *Ent_New()
{
	if (s_Entities[0] == NULL)
		Panic("Entity pool not initialised; call Ent_Init() first");

	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (!s_Entities[i]->inUse) {
#ifdef DEBUG_ENT_MGMT
			Trace(Fmt("entity slot %d selected", i));
#endif
			ZeroEntity(s_Entities[i]);
			return s_Entities[i];
		}
	}

	Panic("No free Entities");
	return NULL; /* not reached; shuts warning up */
}

/*
 * Ent_Free
 */
ecode_t Ent_Free(struct Entity *ent)
{
	if (s_Entities[0] == NULL) {
		Trace("Attempting to free an entity before Ent_Init()");
		return EFAIL;
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (s_Entities[i] == ent) {
#ifdef DEBUG_ENT_MGMT
			Trace(Fmt("marking entity in slot %d as free", i));
#endif
			s_Entities[i]->inUse = false;
			ZeroEntity(s_Entities[i]);
			return EOK;
		}
	}

	Panic("Invalid Entity pointer");
	return EFAIL; /* not reached */
}

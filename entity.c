#include "base.h"
#include "entity.h"
#include "memory.h"
#include "panic.h"
#include "globals.h"

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
		s_Entities[i] = MemAlloc(sizeof(struct Entity));
		s_Entities[i]->inUse = false;
	}

#ifdef DEBUG_TRACING_ON
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

#ifdef DEBUG_TRACING_ON
	Trace(Fmt("freed %d entities", MAX_ENTITIES));
#endif

	return EOK;
}

/*
 * DefaultEntity
 *	Set all fields in the given Entity to their default values.
 */
ecode_t EntityDefaultUpdate(struct Entity *self, float dT)
{
	Trace("well, this can't be good");

	if (self->updateType == UPDATE_SCHED)
		self->nextUpdate = g_Globals.timeNowMs + 500;

	return EOK;
}

ecode_t EntityDefaultRender(struct Entity *self)
{
	Trace("well, this can't be good either");

	if (self->updateType == UPDATE_SCHED)
		self->nextUpdate = g_Globals.timeNowMs + 500;

	return EOK;
}

static void DefaultEntity(struct Entity *ent)
{
	ent->name = "(unnamed)";

	ent->updateType = UPDATE_ALWAYS;
	ent->nextUpdate = 0;
	ent->Update = EntityDefaultUpdate;

	ent->Render = EntityDefaultRender;
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
#ifdef DEBUG_TRACING_ON
			Trace(Fmt("entity slot %d selected", i));
#endif
			DefaultEntity(s_Entities[i]);
			s_Entities[i]->inUse = true;
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
#ifdef DEBUG_TRACING_ON
			Trace(Fmt("marking entity in slot %d as free", i));
#endif
			s_Entities[i]->inUse = false;
			DefaultEntity(s_Entities[i]);
			return EOK;
		}
	}

	Panic("Invalid Entity pointer");
	return EFAIL; /* not reached */
}

/*
 * Ent_UpdateAll
 *	Update all in-use Entities in the pool.
 */
ecode_t Ent_UpdateAll(float dT)
{
	if (s_Entities[0] == NULL) {
		Trace("Entity pool not initialised");
		return EFAIL;
	}

	/* FIXME: This should probably be refactored... */
	for (int i = 0; i < MAX_ENTITIES; i++) {
		struct Entity *ent = s_Entities[i];

		if (!ent->inUse)
			continue;

		switch (ent->updateType) {
		case UPDATE_ALWAYS:
			if (ent->Update(ent, dT) != EOK) {
				Trace(Fmt("entity '%s' in slot %d failed "
						"to update", ent->name, i));
				return EFAIL;
			}

			break;
		case UPDATE_SCHED:
			if (ent->nextUpdate <= g_Globals.timeNowMs) {
				if (ent->Update(ent, dT) != EOK) {
					Trace(Fmt("entity '%s' in slot %d"
						"failed to update", ent->name,
						i));
					return EFAIL;
				}
			}

			break;
		}
	}

	return EOK;
}

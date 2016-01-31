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
		Trace(CHAN_GENERAL, "already called");
		return EFAIL;
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		s_Entities[i] = MemAlloc(sizeof(struct Entity));
		s_Entities[i]->inUse = false;
	}

	Trace(CHAN_DBG, Fmt("allocated entity pool size %d", MAX_ENTITIES));

	return EOK;
}

/*
 * Ent_Shutdown
 *	Free the Entity pool and NULL it out.
 */
ecode_t Ent_Shutdown()
{
	if (s_Entities[0] == NULL) {
		Trace(CHAN_GENERAL, "already called or Ent_Init not called");
		return EFAIL;
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (s_Entities[i] != NULL)
			MemFree(s_Entities[i]);
		s_Entities[i] = NULL;
	}

	Trace(CHAN_DBG, Fmt("freed %d entities", MAX_ENTITIES));

	return EOK;
}

/*
 * DefaultEntity
 *	Set all fields in the given Entity to their default values.
 */
ecode_t EntityDefaultUpdate(struct Entity *self, float dT)
{
	Trace(CHAN_GAME, "well, this can't be good");

	if (self->updateType == UPDATE_SCHED)
		self->nextUpdate = g_Globals.timeNowMs + 500;

	return EOK;
}

ecode_t EntityDefaultRender(struct Entity *self)
{
	Trace(CHAN_GAME, "well, this can't be good either");

	if (self->updateType == UPDATE_SCHED)
		self->nextUpdate = g_Globals.timeNowMs + 500;

	return EOK;
}

static void DefaultEntity(struct Entity *ent)
{
	assert(ent != NULL);

	ent->name = "(unnamed)";

	ent->updateType = UPDATE_FRAME;
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
	if (s_Entities[0] == NULL) {
		Panic("Entity pool not initialised");
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (!s_Entities[i]->inUse) {
			Trace(CHAN_DBG, Fmt("entity slot %d selected", i));

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
	assert(ent != NULL);

	if (s_Entities[0] == NULL) {
		Trace(CHAN_GENERAL,
			"Attempting to free an entity before Ent_Init()");
		return EFAIL;
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (s_Entities[i] == ent) {
			Trace(CHAN_DBG,
				Fmt("marking entity in slot %d as free", i));

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
static ecode_t UpdateEntity(struct Entity *ent, float dT)
{
	switch (ent->updateType) {
	case UPDATE_FRAME:
		if (ent->Update(ent, dT) != EOK) {
			Trace(CHAN_GAME,
				Fmt("entity '%s' failed to update", ent->name));
			return EFAIL;
		}

		break;
	case UPDATE_SCHED:
		if (ent->nextUpdate <= g_Globals.timeNowMs) {
			if (ent->Update(ent, dT) != EOK) {
				Trace(CHAN_GAME,
					Fmt("entity '%s' failed to update",
					ent->name));
				return EFAIL;
			}
		}

		break;
	}

	return EOK;
}

ecode_t Ent_UpdateAll(float dT)
{
	if (s_Entities[0] == NULL) {
		Trace(CHAN_GENERAL, "Entity pool not initialised");
		return EFAIL;
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		struct Entity *ent = s_Entities[i];

		if (!ent->inUse)
			continue;

		if (UpdateEntity(ent, dT) != EOK) {
			return EFAIL;
		}
	}

	return EOK;
}

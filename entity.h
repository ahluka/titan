#pragma once
#include "list.h"

struct property {
        char *key, *val;
        struct list_head list;
};

/* Each Entity has a property table.
 */
struct property_tbl {
        struct list_head props;
        uint32_t size;
};

/* Determines how Entities are updated */
enum EntUpdateType {
	UPDATE_FRAME,	/* once per frame */
	UPDATE_SCHED	/* when game time >= Entity.nextUpdate */
};

typedef struct Entity {
	bool inUse;

        /* class is the type of entity, name is the name of the specific
         * instance of that class, and can be NULL.
         * These are actually loaded from an entity definition script and
         * will also be in the property table, but we keep pointers to them
         * here for easy access.
         */
	const char *class;
        const char *name;
        struct property_tbl properties;

	/* Updating */
	enum EntUpdateType updateType;
	uint32_t nextUpdate;
	ecode_t (*Update)(struct Entity *self, float dT);

	/* Rendering */
	ecode_t (*Render)(struct Entity *self);

} Entity;

ecode_t Ent_Init();
ecode_t Ent_Shutdown();

Entity *Ent_Spawn(const char *class);
ecode_t Ent_Free(Entity *ent);


/* Get the given property string from the entity's property table.
 * Returns NULL if it can't be found.
 * DO NOT free the string returned, it's allocated from the global string
 * pool and will be freed for you. */
const char *Ent_GetProperty(Entity *ent, const char *key);

ecode_t Ent_UpdateAll(float dT);

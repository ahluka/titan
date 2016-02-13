#pragma once
#include "list.h"
#include "vec.h"

/* Every entity update / render function is given a pointer to itself named
 * self; this macro is defined for convenience. */
#define SelfProperty(key) Ent_GetProperty(self, (key))
#define TIMENOW_PLUS(ms) g_Globals.timeNowMs + (ms)


/* Each Entity has a property table.
 */
struct property {
        char *key, *val;
        struct list_head list;
};

struct property_tbl {
        struct list_head props;         /* list of struct property */
        uint32_t size;
};

/* Determines how Entities are updated */
enum EntUpdateType {
	ENT_UPDATE_FRAME,	/* once per frame */
	ENT_UPDATE_SCHED	/* when game time >= Entity.nextUpdate */
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

        /* Position and velocity
         * These will be in the property table too, but those are only used
         * as initial values when the Entity is spawned in.
         */
        vec2_t pos;
        vec2_t vel;

	/* Updating */
	enum EntUpdateType updateType;
	uint32_t nextUpdate;
	ecode_t (*Update)(struct Entity *self, float dT);

	/* Rendering */
	ecode_t (*Render)(struct Entity *self);

} Entity;

/* These are called by base modules. */
ecode_t Ent_Init();
ecode_t Ent_Shutdown();
ecode_t Ent_UpdateAll(float dT);

/* Spawn an entity of the given class into the game world. This loads the
 * entity definition script named from the given parameter "class", so
 * Ent_Spawn("default"); loads default.ent. It returns a relatively bare-
 * bones Entity that only has a property table and Update() and Render()
 * stubs (both of which you should replace immediately).
 * After this call, the Entity returned will be processed on the next frame.
 */
Entity *Ent_Spawn(const char *class);

/* Mark the given Entity as unused and free its property table. */
ecode_t Ent_Free(Entity *ent);

/* Get the given property string from the entity's property table.
 * Returns NULL if it can't be found.
 * DO NOT free the string returned, it's allocated from the global string
 * pool and will be freed for you. */
const char *Ent_GetProperty(Entity *ent, const char *key);

/* Add the given key / value property to the given entity's property
 * table, or update its value if it already exists. */
void Ent_SetProperty(Entity *ent, const char *key, const char *val);

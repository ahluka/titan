#pragma once
#include "list.h"
#include "vec.h"

/* If you define every Entity's Update and Render functions first argument
 * as 'self' then you can use this macro for convenience. */
#define SelfProperty(key) Ent_GetProperty(self, (key))

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
enum ent_update_type {
	ENT_UPDATE_FRAME,	/* once per frame */
	ENT_UPDATE_SCHED	/* when game time >= Entity.nextUpdate */
};

typedef struct entity {
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
	enum ent_update_type update_type;
	uint32_t next_update;
	ecode_t (*update)(struct entity *self, float dT);

	/* Rendering */
        bool visible;
	ecode_t (*render)(struct entity *self);

} entity_t;

/* Spawn an entity of the given class into the game world. This loads the
 * entity definition script named from the given parameter "class", so
 * Ent_Spawn("default"); loads default.ent. It returns a relatively bare-
 * bones Entity that only has a property table and Update() and Render()
 * stubs (both of which you should replace immediately).
 * After this call, the Entity returned will be processed on the next frame.
 */
entity_t *Ent_Spawn(const char *class);

/* Mark the given Entity as unused and free its property table. */
ecode_t Ent_Free(entity_t *ent);

/* Get the given property string from the entity's property table.
 * Returns NULL if it can't be found.
 * DO NOT free the string returned, it's allocated from the global string
 * pool and will be freed for you. */
const char *Ent_GetProperty(entity_t *ent, const char *key);

/* Add the given key / value property to the given entity's property
 * table, or update its value if key already exists. */
void Ent_SetProperty(entity_t *ent, const char *key, const char *val);


/* These are called by base modules. */
ecode_t init_entities();
ecode_t shutdown_entities();
ecode_t update_entities(float dT);
ecode_t render_all_entities();

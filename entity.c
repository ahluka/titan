#include "base.h"
#include "entity.h"
#include "memory.h"
#include "panic.h"
#include "globals.h"
#include "ini.h"
#include "files.h"

/* Not using a mem_pool_t here because we need to iterate over the entities
 * all the time. I *could* use a mem_pool_t, but I'd have to either A) change
 * the mem_pool_t API to include iteration, or B) keep an array of pointers
 * like this anyway / be redundant.
 */
#define MAX_ENTITIES	64
static entity_t *s_Entities[MAX_ENTITIES] = {NULL};

#define DEFAULT_ENTDEF_FILE "./res/ent/default.ent"

/*
 * init_entities
 *	Allocate the Entity pool and set them all to unused, ready for
 *	Ent_New() to make use of them.
 */
ecode_t init_entities()
{
	if (s_Entities[0] != NULL) {
		trace(CHAN_INFO, "Entity manager already initialised");
		return EFAIL;
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		s_Entities[i] = MemAlloc(sizeof(struct entity));
		s_Entities[i]->inUse = false;
	}

	trace(CHAN_DBG, fmt("Allocated entity pool size %d", MAX_ENTITIES));

	return EOK;
}

/*
 * shutdown_entities
 *	Free the Entity pool and NULL it out.
 */
ecode_t shutdown_entities()
{
	if (s_Entities[0] == NULL) {
		trace(CHAN_INFO, "Already called or init_entities not called");
		return EFAIL;
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (s_Entities[i] != NULL) {
                        Ent_Free(s_Entities[i]);
			MemFree(s_Entities[i]);
                }

		s_Entities[i] = NULL;
	}

	trace(CHAN_DBG, fmt("Freed %d entities", MAX_ENTITIES));

	return EOK;
}

// TODO: use sections?
static int
handle_prop(void *usr, const char *sec, const char *key, const char *val)
{
        //trace(CHAN_DBG, fmt("[%s] %s=%s", sec, key, val));

        struct property_tbl *prop_tbl = usr;
        struct property *prop = MemAlloc(sizeof(*prop));
        prop->key = sstrdup_lower(key);
        prop->val = sstrdup_lower(val);
        list_add(&prop->list, &prop_tbl->props);
        prop_tbl->size++;

        return 1;
}

/*
 * load_properties
 *      Load and parse the specified entity definition file.
 */
static void load_properties(entity_t *ent, const char *entfile)
{
        assert(ent != NULL);
        assert(entfile != NULL);

        trace(CHAN_DBG, fmt("loading %s", entfile));

        if (ini_parse(entfile, handle_prop, &ent->properties) < 0) {
                panic(fmt("Failed to parse entity defintion '%s'", entfile));
        }

        trace(CHAN_DBG, fmt("loaded %u properties", ent->properties.size));
}

/*
 * free_property_table
 */
static void free_property_table(struct property_tbl *ptbl)
{
        assert(ptbl != NULL);

        struct property *i, *tmp;
        list_for_each_entry_safe(i, tmp, &ptbl->props, list) {
                sstrfree(i->key);
                sstrfree(i->val);
                list_del(&i->list);
                MemFree(i);
        }
}

/*
 * Update and render stub functions. They just panic(), because I obviously
 * forgot to replace them.
 */
ecode_t EntityDefaultUpdate(entity_t *self, float dT)
{
        const char *name = SelfProperty("name");

        panic(fmt("no update function specified for '%s'", name));
	return EOK;
}

ecode_t EntityDefaultRender(entity_t *self)
{
        const char *name = SelfProperty("name");

        panic(fmt("no render function specified for '%s'", name));
	return EOK;
}

static void set_basic_fields(entity_t *ent)
{
        ent->update_type = ENT_UPDATE_FRAME;
        ent->next_update = 0;
        ent->update = EntityDefaultUpdate;

        ent->visible = true;
        ent->render = EntityDefaultRender;

        INIT_LIST_HEAD(&ent->properties.props);
}

/*
 * Ent_New
 *	Return a pointer to the first unused Entity in the pool.
 *	If we don't find one, panic().
 */
entity_t *Ent_New()
{
	if (s_Entities[0] == NULL) {
		panic("Entity pool not initialised");
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (!s_Entities[i]->inUse) {
			trace(CHAN_DBG, fmt("Entity slot %d selected", i));

			s_Entities[i]->inUse = true;
			return s_Entities[i];
		}
	}

	panic("No free Entities");
	return NULL; /* not reached; shuts warning up */
}

/*
 * Ent_Free
 */
ecode_t Ent_Free(entity_t *ent)
{
	assert(ent != NULL);

	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (s_Entities[i] == ent) {
			// trace(CHAN_DBG,
			// 	fmt("marking entity in slot %d as free", i));

                        if (s_Entities[i]->inUse) {
        			s_Entities[i]->inUse = false;
                                free_property_table(&s_Entities[i]->properties);
                        }

			return EOK;
		}
	}

	panic("Invalid entity_t pointer");
	return EFAIL; /* not reached */
}

/*
 * parse_loaded_properties
 */
static void parse_loaded_properties(entity_t *ent, const char *entfile)
{
        /* Must have a class specified, set name to 'unnamed' if it wasn't */
        ent->class = Ent_GetProperty(ent, "class");
        if (!ent->class) {
                panic(fmt("no class defined in %s", entfile));
        }

        ent->name = Ent_GetProperty(ent, "name");
        if (!ent->name)
        	ent->name = "unnamed";

        /* If an initial position and velocity were specified, parse them */
        const char *pos = Ent_GetProperty(ent, "pos");
        const char *vel = Ent_GetProperty(ent, "vel");
        if (pos) {
                VParseStr(pos, ent->pos);
        }

        if (vel) {
                VParseStr(vel, ent->vel);
        }
}

/*
 * Ent_Spawn
 */
entity_t *Ent_Spawn(const char *class)
{
        assert(class != NULL);

        entity_t *ent = Ent_New();
        set_basic_fields(ent);

        char *root = sstrcat(get_root_path(), "ent/");
        char *full = sstrfname(root, class, ".ent");
        load_properties(ent, full);
        parse_loaded_properties(ent, full);

        sstrfree(full);
        sstrfree(root);
        return ent;
}


/*
 * Ent_GetProperty
 */
const char *Ent_GetProperty(entity_t *ent, const char *key)
{
        assert(ent != NULL);
        assert(key != NULL);

        struct property *i = NULL;
        list_for_each_entry(i, &ent->properties.props, list) {
                if (strcmp(i->key, key) == 0) {
                        return i->val;
                }
        }

        trace(CHAN_GAME, fmt("Warning: entity has no property '%s'", key));

        return NULL;
}

/*
 * Ent_SetProperty
 */
void Ent_SetProperty(entity_t *ent, const char *key, const char *val)
{
        assert(ent != NULL);
        assert(key != NULL);

        struct property *i = NULL;
        list_for_each_entry(i, &ent->properties.props, list) {
                if (strcmp(i->key, key) == 0) {
                        sstrfree(i->val);
                        i->val = sstrdup_lower(val);
                        return;
                }
        }

        handle_prop(&ent->properties, "api-set", key, val);
}

/*
 * update_entities
 *	Update all in-use Entities in the pool.
 */
static ecode_t UpdateEntity(entity_t *ent, float dT)
{
	switch (ent->update_type) {
	case ENT_UPDATE_FRAME:
		if (ent->update(ent, dT) != EOK) {
			trace(CHAN_GAME,
				fmt("entity '%s' failed to update", ent->name));
			return EFAIL;
		}

		break;
	case ENT_UPDATE_SCHED:
		if (ent->next_update <= g_globals.timeNowMs) {
			if (ent->update(ent, dT) != EOK) {
				trace(CHAN_GAME,
					fmt("entity '%s' failed to update",
					ent->name));
				return EFAIL;
			}
		}

		break;
	}

	return EOK;
}

ecode_t update_entities(float dT)
{
	if (s_Entities[0] == NULL) {
		trace(CHAN_INFO, "Entity pool not initialised");
		return EFAIL;
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		entity_t *ent = s_Entities[i];

		if (!ent->inUse)
			continue;

		if (UpdateEntity(ent, dT) != EOK)
			return EFAIL;
	}

	return EOK;
}

/*
 * render_all_entities
 *      Call each Entity's Render() function, giving them all a chance to
 *      submit render commands.
 */
ecode_t render_all_entities()
{
        if (s_Entities[0] == NULL) {
                trace(CHAN_INFO, "Entity pool not initialised");
                return EFAIL;
        }

        for (int i = 0; i < MAX_ENTITIES; i++) {
                entity_t *ent = s_Entities[i];

                if (!ent->inUse || !ent->visible)
                        continue;

                if (ent->render(ent) != EOK)
                        return EFAIL;
        }

        return EOK;
}

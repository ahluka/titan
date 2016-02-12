#include "base.h"
#include "entity.h"
#include "memory.h"
#include "panic.h"
#include "globals.h"
#include "ini.h"
#include "files.h"

/* Not using a MemPool here because we need to iterate over the entities
 * all the time. I *could* use a MemPool, but I'd have to either A) change
 * the MemPool API to include iteration, or B) keep an array of pointers
 * like this anyway / be redundant.
 */
#define MAX_ENTITIES	32
static struct Entity *s_Entities[MAX_ENTITIES] = {NULL};

#define DEFAULT_ENTDEF_FILE "./res/ent/default.ent"

/*
 * Ent_Init
 *	Allocate the Entity pool and set them all to unused, ready for
 *	Ent_New() to make use of them.
 */
ecode_t Ent_Init()
{
	if (s_Entities[0] != NULL) {
		Trace(CHAN_INFO, "Entity manager already initialised");
		return EFAIL;
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		s_Entities[i] = MemAlloc(sizeof(struct Entity));
		s_Entities[i]->inUse = false;
	}

	Trace(CHAN_DBG, Fmt("Allocated entity pool size %d", MAX_ENTITIES));

	return EOK;
}

/*
 * Ent_Shutdown
 *	Free the Entity pool and NULL it out.
 */
ecode_t Ent_Shutdown()
{
	if (s_Entities[0] == NULL) {
		Trace(CHAN_INFO, "Already called or Ent_Init not called");
		return EFAIL;
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (s_Entities[i] != NULL) {
                        Ent_Free(s_Entities[i]);
			MemFree(s_Entities[i]);
                }

		s_Entities[i] = NULL;
	}

	Trace(CHAN_DBG, Fmt("Freed %d entities", MAX_ENTITIES));

	return EOK;
}

// TODO: move make_lowercase?
static void make_lowercase(char *str)
{
        while (*str) {
                *str = tolower(*str);
                str++;
        }
}

// TODO: use sections?
static int
handle_prop(void *usr, const char *sec, const char *key, const char *val)
{
        //Trace(CHAN_DBG, Fmt("[%s] %s=%s", sec, key, val));

        struct property_tbl *prop_tbl = usr;
        struct property *prop = MemAlloc(sizeof(*prop));
        prop->key = sstrdup(key);
        make_lowercase(prop->key);
        prop->val = sstrdup(val);
        make_lowercase(prop->val);
        list_add(&prop->list, &prop_tbl->props);
        prop_tbl->size++;

        return 1;
}

/*
 * load_properties
 *      Load and parse the specified entity definition file.
 */
static void load_properties(Entity *ent, const char *entfile)
{
        assert(ent != NULL);
        assert(entfile != NULL);

        Trace(CHAN_DBG, Fmt("loading %s", entfile));

        if (ini_parse(entfile, handle_prop, &ent->properties) < 0) {
                Panic(Fmt("Failed to parse entity defintion '%s'", entfile));
        }

        Trace(CHAN_DBG, Fmt("loaded %u properties", ent->properties.size));
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
 * Update and render stub functions. They just Panic(), because I obviously
 * forgot to replace them.
 */
ecode_t EntityDefaultUpdate(struct Entity *self, float dT)
{
        const char *name = SelfProperty("name");

        Panic(Fmt("no update function specified for '%s'", name));
	return EOK;
}

ecode_t EntityDefaultRender(struct Entity *self)
{
        const char *name = SelfProperty("name");

        Panic(Fmt("no render function specified for '%s'", name));
	return EOK;
}

static void set_basic_fields(Entity *ent)
{
        ent->updateType = UPDATE_FRAME;
        ent->nextUpdate = 0;
        ent->Update = EntityDefaultUpdate;

        ent->Render = EntityDefaultRender;

        INIT_LIST_HEAD(&ent->properties.props);
}

/*
 * Ent_New
 *	Return a pointer to the first unused Entity in the pool.
 *	If we don't find one, Panic().
 */
Entity *Ent_New()
{
	if (s_Entities[0] == NULL) {
		Panic("Entity pool not initialised");
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (!s_Entities[i]->inUse) {
			Trace(CHAN_DBG, Fmt("Entity slot %d selected", i));

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
ecode_t Ent_Free(Entity *ent)
{
	assert(ent != NULL);

	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (s_Entities[i] == ent) {
			// Trace(CHAN_DBG,
			// 	Fmt("marking entity in slot %d as free", i));

                        if (s_Entities[i]->inUse) {
        			s_Entities[i]->inUse = false;
                                free_property_table(&s_Entities[i]->properties);
                        }

			return EOK;
		}
	}

	Panic("Invalid Entity pointer");
	return EFAIL; /* not reached */
}

/*
 * parse_loaded_properties
 */
static void parse_loaded_properties(Entity *ent, const char *entfile)
{
        /* Must have a class specified, set name to 'unnamed' if it wasn't */
        ent->class = Ent_GetProperty(ent, "class");
        if (!ent->class) {
                Panic(Fmt("no class defined in %s", entfile));
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
Entity *Ent_Spawn(const char *class)
{
        assert(class != NULL);

        Entity *ent = Ent_New();
        set_basic_fields(ent);

        char *root = sstrcat(Files_GetRoot(), "ent/");
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
const char *Ent_GetProperty(Entity *ent, const char *key)
{
        assert(ent != NULL);
        assert(key != NULL);

        struct property *i = NULL;
        list_for_each_entry(i, &ent->properties.props, list) {
                if (strcmp(i->key, key) == 0) {
                        return i->val;
                }
        }

        Trace(CHAN_GAME, Fmt("Warning: entity has no property '%s'", key));

        return NULL;
}

/*
 * Ent_SetProperty
 */
void Ent_SetProperty(Entity *ent, const char *key, const char *val)
{
        assert(ent != NULL);
        assert(key != NULL);

        struct property *i = NULL;
        list_for_each_entry(i, &ent->properties.props, list) {
                if (strcmp(i->key, key) == 0) {
                        sstrfree(i->val);
                        i->val = sstrdup(val);
                        return;
                }
        }

        handle_prop(&ent->properties, "api-set", key, val);
}

/*
 * Ent_UpdateAll
 *	Update all in-use Entities in the pool.
 */
static ecode_t UpdateEntity(Entity *ent, float dT)
{
	switch (ent->updateType) {
	case UPDATE_FRAME:
		if (ent->Update(ent, dT) != EOK) {
			Trace(CHAN_GAME,
				Fmt("entity '%s' failed to update", ent->class));
			return EFAIL;
		}

		break;
	case UPDATE_SCHED:
		if (ent->nextUpdate <= g_Globals.timeNowMs) {
			if (ent->Update(ent, dT) != EOK) {
				Trace(CHAN_GAME,
					Fmt("entity '%s' failed to update",
					ent->class));
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
		Trace(CHAN_INFO, "Entity pool not initialised");
		return EFAIL;
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		Entity *ent = s_Entities[i];

		if (!ent->inUse)
			continue;

		if (UpdateEntity(ent, dT) != EOK) {
			return EFAIL;
		}
	}

	return EOK;
}

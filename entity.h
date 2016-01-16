#pragma once

/*
 * Notes
 *	struct Entity {
 *		Render()
 *		struct Sprite
 */

/* Determines how Entities are updated */
enum EntUpdateType {
	UPDATE_ALWAYS,	/* once per frame */
	UPDATE_SCHED	/* when game time >= Entity.nextUpdate */
};

struct Entity {
	bool inUse;

	const char *name;

	/* Updating */
	enum EntUpdateType updateType;
	uint32_t nextUpdate;
	ecode_t (*Update)(struct Entity *self, float dT);

	/* Rendering */
	ecode_t (*Render)(struct Entity *self);

};

ecode_t Ent_Init();
ecode_t Ent_Shutdown();

struct Entity *Ent_New();
ecode_t Ent_Free(struct Entity *ent);

ecode_t Ent_UpdateAll(float dT);

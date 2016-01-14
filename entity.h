#pragma once

/*
 * Notes
 *	struct Entity {
 *		Update()
 *		nextUpdate
 *		updateType enum UPDATE_ALWAYS UPDATE_SCHED
 *		Render()
 *		struct Sprite
 */

struct Entity {
	bool inUse;


};

ecode_t Ent_Init();
ecode_t Ent_Shutdown();

struct Entity *Ent_New();
ecode_t Ent_Free(struct Entity *ent);

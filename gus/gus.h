/*
 * Gus
 *	A library for working with a sprite definition format I'm working on.
 *	The basic idea is being able to specify arbitrary data--mostly 2D
 *	vectors--within each frame of a sprite. (Other data could be specified
 *	as well.) The point of this being frame-by-frame collision detection
 *	for attack animations, etc.
 *
 * TODO: file format specification
 */
#pragma once

typedef struct GusFile GusFile;

/* Load the given .gus file into memory. Takes a valid FileHandle containing
 * the contents of a .gus file, and a pointer to a GusFile to be filled.
 * Returns EFAIL if anything goes wrong.
 */
ecode_t Gus_LoadFile(FileHandle handle, GusFile *_gf);

/* Write a loaded .gus file to disk. */
ecode_t Gus_WriteFile(GusFile *gf);

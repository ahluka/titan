#include "../base.h"
#include "../memory.h"
#include "../panic.h"
#include "../files.h"
#include "gus.h"

#define GUSFILE_ID "GUSGUS"

/* Header block for .gus files written to / read from disk. */
typedef struct GusFileHeader {
	char id[6];		/* Must match GUSFILE_ID */
	char name[64];		/* Name of this sprite */
	uint32_t frames;	/* Total number of frames */
} GusFileHeader;

/* The actual data we want to work with. Comes immediately after a
 * GusFileHeader on-disk.
 */
struct GusFile {
	// TODO: need a copy of GusFileHeader in here?
	int dummy;
};

/*
 * ValidHeader
 */
static bool ValidHeader(GusFileHeader *header)
{
	if (strncmp(header->id, GUSFILE_ID, 6) != 0) {
		return false;
	}

	return true;
}

/*
 * Gus_LoadFile
 */
ecode_t Gus_LoadFile(FileHandle handle, GusFile *_gf)
{
	uint8_t *ptr = Files_GetData(handle);
	GusFileHeader header = {0};

	memcpy(&header, ptr, sizeof(GusFileHeader));
	if (!ValidHeader(&header)) {
		Trace(Fmt("Invalid header while reading %s",
			Files_GetPath(handle)));
		return EFAIL;
	}

	memcpy(_gf, ptr + sizeof(GusFileHeader),
		Files_GetSize(handle) - sizeof(GusFileHeader));

	return EOK;
}

/*
 * Gus_WriteFile
 */
ecode_t Gus_WriteFile(GusFile *gf)
{
	return EOK;
}

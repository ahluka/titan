#include "base.h"
#include "panic.h"
#include "files.h"
#include "memory.h"
#include "cmds.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

struct File {
	bool inUse;

	char *path;
	int fd;
	size_t size;
	FileHandle handle;
	uint8_t *data;
};

#define MAX_OPENFILES	64
static struct File *s_Files[MAX_OPENFILES] = {NULL};

/* The next handle that will be returned by Files_OpenFile().
 * We start at 1 because we use 0 as invalid.
 */
static FileHandle s_NextHandle = 1;

/* All file access are relative to here */
static const char *s_FilesRoot = NULL;

const char *Files_GetRoot()
{
        return s_FilesRoot;
}

/*
 * NextFreeFile
 *	Returns a pointer to the next free File slot, or calls Panic().
 */
static struct File *NextFreeFile()
{
	for (int i = 0; i < MAX_OPENFILES; i++) {
		if (s_Files[i] == NULL || s_Files[i]->inUse == false) {
			if (s_Files[i] == NULL)
				s_Files[i] = MemAlloc(sizeof(struct File));

			//Trace(Fmt("selected slot %d", i));
			s_Files[i]->inUse = true;
			return s_Files[i];
		}
	}

	Panic(Fmt("reached MAX_OPENFILES, which is %d", MAX_OPENFILES));
	return NULL;	/* not reached; shuts up warning */
}

/*
 * GetFileByHandle
 *	Returns the struct File * associated with the given FileHandle.
 */
static struct File *GetFileByHandle(FileHandle handle)
{
	for (int i = 0; i < MAX_OPENFILES; i++) {
		if (s_Files[i]->handle == handle)
			return s_Files[i];
	}

	return NULL;
}

/*
 * DestroyFile
 *	Cleans up the given struct File *; closes the file descriptor
 *	and frees all the memory it used.
 */
static void DestroyFile(struct File *f)
{
	close(f->fd);
	f->fd = -1;
	f->handle = 0;
	MemFree(f->data);
	f->data = NULL;
	f->size = 0;
	sstrfree(f->path);
	f->path = NULL;

	f->inUse = false;
}

/*
 * Files_Init
 */
void Files_ListOpen(int argc, char **argv);
ecode_t Files_Init(const char *rootDir)
{
	Trace(CHAN_DBG, Fmt("setting root directory to %s", rootDir));
	s_FilesRoot = rootDir;

	Cmd_Register("files-listopen", Files_ListOpen);

	return EOK;
}

/*
 * Files_Shutdown
 *	Close all open files and free all buffers.
 */
ecode_t Files_Shutdown()
{
	uint32_t closed = 0;
	uint32_t used = 0;

	for (int i = 0; i < MAX_OPENFILES; i++) {
		if (s_Files[i] == NULL)
			continue;

		closed++;
		if (s_Files[i]->inUse) used++;

		DestroyFile(s_Files[i]);
		MemFree(s_Files[i]);
		s_Files[i] = NULL;
	}

	Trace(CHAN_DBG,
		Fmt("freed %u file structures (%u were in use)", closed, used));

	return EOK;
}

/*
 * Files_OpenFile
 */
static void OpenAndRead(struct File *file)
{
	file->fd = open(file->path, O_RDONLY);
	if (file->fd == -1)
		Panic(Fmt("failed to open %s", file->path));

	struct stat status;
	fstat(file->fd, &status);
	file->data = MemAlloc(status.st_size);
	file->size = status.st_size;

	/* We want read() to read the entire file at once, or we consider
	 * it to have failed.
	 */
	ssize_t ret = read(file->fd, file->data, file->size);
	if (ret == -1 || ret < file->size) {
		Panic(Fmt("read() returned %d", ret));
	}
}

FileHandle Files_OpenFile(const char *filename)
{
	assert(filename != NULL);

	char *fullPath = sstrcat(s_FilesRoot, filename);
	struct File *file = NextFreeFile();

	file->path = fullPath;
	file->handle = s_NextHandle++;
	OpenAndRead(file);

	Trace(CHAN_DBG, Fmt("opened %s (handle %u)", fullPath, file->handle));

	return file->handle;
}

/*
 * Files_CloseFile
 */
void Files_CloseFile(FileHandle handle)
{
	struct File *file = GetFileByHandle(handle);
	if (!file)
		Panic(Fmt("no file for handle %u", handle));

	Trace(CHAN_DBG, Fmt("closed %s (handle %u)", file->path, handle));
	DestroyFile(file);
}

/*
 * Files_GetData
 */
uint8_t *Files_GetData(FileHandle handle)
{
	struct File *file = GetFileByHandle(handle);
	if (!file)
		Panic(Fmt("no file for handle %u", handle));

	return file->data;
}

size_t Files_GetSize(FileHandle handle)
{
	struct File *file = GetFileByHandle(handle);
	if (!file) {
		Panic(Fmt("no file for handle %u", handle));
	}

	return file->size;
}

/*
 * Files_GetPath
 */
const char *Files_GetPath(FileHandle handle)
{
	struct File *file = GetFileByHandle(handle);
	if (!file) {
		Panic(Fmt("no file for handle %u", handle));
	}

	return file->path;
}

/*
 * Files_ListOpen
 */
void Files_ListOpen(int argc, char **argv)
{
	int totalOpen = 0;
	size_t totalBytes = 0;

	Trace(CHAN_INFO, "List of open files:");
	for (int i = 0; i < MAX_OPENFILES; i++) {
		if (s_Files[i] == NULL)
			continue;

		if (s_Files[i]->inUse) {
			struct File *f = s_Files[i];

			Trace(CHAN_INFO,
				Fmt("\t%s (%lu bytes)", f->path, f->size));
			totalOpen++;
			totalBytes += f->size;
		}
	}
	Trace(CHAN_INFO,
		Fmt("\tTotals: %d open, %lu bytes", totalOpen, totalBytes));
}

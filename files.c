#include "base.h"
#include "panic.h"
#include "files.h"
#include "memory.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

struct File {
	bool inUse;

	char *path;
	int fd;
	size_t size;
	filehandle_t handle;
	uint8_t *data;
};

#define MAX_OPENFILES	64
static struct File *s_Files[MAX_OPENFILES] = {NULL};

/* The next handle that will be returned by open_file().
 * We start at 1 because we use 0 as invalid.
 */
static filehandle_t s_NextHandle = 1;

/* All file access are relative to here */
static const char *s_FilesRoot = NULL;

const char *get_root_path()
{
        return s_FilesRoot;
}

/*
 * NextFreeFile
 *	Returns a pointer to the next free File slot, or calls panic().
 */
static struct File *NextFreeFile()
{
	for (int i = 0; i < MAX_OPENFILES; i++) {
		if (s_Files[i] == NULL || s_Files[i]->inUse == false) {
			if (s_Files[i] == NULL)
				s_Files[i] = MemAlloc(sizeof(struct File));

			//trace(fmt("selected slot %d", i));
			s_Files[i]->inUse = true;
			return s_Files[i];
		}
	}

	panic(fmt("reached MAX_OPENFILES, which is %d", MAX_OPENFILES));
	return NULL;	/* not reached; shuts up warning */
}

/*
 * GetFileByHandle
 *	Returns the struct File * associated with the given filehandle_t.
 */
static struct File *GetFileByHandle(filehandle_t handle)
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
 * init_files
 */
ecode_t init_files(const char *rootDir)
{
        if (rootDir[strlen(rootDir)-1] != '/')
                panic("root directory must end with /");

	trace(CHAN_DBG, fmt("setting root directory to %s", rootDir));
	s_FilesRoot = rootDir;

	return EOK;
}

/*
 * shutdown_files
 *	Close all open files and free all buffers.
 */
ecode_t shutdown_files()
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

	trace(CHAN_DBG,
		fmt("freed %u file structures (%u were in use)", closed, used));

	return EOK;
}

/*
 * open_file
 */
static void OpenAndRead(struct File *file)
{
	file->fd = open(file->path, O_RDONLY);
	if (file->fd == -1)
		panic(fmt("failed to open %s", file->path));

	struct stat status;
	fstat(file->fd, &status);
	file->data = MemAlloc(status.st_size);
	file->size = status.st_size;

	/* We want read() to read the entire file at once, or we consider
	 * it to have failed.
	 */
	ssize_t ret = read(file->fd, file->data, file->size);
	if (ret == -1 || ret < file->size) {
		panic(fmt("read() returned %d", ret));
	}
}

filehandle_t open_file(const char *filename)
{
	assert(filename != NULL);

	char *fullPath = sstrcat(s_FilesRoot, filename);
	struct File *file = NextFreeFile();

	file->path = fullPath;
	file->handle = s_NextHandle++;
	OpenAndRead(file);

	trace(CHAN_DBG, fmt("opened %s (handle %u)", fullPath, file->handle));

	return file->handle;
}

/*
 * close_file
 */
void close_file(filehandle_t handle)
{
	struct File *file = GetFileByHandle(handle);
	if (!file)
		panic(fmt("no file for handle %u", handle));

	trace(CHAN_DBG, fmt("closed %s (handle %u)", file->path, handle));
	DestroyFile(file);
}

/*
 * file_get_data
 */
uint8_t *file_get_data(filehandle_t handle)
{
	struct File *file = GetFileByHandle(handle);
	if (!file)
		panic(fmt("no file for handle %u", handle));

	return file->data;
}

size_t file_get_size(filehandle_t handle)
{
	struct File *file = GetFileByHandle(handle);
	if (!file) {
		panic(fmt("no file for handle %u", handle));
	}

	return file->size;
}

/*
 * file_get_path
 */
const char *file_get_path(filehandle_t handle)
{
	struct File *file = GetFileByHandle(handle);
	if (!file) {
		panic(fmt("no file for handle %u", handle));
	}

	return file->path;
}

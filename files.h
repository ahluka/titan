#pragma once

/*
 * files.h
 *	All file accesses for the game should go through here.
 *	Error handling here is simple; we always panic() if a file cannot
 *	be found, hence little use of ecode_t.
 */

typedef uint32_t filehandle_t;

/* Initialise the file system. All file accesses are relative to rootDir,
 * which must end with a '/' */
ecode_t init_files(const char *rootDir);

/* Shutdown the file system, closing all open files and freeing all buffers */
ecode_t shutdown_files();

/* Open the specified file and buffer it up. Returns a handle to it. */
filehandle_t open_file(const char *filename);
void close_file(filehandle_t handle);

/* Get a pointer to the buffer associated with the given handle */
uint8_t *file_get_data(filehandle_t handle);

/* Get the size of the given file */
size_t file_get_size(filehandle_t handle);

/* Get a pointer to the file's full path */
const char *file_get_path(filehandle_t handle);

/* Returns the root directory name. It must end with a '/' so don't worry
 * about appending one when building a full path with this. */
const char *get_root_path();

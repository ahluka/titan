#pragma once

/*
 * All file accesses for the game should go through here.
 * Error handling here is simple; we always Panic() if a file cannot be found.
 * Hence little use of ecode_t.
 */

typedef uint32_t FileHandle;

/* Initialise the file system. All file accesses are relative to rootDir */
ecode_t Files_Init(const char *rootDir);

/* Shutdown the file system, closing all open files and freeing all buffers */
ecode_t Files_Shutdown();

/* Open the specified file and buffer it up. Returns a handle to it. */
FileHandle Files_OpenFile(const char *filename);
void Files_CloseFile(FileHandle handle);

/* Get a pointer to the buffer associated with the given handle */
uint8_t *Files_GetData(FileHandle handle);

#pragma once
#include "files.h"

ecode_t Script_Init();
ecode_t Script_Shutdown();

/* Execute an arbitrary string of code */
ecode_t Script_ExecString(const char *str);

/* Execute the script from the given FileHandle. Use this for executing files
 * of code; don't pass the file contents to Script_ExecString() because you
 * lose some of the debugging clarity since we can't tell where the code
 * came from.
 */
ecode_t Script_ExecFile(FileHandle handle);

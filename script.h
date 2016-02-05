#pragma once
#include "files.h"
#include <tcl8.6/tcl.h>


// typedef int (*ScriptCmdFunc)(ClientData *cdata, Tcl_Interp *interp,
// 	int argc, Tcl_Obj * const argv[]);
typedef Tcl_ObjCmdProc ScriptCmdFunc;

#define DECLARE_SCRIPT_PROC(name) \
	int name(ClientData cdata, Tcl_Interp *interp, \
		int argc, Tcl_Obj *const argv[])

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

/* Register the given script command function, so it can be called from
 * Tcl code. (DECLARE_SCRIPT_PROC helps.) The script will reference it
 * by the given name.
 */
ecode_t Script_RegisterCommand(ScriptCmdFunc fn, const char *name);

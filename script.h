#pragma once
#include "files.h"
#include <tcl8.6/tcl.h>


// typedef int (*ScriptCmdFunc)(ClientData *cdata, Tcl_Interp *interp,
// 	int argc, Tcl_Obj * const argv[]);
typedef Tcl_ObjCmdProc ScriptCmdFunc;

#define SCRIPT_PROCEDURE(name) \
	int name(ClientData cdata, Tcl_Interp *interp, \
		int argc, Tcl_Obj *const argv[])

ecode_t init_script();
ecode_t shutdown_script();

/* Execute an arbitrary string of code */
ecode_t script_execstr(const char *str);

/* Execute the script from the given filehandle_t. Use this for executing files
 * of code; don't pass the file contents to script_execstr() because you
 * lose some of the debugging clarity since we can't tell where the code
 * came from.
 */
ecode_t script_execfile(filehandle_t handle);

/* Register the given script command function, so it can be called from
 * Tcl code. (SCRIPT_PROCEDURE helps.) The script will reference it
 * by the given name.
 */
ecode_t script_register_command(ScriptCmdFunc fn, const char *name);

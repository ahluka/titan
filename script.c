#include "base.h"
#include "panic.h"
#include "script.h"
#include <tcl8.6/tcl.h>

static Tcl_Interp *s_Interp = NULL;
#define BASEDEFS_FILENAME "./res/tcl/basedefs.tcl"

/*
 * Script_Init
 *	Initialise Tcl and register some of our own core functionality.
 */
ecode_t Script_Init()
{
	if (s_Interp != NULL) {
		Panic("Script module already initialised");
	}

	s_Interp = Tcl_CreateInterp();
	if (!s_Interp) {
		Trace(CHAN_GENERAL, "Failed to create Tcl interpreter");
		return EFAIL;
	}

	/* Load and run bootstrap script */
	if (Tcl_EvalFile(s_Interp, BASEDEFS_FILENAME) != TCL_OK) {
		Trace(CHAN_GENERAL, Fmt("Couldn't eval %s", BASEDEFS_FILENAME));
		return EFAIL;
	}

	return EOK;
}

ecode_t Script_Shutdown()
{
	if (!s_Interp) {
		Panic("Script module not initialised");
	}

	Tcl_DeleteInterp(s_Interp);
	s_Interp = NULL;

	return EOK;
}

/*
 * Script_ExecString
 */
ecode_t Script_ExecString(const char *str)
{
	assert(str != NULL);

	if (!s_Interp) {
		Panic("Script module not initialised");
	}

	if (Tcl_Eval(s_Interp, str) != TCL_OK) {
		Trace(CHAN_GENERAL, Fmt("Failed to eval string '%s'", str));
		return EFAIL;
	}

	return EOK;
}

/*
 * Script_ExecFile
 */
ecode_t Script_ExecFile(FileHandle handle)
{
	const char *contents = (const char *) Files_GetData(handle);
	const char *path = Files_GetPath(handle);

	if (Tcl_Eval(s_Interp, contents) != TCL_OK) {
		Trace(CHAN_GENERAL, Fmt("Failed to eval file '%s'", path));
		return EFAIL;
	}

	return EOK;
}

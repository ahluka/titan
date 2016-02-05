#include "base.h"
#include "panic.h"
#include "script.h"

static Tcl_Interp *s_Interp = NULL;
#define BASEDEFS_FILENAME "./res/tcl/basedefs.tcl"

DECLARE_SCRIPT_PROC(ScriptTests) {
	Trace(CHAN_SCRIPT, "In ScriptTests()");

	return TCL_OK;
}

/*
 * Get the result string from the given interpreter and print it out.
 * Call after a Tcl API function doesn't return TCL_OK and the result string
 * will be an error message.
 */
static void PrintError(Tcl_Interp *i)
{
	Trace(CHAN_SCRIPT, Fmt("%s", Tcl_GetStringResult(i)));
}

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
		Trace(CHAN_SCRIPT, "Failed to create Tcl interpreter");
		return EFAIL;
	}

	Script_RegisterCommand(ScriptTests, "scrtests");

	/* Load and run bootstrap script */
	if (Tcl_EvalFile(s_Interp, BASEDEFS_FILENAME) != TCL_OK) {
		Trace(CHAN_SCRIPT, Fmt("Couldn't eval %s", BASEDEFS_FILENAME));
		PrintError(s_Interp);
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
		Trace(CHAN_INFO, Fmt("Failed to eval string '%s'", str));
		PrintError(s_Interp);
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
		Trace(CHAN_INFO, Fmt("Failed to eval file '%s'", path));
		PrintError(s_Interp);
		return EFAIL;
	}

	return EOK;
}

/*
 * Script_RegisterCommand
 */
ecode_t Script_RegisterCommand(ScriptCmdFunc fn, const char *name)
{
	assert(fn != NULL);
	assert(name != NULL);

	if (!s_Interp) {
		Panic("Script module not initialised");
	}

	if (Tcl_CreateObjCommand(s_Interp, name, fn, NULL, NULL) == NULL) {
		Script_Shutdown();
		Panic(Fmt("Failed to register '%s'", name));
	}

	return EOK;
}

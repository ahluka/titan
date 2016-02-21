#include "base.h"
#include "panic.h"
#include "script.h"

static Tcl_Interp *s_Interp = NULL;
#define BASEDEFS_FILENAME "./res/tcl/basedefs.tcl"

SCRIPT_PROCEDURE(ScriptTests) {
	trace(CHAN_SCRIPT, "In ScriptTests()");

	return TCL_OK;
}

/*
 * Get the result string from the given interpreter and print it out.
 * Call after a Tcl API function doesn't return TCL_OK and the result string
 * will be an error message.
 */
static void print_error(Tcl_Interp *i)
{
	trace(CHAN_SCRIPT, fmt("%s", Tcl_GetStringResult(i)));
}

/*
 * init_script
 *	Initialise Tcl and register some of our own core functionality.
 */
ecode_t init_script()
{
	if (s_Interp != NULL) {
		panic("Script module already initialised");
	}

	s_Interp = Tcl_CreateInterp();
	if (!s_Interp) {
		trace(CHAN_SCRIPT, "Failed to create Tcl interpreter");
		return EFAIL;
	}

	script_register_command(ScriptTests, "scrtests");

	/* Load and run bootstrap script */
	if (Tcl_EvalFile(s_Interp, BASEDEFS_FILENAME) != TCL_OK) {
		trace(CHAN_SCRIPT, fmt("Couldn't eval %s", BASEDEFS_FILENAME));
		print_error(s_Interp);
		return EFAIL;
	}

	return EOK;
}

ecode_t shutdown_script()
{
	if (!s_Interp) {
		panic("Script module not initialised");
	}

	Tcl_DeleteInterp(s_Interp);
	s_Interp = NULL;

	return EOK;
}

/*
 * script_execstr
 */
ecode_t script_execstr(const char *str)
{
	assert(str != NULL);

	if (!s_Interp) {
		panic("Script module not initialised");
	}

	if (Tcl_Eval(s_Interp, str) != TCL_OK) {
		trace(CHAN_INFO, fmt("Failed to eval string '%s'", str));
		print_error(s_Interp);
		return EFAIL;
	}

	return EOK;
}

/*
 * script_execfile
 */
ecode_t script_execfile(filehandle_t handle)
{
	const char *contents = (const char *) file_get_data(handle);
	const char *path = file_get_path(handle);

	if (Tcl_Eval(s_Interp, contents) != TCL_OK) {
		trace(CHAN_INFO, fmt("Failed to eval file '%s'", path));
		print_error(s_Interp);
		return EFAIL;
	}

	return EOK;
}

/*
 * script_register_command
 */
ecode_t script_register_command(ScriptCmdFunc fn, const char *name)
{
	assert(fn != NULL);
	assert(name != NULL);

	if (!s_Interp) {
		panic("Script module not initialised");
	}

	if (Tcl_CreateObjCommand(s_Interp, name, fn, NULL, NULL) == NULL) {
		shutdown_script();
		panic(fmt("Failed to register '%s'", name));
	}

	return EOK;
}

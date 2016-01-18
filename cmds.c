#include "base.h"
#include "memory.h"
#include "panic.h"
#include "cmds.h"

struct Command {
	const char *name;
	CmdFunction function;
};

#define MAX_COMMANDS 64
static struct Command *s_Commands[MAX_COMMANDS] = {NULL};

/*
 * NewCommand
 */
static void NewCommand(const char *name, CmdFunction fn)
{
	for (int i = 0; i < MAX_COMMANDS; i++) {
		if (!s_Commands[i]) {
			s_Commands[i] = MemAlloc(sizeof(struct Command));
			s_Commands[i]->name = name;
			s_Commands[i]->function = fn;
			return;
		}
	}

	Panic(Fmt("reached MAX_COMMANDS, which is %d", MAX_COMMANDS));
}

/*
 * CmdList
 */
static void CmdList()
{
	uint32_t total = 0;
	printf("List of commands:\n");
	for (int i = 0; i < MAX_COMMANDS; i++) {
		if (s_Commands[i]) {
			printf("\t%s\n", s_Commands[i]->name);
			total++;
		}
	}
	printf("\tTotal: %u commands (max is %d)\n", total, MAX_COMMANDS);
}

/*
 * Cmd_Init
 */
ecode_t Cmd_Init()
{
	Cmd_Register("cmdlist", CmdList);

	return EOK;
}

/*
 * Cmd_Shutdown
 */
ecode_t Cmd_Shutdown()
{
	for (int i = 0; i < MAX_COMMANDS; i++) {
		if (s_Commands[i] != NULL) {
			MemFree(s_Commands[i]);
			s_Commands[i] = NULL;
		}
	}

	return EOK;
}

/*
 * Cmd_Register
 */
ecode_t Cmd_Register(const char *name, CmdFunction func)
{
	NewCommand(name, func);

#ifdef DEBUG_TRACING_ON
	Trace(Fmt("registered command '%s'", name));
#endif

	return EOK;
}

/*
 * Cmd_Execute
 */
ecode_t Cmd_Execute(const char *name)
{
	for (int i = 0; i < MAX_COMMANDS; i++) {
		if (strcmp(name, s_Commands[i]->name) == 0) {
#ifdef DEBUG_TRACING_ON
			Trace(Fmt("executing '%s'", name));
#endif
			s_Commands[i]->function();
			return EOK;
		}
	}
#ifdef DEBUG_TRACING_ON
	Trace(Fmt("command '%s' doesn't exist", name));
#endif
	return EFAIL;
}

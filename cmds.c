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
	assert(name != NULL);
	assert(fn != NULL);

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
static void CmdList(int argc, char **argv)
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

static void TestFunc(int argc, char **argv)
{
	printf("Got %d args:\n", argc);
	for (int i = 0; i < argc; i++) {
		printf("\t%s\n", argv[i]);
	}
}

/*
 * Cmd_Init
 */
ecode_t Cmd_Init()
{
	Cmd_Register("cmdlist", CmdList);
	Cmd_Register("test", TestFunc);

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

 	Trace(CHAN_DBG, Fmt("registered command '%s'", name));

	return EOK;
}

/*
 * Cmd_Execute
 */
ecode_t Cmd_Execute(const char *cmd, int argc, char **argv)
{
	assert(cmd != NULL);

	for (int i = 0; i < MAX_COMMANDS; i++) {
		if (!s_Commands[i])
			continue;

		if (strcmp(cmd, s_Commands[i]->name) == 0) {
			Trace(CHAN_DBG, Fmt("executing '%s'", cmd));
			s_Commands[i]->function(argc, argv);
			return EOK;
		}
	}

	Trace(CHAN_DBG, Fmt("command '%s' doesn't exist", cmd));

	return EFAIL;
}


static int CountArgs(const char *buffer)
{
	int count = 0;
	const char *p = buffer;

	while (*p) {
		if (*p == ' ') {
			count++;
		}

		p++;
	}

	return count;
}

/*
 * Cmd_ExecuteBuf
 *	Given a buffer containing an arbitrary command string, attempt to
 *	parse and execute it. Commands are of the form:
 *		cmd arg1 arg2 ... argN
 *
 * NOTE: This uses strdup() instead of sstrdup() to avoid filling the global
 * string pool with user-typed commands.
 */
ecode_t Cmd_ExecuteBuf(char *buffer)
{
	const char *delims = " \n\t\r";
	char *copy = strdup(buffer);
	char *token = strtok(copy, delims);

	if (!token) {
		Panic("malformed command string");
	}

	char *cmd = strdup(token);
	char **argv = NULL;
	int argc = CountArgs(buffer);

	if (!argc) {
		Trace(CHAN_DBG, Fmt("found command '%s'", cmd));
		Cmd_Execute(cmd, argc, NULL);
		goto return_eok;
	}

	argv = MemAlloc(argc * sizeof(char *));

	int argsCopied = 0;
	while (token != NULL) {
		token = strtok(NULL, delims);
		if (!token) break;
		argv[argsCopied++] = strdup(token);
	}

	Trace(CHAN_DBG, Fmt("found command '%s' with %d args", cmd, argc));

	Cmd_Execute(cmd, argc, argv);

	for (int i = 0; i < argc; i++) {
		MemFree(argv[i]);
	}

return_eok:
	free(copy);
	free(cmd);
	return EOK;
}

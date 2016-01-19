#pragma once

typedef void (*CmdFunction)(int argc, char **argv);

ecode_t Cmd_Init();
ecode_t Cmd_Shutdown();

ecode_t Cmd_Register(const char *name, CmdFunction func);
ecode_t Cmd_Execute(const char *cmd, int argc, char **argv);
ecode_t Cmd_ExecuteBuf(char *buffer);

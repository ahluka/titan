#pragma once

typedef void (*CmdFunction)();

ecode_t Cmd_Init();
ecode_t Cmd_Shutdown();

ecode_t Cmd_Register(const char *name, CmdFunction func);
ecode_t Cmd_Execute(const char *name);

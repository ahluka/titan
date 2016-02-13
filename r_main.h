#pragma once

/*
 * r_main.h
 *	Main renderer module. Pulls in everything else.
 */


ecode_t Rend_Init();
ecode_t Rend_Shutdown();
ecode_t Rend_Frame();

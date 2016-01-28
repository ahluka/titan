#pragma once

/*
 * r_main.h
 *	TODO: Write description comment
 */

#include <SDL2/SDL.h>

ecode_t Rend_Init();
ecode_t Rend_Shutdown();
ecode_t Rend_Frame();
SDL_Window *Rend_GetWindow();

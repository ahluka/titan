#ifndef __RENDERER_H__
#define __RENDERER_H__

/*
 * renderer.h
 *	TODO: Write description comment
 */

#include <SDL2/SDL.h>

ecode_t Rend_Init();
ecode_t Rend_Shutdown();
ecode_t Rend_Frame();
SDL_Window *Rend_GetWindow();

#endif /* __RENDERER_H__ */

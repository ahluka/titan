/*
 * r_viewport.h
 *      Manages the current viewport.
 */
#pragma once
#include "vec.h"
#include <SDL2/SDL.h>

struct viewport {
        SDL_Rect r;
        vec2_t vel;
};

struct viewport create_viewport(int x, int y, int w, int h);
bool viewport_contains(struct viewport *vp, vec2_t v);
bool viewport_contains_xy(struct viewport *vp, int x, int y);

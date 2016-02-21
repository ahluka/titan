#pragma once

/*
 * r_main.h
 *	Main renderer module.
 */
#include "r_cache.h"

typedef enum Colour {
        COLOUR_BLACK,
        COLOUR_WHITE,
        COLOUR_RED,
        COLOUR_GREEN,
        COLOUR_BLUE,
        COLOUR_GREY,
        COLOUR_MAGENTA
} Colour;

typedef enum FontSize {
        FONT_SMALL,
        FONT_NORMAL
} FontSize;

/* Rendering commands. Can only be called during an entity's Render()
 * function as that's when we want all commands to be added. */
void r_add_string(FontSize sz, Colour c, int x, int y, const char *str);
void r_add_circle(Colour c, int x, int y, float r);
void r_add_line(Colour c, int sx, int sy, int ex, int ey);
void r_add_rect(Colour c, int x, int y, int w, int h);
void r_add_point(Colour c, int x, int y);

/* These are called by the base modules. */
ecode_t init_renderer();
ecode_t shutdown_renderer();
ecode_t r_render_frame();
void r_begin_commands();
void r_end_commands();

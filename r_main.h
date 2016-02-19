#pragma once

/*
 * r_main.h
 *	Main renderer module. Pulls in everything else.
 */

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
void R_AddString(FontSize sz, Colour c, int x, int y, const char *str);
void R_AddCircle(Colour c, int x, int y, float r);
void R_AddLine(Colour c, int sx, int sy, int ex, int ey);
void R_AddRect(Colour c, int x, int y, int w, int h);
void R_AddPoint(Colour c, int x, int y);

/* These are called by the base modules. */
ecode_t R_Init();
ecode_t R_Shutdown();
ecode_t R_RenderFrame();
void R_BeginCommands();
void R_EndCommands();

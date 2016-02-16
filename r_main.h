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
        COLOUR_GREY
} Colour;

typedef enum FontSize {
        FONT_SMALL,
        FONT_NORMAL
} FontSize;


void R_AddString(FontSize sz, Colour c, int x, int y, const char *str);


/* These are called by the base modules. */
ecode_t Rend_Init();
ecode_t Rend_Shutdown();
ecode_t Rend_Frame();

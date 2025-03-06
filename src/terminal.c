/*
 *  terminal.h
 *
 *  Extends "terminal.h".
 * 
 *  By Aleksander Slepowronski.
 */

#include "terminal.h"

/* Moves the cursor in given direction.
 *
 *  dir         - the direction
 *  x           - no of characters (distance) 
 */
void cur_move(dir_t dir, size_t x)
{
    fprintf(stdout, "\e[%zu%c", x, 'A' + dir);
}

/* Moves cursor to given location.
 *
 *  x, y        - the location
 */
void cur_to(size_t x, size_t y)
{
    cur_home();
    cur_move(DOWN, y);
    cur_move(RIGHT, x);
}

/* Saves current cursor position. 
 */
void cur_save(void)
{
    fprintf(stdout, "\e[s");
}

/* Reverts saved cursor position.
 */
void cur_load(void)
{
    fprintf(stdout, "\e[u");
}

/* Moves cursor to home (0, 0) position
 */
void cur_home(void)
{
    fprintf(stdout, "\e[H");
}

/* Clears current line.
 */
void clr_line(void)
{
    fprintf(stdout, "\e[2K");
    fprintf(stdout, "\r");
}

/* Clears the screen. 
 */
void cls(void)
{
    system(CMD_CLEAR);
    cur_home();
}

/* Sets text color. 
 *
 *  color       - the color
 */
void col_set(color_t color)
{
    fprintf(stdout, "\e[%dm", (int) color);
}

/* Writes text in given color.
 * Returns back to the default one.

 *  text        - the text
 *  color       - the color   
 */
void col_write(const char *text, color_t color)
{
    assert(text);

    fprintf(stdout, "\e[%dm", (int) color);
    fprintf(stdout, "%s", text);
    fprintf(stdout, "\e[%dm", (int) COLOR_DEFAULT);
}
/*
 *  terminal.h
 *
 *  Based on terminal escape codes.
 *  Gives opportunity to move the cursor.
 *  Contains support for basic colors. 
 *  Also manages displaying text (UI).
 * 
 *  By Aleksander Slepowronski.
 */

 #ifndef _GRAPH_TERMINAL_H_FILE_
 #define _GRAPH_TERMINAL_H_FILE_
 
 #ifdef __linux__
     #define CMD_CLEAR       "clear"
 #elif _WIN32
     #define CMD_CLEAR       "cls"
 #else
     #error "Unknown platform."
 #endif
 
 #include <assert.h>
 #include <stdint.h>
 #include <stdio.h>
 #include <stdlib.h>
 
 /* Basic directions. */
 typedef enum _gph_dir_t
 {
     UP,
     DOWN,
     RIGHT,
     LEFT,
     DIR_NONE
 
 } dir_t;
 
 /* Terminal colors */
 typedef enum _gph_color_t
 {
     COLOR_DEFAULT = 39,
 
     BLACK = 30,
     RED = 31,
     GREEN,
     YELLOW,
     BLUE,
     MAGENTA,
     CYAN,
     WHITE
 
 } color_t;
  

/* Moves the cursor in given direction.
 *
 *  dir         - the direction
 *  x           - no of characters (distance) 
 */
void cur_move(dir_t dir, size_t x);

/* Moves cursor to given location.
 *
 *  x, y        - the location
 */
void cur_to(size_t x, size_t y);

/* Saves current cursor position. 
 */
void cur_save(void);

/* Reverts saved cursor position.
 */
void cur_load(void);

/* Moves cursor to home (0, 0) position
 */
void cur_home(void);

/* Clears current line.
 */
void clr_line(void);

/* Clears the screen. 
 */
void cls(void);

/* Sets text color. 
 *
 *  color       - the color
 */
void col_set(color_t color);

/* Writes text in given color.
 * Returns back to the default one.

 *  text        - the text
 *  color       - the color   
 */
void col_write(const char *text, color_t color);

#endif /* _GRAPH_TERMINAL_H_FILE_ */
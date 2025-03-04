/*
 *  command.h
 *
 *  System of program commands. 
 *  They can be dispatched, calling
 *  appropiate functions. Arguments
 *  are splitted into words (tokens).
 * 
 *  By Aleksander Slepowronski.
 */

#ifndef _GRAPH_COMMAND_H_FILE_
#define _GRAPH_COMMAND_H_FILE_

#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD_SIZE                32u      /* Fixed # of commands in buffer */

#define CMD_RET_ERROR           -1       /* RUN: Returned, critical error */
#define CMD_RET_UNKNOWN         0        /* RUN: Returned, entered unknown command */
#define CMD_RET_SKIPPED         1        /* RUN: Returned, no command executed (empty input) */  
#define CMD_RET_SUCCESS         2        /* RUN: Returned, command executed */

#define CMD_SET_INSENS          (1 << 1) /* RUN: Case insensitive */


/* Command callable function alias */
typedef void * (*cmd_func_t)(char **, int);

/* A command */
typedef struct _gph_cmd
{
    char           *_name;      /* Command name */
    cmd_func_t      _fnexe;     /* Execution function */

} command_t;


/* Registers new command 
 *
 *  name        - command calling name
 *  func        - function to be executed on cmd call 
 */
void            cmd_add(const char *name, cmd_func_t func);

/* Analyses input and calls valid commands.
 *
 *  input       - text input from user 
 *  settings    - additional info (CMD_SET_*)
 * 
 * Returns appropiate CMD_RET_* value.
 */
int             cmd_run(const char *input, int settings);


#endif /* _GRAPH_COMMAND_H_FILE_ */
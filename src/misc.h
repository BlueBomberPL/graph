/*
 *  misc.h
 *
 *  This file contains multi purpose
 *  objects that could not be classified
 *  in logical manner.
 * 
 *  By Aleksander Slepowronski.
 */

#ifndef _GRAPH_MISC_H_FILE_
#define _GRAPH_MISC_H_FILE_

#include <assert.h>
#include <stdio.h>

#include "global.h"
#include "terminal.h"

/* Prints error to stderr.
 *
 *  msg         - the message
 */
static inline void msc_err(const char *msg)
{
    assert(msg);
    col_set(RED);
    fprintf(stderr, "E: ");
    col_set(COLOR_DEFAULT);
    fprintf(stderr, "%s\n", msg);
}

/* Prints warning to stderr.
 *
 *  msg         - the message
 */
static inline void msc_war(const char *msg)
{
    assert(msg);
    col_set(MAGENTA);
    fprintf(stderr, "W: ");
    col_set(COLOR_DEFAULT);
    fprintf(stderr, "%s\n", msg);
}

/* Prints info to stdout.
 *
 *  msg         - the message
 */
static inline void msc_inf(const char *msg)
{
    assert(msg);
    col_set(GREEN);
    fprintf(stdout, "I: ");
    col_set(COLOR_DEFAULT);
    fprintf(stdout, "%s\n", msg);
}

/* Gets user input in a beautiful way 
 *
 *  Returns the allocated input in text format.
 */
static inline char *msc_inp(void)
{
    char *buffer = NULL;
    if((buffer = (char *) malloc(GLO_MAX_USER_INPUT)) == NULL)
        return NULL;

    fprintf(stdout, "> ");
    col_set(BLUE);

    fgets(buffer, GLO_MAX_USER_INPUT - 1u, stdin);
    col_set(COLOR_DEFAULT);

    return buffer;
}

#endif /* _GRAPH_MISC_H_FILE_ */
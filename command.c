/*
 *  command.c
 *
 *  Extends "command.h".
 * 
 *  By Aleksander Slepowronski.
 */

#include "command.h"

/* Global command buffer and its size */
static command_t g_commands[CMD_SIZE] = {0, };
static size_t    g_n = 0u;

/* Registers new command 
 *
 *  name        - command calling name
 *  func        - function to be executed on cmd call 
 */
void cmd_add(const char *name, cmd_func_t func)
{
    assert(name && func && g_n < CMD_SIZE);

    /* Alloc */
    if(g_commands[g_n]._name == NULL)
    {
        if((g_commands[g_n]._name = (char *) malloc(strlen(name) + 1u)) == NULL)
            return;
    }

    strcpy(g_commands[g_n]._name, name);
    g_commands[g_n]._fnexe = func;

    ++g_n;
}

/* Analyses input and calls valid commands.
 *
 *  input       - text input from user 
 *  settings    - additional info (CMD_SET_*)
 * 
 * Returns appropiate CMD_RET_* value.
 */
int cmd_run(const char *input, int settings)
{
    assert(input);

    /* Input copy */
    char *in = NULL;
    if((in = (char *) malloc(strlen(input) + 1u)) == NULL)
        return CMD_RET_ERROR;


    /* Skipping multi spaces */
    size_t k = 0u;
    for(size_t i = 0u; i < strlen(input); ++i)
    {
        if(isspace(input[i]) && isspace(input[i + 1u]))
            continue;

        else if(isspace(input[i]))
            in[k++] = ' ';

        else
            in[k++] = input[i];
    }

    in[k] = '\0';
    
    /* Trimming leading spaces */
    while(1)
    {
        if(isspace(in[0u]))
        {
            in++;
            continue;
        }
        break;
    }

    /* Trimming trailing spaces */
    while(1)
    {
        const size_t last = strlen(in) - 1u;

        if(isspace(in[last]))
        {
            in[last] = '\0';
            continue;
        }
        break;
    }

    /* Anything left ? */
    if(strlen(in) == 0u)
        return CMD_RET_SKIPPED;

    /* Dividing into words */
    char **argv = NULL;

    /* Counting */
    size_t words = 1u;
    for(size_t i = 0u; i < strlen(in); ++i)
    {
        if(in[i] == ' ')
            ++words;
    }

    if((argv = (char **) malloc(words * sizeof(char *))) == NULL)
    {
        free(in);
        return CMD_RET_ERROR;
    }

    const size_t ori_len = strlen(in) + 1u;
    argv[0u] = in;
    k = 1u;
    for(size_t i = 0u; i <= ori_len && k < words; ++i)
    {
        if(in[i] == ' ')
        {
            in[i] = '\0';
            argv[k++] = in + i + 1u;
        }
    }

    /* Calling appropiate function */
    for(size_t i = 0u; i < g_n; ++i)
    {
        if(strcmp(g_commands[i]._name, argv[0u]) == 0)
        {
            g_commands[i]._fnexe(argv + 1u, words - 1u);
            free(argv);
            free(in);
            return CMD_RET_SUCCESS;
        }
    }

    return CMD_RET_UNKNOWN;
}
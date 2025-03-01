#include <stdio.h>

#include "command.h"
#include "global.h"
#include "graph.h"
#include "misc.h"
#include "terminal.h"

/* GLOBAL GRAPH */
static graph_t *g_graph = NULL;

/* CMD: For "add" command */
/* Adds new vertex */
void *_command_add(char **argv, int argc)
{
    assert(argv);

    /* If argc = 0, the vertex is isolated */
    if(argc == 0)
        msc_war("This vertex will be isolated (0 arches).");

    /* Validation (if natural numbers) */
    for(int i = 0; i < argc; ++i)
    {
        for(size_t j = 0u; j < strlen(argv[i]); ++j)
        {
            if(! isdigit(argv[i][j]))
            {
                printf("%c ", argv[i][j]);
                msc_err("Expected positive integer.");
                goto END;
            }
        }
    }

    /* Adding vertex */
    const size_t result = gph_add(g_graph, NULL);

    /* Added arch # */
    size_t added = 0u;

    /* Alias */
    vertex_t *v = g_graph->_list[(g_graph->_n > 0u) ? (g_graph->_n - 1u) : 0u];

    if(result != 1u || v == NULL)
    {
        msc_err("Critical memory error. Closing...");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < argc; ++i)
    {
        /* Adding arches (only if graph size > 1) */
        const size_t result = gph_con(g_graph, g_graph->_n - 1u, (uint16_t) atol(argv[i]), GPH_ADD);
        if(result == (size_t) -1 && g_graph->_n > 1u)
        {
            msc_err("Critical memory error. Closing...");
            exit(EXIT_FAILURE);
        }
        else if(result < 1u)
        {
            msc_err("Arch pointing to non-existent vertex. ");
            gph_del(g_graph, g_graph->_n - 1u);
            goto END;
        }
        else if(g_graph->_n > 1u)
        {
            ++added;
        }
    }

    /* Printing info (success) */
    {
        char buf[GLO_MAX_MSG_OUTPUT] = {0, };
        snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Created new vertex (ID = %zu, arches = %zu).", g_graph->_n - 1u, added);
        msc_inf(buf);
    }

    END:;
    return NULL;
}

/* CMD: For "arch" command */
/* Manages an arch A -> B */
void *_command_arch(char **argv, int argc)
{
    /* Validation */
    if(argc < 3u)
    {
        msc_err("Missing parameters.");
        return NULL;
    }

    int operation = 0;
    uint16_t a, b;

    /* 1st parameter */
    if(strcmp(argv[0u], "add") == 0)
        operation = GPH_ADD;
    else if(strcmp(argv[0u], "del") == 0)
        operation = GPH_DELETE;
    else
    {
        msc_err("Unknown operation parameter.");
        return NULL;
    }

    /* 2nd and 3rd params */
    if(sscanf(argv[1], "%hu", &a) + sscanf(argv[2], "%hu", &b) < 2)
    {
        msc_err("Expected positive integer.");
        return NULL;
    }

    /* Operation */
    const size_t result = gph_con(g_graph, a, b, operation);
    if(result == (size_t) -1)
    {
        msc_err("Critical memory error. Closing...");
        exit(EXIT_FAILURE);
    }
    else
    {
        char buf[GLO_MAX_MSG_OUTPUT];
        snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Updated %u arch(es).", result);
        msc_inf(buf);
    }
    return NULL;
}

/* CMD: For "cls" command */
/* Cleans the screen */
void *_command_cls(char **argv, int argc)
{
    cls();
    return NULL;
}

/* CMD: For "exit" command */
/* Closes the program */
void *_command_exit(char **argv, int argc)
{
    gph_fre(g_graph);
    col_set(COLOR_DEFAULT);
    printf("\n");
    exit(EXIT_SUCCESS);
}

/* CMD: For "help" command */
/* Views help */
void *_command_help(char **argv, int argc)
{
    fprintf(stdout, "\nBasic Graph Generator - Help                                                 \n\n");
    fprintf(stdout, "\tadd      [A B C ...]         - adds new vertex pointing to a, b, c ... vertices\n");
    fprintf(stdout, "\tarch     <add/del> <A> <B>   - adds/deletes an arch from A to B                \n");
    fprintf(stdout, "\tcls                          - clears the screen                               \n");
    fprintf(stdout, "\tdel      <A>                 - deletes A vertex, updating whole graph          \n");
    fprintf(stdout, "\texit                         - closes the program                              \n");
    fprintf(stdout, "\tfile     <name>              - saves the graph to the given file               \n");
    fprintf(stdout, "\tfind     <A> <B>             - looks for an A to B arch                        \n");
    fprintf(stdout, "\thelp                         - who knows...                                    \n");
    fprintf(stdout, "\tlist     [-t]                - prints the graph (-t - with \'tell\')           \n");
    fprintf(stdout, "\tnew      [-f]                - clears the graph (-f - with force )             \n");
    fprintf(stdout, "\tset      <A> [B C D ...]     - updates A vertex                                \n");
    fprintf(stdout, "\tsize     <n>                 - resizes the graph                               \n");
    fprintf(stdout, "\ttell                         - prints info about the graph                     \n");
    fprintf(stdout, "\n");
    return NULL;
}

/* CMD: For "del" command */
/* Deletes a vertex */
void *_command_del(char **argv, int argc)
{
    /* Validation */
    if(argc == 0u)
    {
        msc_err("Missing parameters.");
        return NULL;
    }

    /* 1st parameter */
    uint16_t a;
    if(sscanf(argv[0u], "%hu", &a) < 1u)
    {
        msc_err("Expected positive integer.");
        return NULL;
    }

    /* Operation */
    const size_t result = gph_del(g_graph, a);
    if(result == (size_t) -1)
    {
        msc_err("Critical memory error. Closing...");
        exit(EXIT_FAILURE);
    }
    else
    {
        char buf[GLO_MAX_MSG_OUTPUT];
        snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Updated %u vertex(vertices).", result);
        msc_inf(buf);
    }

    return NULL;
}

int main(int argc, char **argv)
{
    /* Initialisation */

    /* Atexit */
    atexit((void (*)(void))_command_exit);

    /* Graph */
    g_graph = gph_new(GLO_DEF_GRAPH_SIZE);
    if(g_graph == NULL)
    {
        msc_err("Critical memory error. Closing...");
        exit(EXIT_FAILURE);
    }

    /* Commands */
    cmd_add("add", _command_add);
    cmd_add("arch", _command_arch);
    cmd_add("cls", _command_cls);
    cmd_add("del", _command_del);
    cmd_add("exit", _command_exit);

    /* Input loop */
    while(1)
    {
        char *input = NULL;
        if((input = msc_inp()) == NULL)
        {
            msc_err("Critical memory error. Closing...");
            exit(EXIT_FAILURE);
        }

        switch (cmd_run(input, 0))
        {
        case CMD_RET_ERROR:
        {
            msc_err("Critical input error. Closing...");
            exit(EXIT_FAILURE);
        }
        case CMD_RET_UNKNOWN:
        {
            msc_err("Unknown command. Check \'help\'.");
            break;
        }
        case CMD_RET_SUCCESS:
        case CMD_RET_SKIPPED:
        {
            break;
        }
        default:
            break;
        }

        free(input);
    }
    
    return EXIT_SUCCESS;
}
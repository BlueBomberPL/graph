#include <stdio.h>

#include "command.h"
#include "global.h"
#include "graph.h"
#include "misc.h"
#include "terminal.h"
#include "ai.h"

/* GLOBAL GRAPH */
static graph_t *g_graph = NULL;


/* CMD: For "tell" command */
/* Prints details */
void *_command_tell(char **argv, int argc);


/* CMD: For "add" command */
/* Adds new vertex */
void *_command_add(char **argv, int argc)
{
    assert(argv);

    /* If argc = 0, the vertex is isolated */
    /* Showing warning only if the graph is not empty */
    if(argc == 0 && g_graph->_n > 0u)
        msc_war("This vertex will be isolated (0 arches).");

    /* Adding fresh vertex */
    if(gph_add(g_graph, NULL) != 1u)
    {
        msc_err("Critical memory error. Closing...");
        exit(EXIT_FAILURE);
    }

    /* # of added arches */
    size_t added = 0u;

    /* Reading args (arches) */
    for(int i = 0; i < argc; ++i)
    {
        index_t arch = 0u;
        if(sscanf(argv[i], "%hu", &arch) < 1)
        {
            msc_err("Expected positive integer.");
            return NULL;
        }

        /* Trying to create a connection */
        size_t temp = gph_con(g_graph, GPH_LAST, arch, GPH_ADD);
        if(temp == (size_t) -1)
        {
            msc_err("Critical memory error. Closing...");
            exit(EXIT_FAILURE);
        }
        else if(temp == 0u)
        {
            /* Printing info (failure) */
            char buf[GLO_MAX_MSG_OUTPUT] = {0, };
            snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Invalid vertex index (%hu).", arch);
            msc_err(buf);

            /* Deleting */
            gph_del(g_graph, GPH_LAST);
            return NULL;
        }

        added += temp;
    }

    /* Printing info (success) */
    {
        char buf[GLO_MAX_MSG_OUTPUT] = {0, };
        snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Created new vertex (ID = %zu, arches = %zu).", g_graph->_n - 1u, added);
        msc_inf(buf);
    }

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
    index_t a = 0u, b = 0u;

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

    /* 2nd param */
    if(strcmp(argv[1u], "last") == 0)
        a = GPH_LAST;

    else if(sscanf(argv[1u], "%hu", &a) < 1)
    {
        msc_err("Expected positive integer or \'last\'.");
        return NULL;
    }

    /* 3rd param */
    if(strcmp(argv[2u], "last") == 0)
        b = GPH_LAST;

    else if(sscanf(argv[2u], "%hu", &b) < 1)
    {
        msc_err("Expected positive integer or \'last\'.");
        return NULL;
    }

    /* Operation */
    const size_t result = gph_con(g_graph, a, b, operation);
    if(result == (size_t) -1)
    {
        msc_err("Critical memory error. Closing...");
        exit(EXIT_FAILURE);
    }

    /* Bad A index */
    else if(result == 0u && (a >= g_graph->_n && a != GPH_LAST))
    {
        /* Printing info (failure) */
        char buf[GLO_MAX_MSG_OUTPUT] = {0, };
        snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Invalid vertex index (%hu).", a);
        msc_err(buf);
    }

    /* Bad B index */
    else if(result == 0u && (b >= g_graph->_n && b != GPH_LAST))
    {
        /* Printing info (failure) */
        char buf[GLO_MAX_MSG_OUTPUT] = {0, };
        snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Invalid vertex index (%hu).", b);
        msc_err(buf);
    }

    else
    {
        /* Printing info (success) */
        char buf[GLO_MAX_MSG_OUTPUT];
        snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Updated %zu arch(es).", result);
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

/* CMD: For "del" command */
/* Deletes a vertex */
void *_command_del(char **argv, int argc)
{
    /* Validation */
    if(argc < 1)
    {
        msc_err("Missing parameter.");
        return NULL;
    }

    /* Number of deleted */
    size_t deleted = 0u;
    /* Indexes of these to be deleted */
    index_t *tab = NULL;

    if((tab = (index_t *) malloc(argc * sizeof(index_t))) == NULL)
    {
        msc_err("Critical memory error. Closing...");
        exit(EXIT_FAILURE);
    }

    /* Getting indexes from args */
    for(int i = 0; i < argc; ++i)
    {
        index_t index = 0u;

        if(strcmp(argv[i], "last") == 0)
            tab[i] = GPH_LAST;

        else if(sscanf(argv[i], "%hu", &index) < 1)
        {
            msc_err("Expected positive integer or \'last\'.");
            return NULL;
        }
        else if(index != GPH_LAST && index >= g_graph->_n)
        {
            /* Printing info (failure) */
            char buf[GLO_MAX_MSG_OUTPUT] = {0, };
            snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Invalid vertex index (%hu).", index);
            msc_err(buf);
        }
        else
            tab[i] = index;
        
    }

    /* Deleting basing on these pointers */
    /* Sorting descending so the indexes will always be valid */
    qsort(tab, argc, sizeof(index_t), _gph_sort_des);

    /* For each index */
    for(int i = 0; i < argc; ++i)
    {
        const size_t result = gph_del(g_graph, tab[i]);

            if(result == (size_t) -1)
            {
                msc_err("Critical memory error. Closing...");
                exit(EXIT_FAILURE);
            }
        
        deleted += result;
    }
    
    /* Printing info */
    {
        char buf[GLO_MAX_MSG_OUTPUT];
        snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Updated %u vertex(vertices).", deleted);
        msc_inf(buf);
    }

    free(tab);
    return NULL;
}

/* CMD: For "exit" command */
/* Closes the program */
void *_command_exit(char **argv, int argc)
{
    col_set(COLOR_DEFAULT);
    printf("\n");
    exit(EXIT_SUCCESS);
}

/* CMD: For "find" command */
/* Finds a connection (arch) */
void *_command_find(char **argv, int argc)
{
    /* Validation */
    if(argc < 2)
    {
        msc_err("Missing parameters.");
        return NULL;
    }


    index_t a = 0u, b = 0u;
    int result = 0;

    /* 1st param */
    if(strcmp(argv[0u], "last") == 0)
        a = GPH_LAST;

    else if(sscanf(argv[0u], "%hu", &a) < 1)
    {
        msc_err("Expected positive integer or \'last\'.");
        return NULL;
    }

    /* 2nd param */
    if(strcmp(argv[1u], "last") == 0)
        b = GPH_LAST;

    else if(sscanf(argv[1u], "%hu", &b) < 1)
    {
        msc_err("Expected positive integer or \'last\'.");
        return NULL;
    }

    /* Operation */
    result = gph_typ(g_graph, a, b);

    /* Bad A index */
    if(result == (size_t) -1 && (a >= g_graph->_n && a != GPH_LAST))
    {
        /* Printing info (failure) */
        char buf[GLO_MAX_MSG_OUTPUT] = {0, };
        snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Invalid vertex index (%hu).", a);
        msc_err(buf);
    }

    /* Bad B index */
    else if(result == (size_t) -1 && (b >= g_graph->_n && b != GPH_LAST))
    {
        /* Printing info (failure) */
        char buf[GLO_MAX_MSG_OUTPUT] = {0, };
        snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Invalid vertex index (%hu).", b);
        msc_err(buf);
    }

    /* Valid indexes */
    else
    {
        switch (result)
        {
        case GPH_ONEWAY:
            msc_inf("One-way connection found (A --> B).");
            return NULL;

        case GPH_TWOWAY:
            msc_inf("Two-way connection found (A <-> B).");
            return NULL;

        default:
            msc_inf("No connection found.");
            return NULL;
        }
    }

    return NULL;
}

/* CMD: For "help" command */
/* Views help */
void *_command_help(char **argv, int argc)
{
    fprintf(stdout, "\nBasic Graph Generator - Help                                                 \n\n");
    fprintf(stdout, "\tadd      [A B C ...]         - adds new vertex pointing to A, B, C ... vertices\n");
    fprintf(stdout, "\tarch     <add/del> <A> <B>   - adds/deletes an arch from A to B                \n");
    fprintf(stdout, "\tcls                          - clears the screen                               \n");
    fprintf(stdout, "\tdel      <A>                 - deletes A vertex, updating whole graph          \n");
    fprintf(stdout, "\texit                         - closes the program                              \n");
    //fprintf(stdout, "\tfile     <name>              - saves the graph to the given file               \n");
    fprintf(stdout, "\tfind     <A> <B>             - looks for an A to B arch                        \n");
    fprintf(stdout, "\thelp                         - who knows...                                    \n");
    fprintf(stdout, "\tlist     [-t]                - prints the graph (-t - with \'tell\')           \n");
    fprintf(stdout, "\tnew      [-f]                - clears the graph (-f - with force )             \n");
    fprintf(stdout, "\tset      <A>: [B C D ...]    - updates A vertex                                \n");
    fprintf(stdout, "\tsize     <n> [-f]            - resizes the graph (-f - with force )            \n");
    fprintf(stdout, "\ttell                         - prints info about the graph                     \n");
    fprintf(stdout, "\tai                           - opens AI prompt that can generate commands from user input\n");
    fprintf(stdout, "\taimodel                      - changes used ollama model\n");
    fprintf(stdout, "\n");
    return NULL;
}

/* CMD: For "list" command */
/* Prints the graph */
void *_command_list(char **argv, int argc)
{
#define FLAG_TELL       (1 << 0)

    /* Check flags */
    int settings = 0;
    for(int i = 0; i < argc; ++i)
    {
        if(strcmp(argv[i], "-t") == 0)
            settings |= FLAG_TELL;

        /* Wrong flag */
        else
        {
            /* Printing info (failure) */
            char buf[GLO_MAX_MSG_OUTPUT] = {0, };
            snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Invalid flag (%s).", argv[i]);
            msc_err(buf);
            return NULL;
        }
    }

    /* Printing */
    gph_out(g_graph, stdout, GPH_SET_SORT_ASC | GPH_SET_MARK_DUAL);

    /* Optional: tell */
    if(settings & FLAG_TELL)
        _command_tell(NULL, 0);

    return NULL;

#undef FLAG_TELL
}

/* CMD: For "new" command */
/* Clears the graph */
void *_command_new(char **argv, int argc)
{
#define FLAG_FORCE       (1 << 0)

    /* Check flags */
    int settings = 0;
    for(int i = 0; i < argc; ++i)
    {
        if(strcmp(argv[i], "-f") == 0)
            settings |= FLAG_FORCE;

        /* Wrong flag */
        else
        {
            /* Printing info (failure) */
            char buf[GLO_MAX_MSG_OUTPUT] = {0, };
            snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Invalid flag (%s).", argv[i]);
            msc_err(buf);
            return NULL;
        }
    }

    /* Asking (no force) */
    if(! (settings & FLAG_FORCE))
    {
        /* Input */
        char c = 0;
        do
        {
            msc_war("Do you really want to clear the graph? y/n");
            c = getchar();
            fflush(stdin);
            fflush(stdout);

        } while (tolower(c) != 'y' && tolower(c) != 'n');

        if(tolower(c) == 'y')
            /* OK */;
        else
            return NULL; /* No permission */
    }

    /* Deleting */

    gph_fre(g_graph);
    if((g_graph = gph_new(GLO_DEF_GRAPH_SIZE)) == NULL)
    {
        msc_err("Critical memory error. Closing...");
        exit(EXIT_FAILURE);
    }
    else
    {
        /* Success */
        msc_inf("Operation completed.");
        fflush(stdout);
    }

    return 0;

#undef FLAG_FORCE
}



/* CMD: For "set" command */
/* Changes chosen vertex */
void *_command_set(char **argv, int argc)
{
    /* Validation */
    if(argc < 1)
    {
        msc_err("Missing parameter.");
        return NULL;
    }

    index_t index = 0u;
    index_t arch = 0u;

    /* 1st param */
    /* If has a colon */
    if(argv[0u][strlen(argv[0u]) - 1u] != ':')
    {
        msc_err("Missing colon.");
        return NULL;
    }

    /* Has the colon */
    /* Checking data type */
    /* + temp NULL character to ignore the colon */
    argv[0u][strlen(argv[0u]) - 1u] = '\0';

    if(strcmp(argv[0u], "last") == 0)
        index = GPH_LAST;

    else if(sscanf(argv[0u], "%hu", &index) < 1)
    {
        msc_err("Expected positive integer or \'last\'.");
        return NULL;
    }

    /* Reverting the character */
    argv[0u][strlen(argv[0u]) - 1u] = ':';

    /* Checking if the target index is good */
    if(index >= g_graph->_n && index != GPH_LAST)
    {
        /* Printing info (failure) */
        char buf[GLO_MAX_MSG_OUTPUT] = {0, };
        snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Invalid vertex index (%hu).", index);
        msc_err(buf);
        return NULL;
    }

    /* For each vertex */
    for(int i = 1; i < argc; ++i)
    {
        if(strcmp(argv[i], "last") == 0)
            arch = GPH_LAST;

        else if(sscanf(argv[i], "%hu", &arch) < 1)
        {
            msc_err("Expected positive integer or \'last\'.");
            return NULL;
        }
        else if(arch >= g_graph->_n && arch != GPH_LAST)
        {
            /* Printing info (failure) */
            char buf[GLO_MAX_MSG_OUTPUT] = {0, };
            snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Invalid vertex index (%hu).", arch);
            msc_err(buf);
            return NULL;
        }
    }

    if(index == GPH_LAST)
        index = g_graph->_n - 1u;

    /* The params (should be) good */
    /* Now the target vertex can be reset */
    free(g_graph->_list[index]->_arch);
    free(g_graph->_list[index]);

    /* Alloc */
    if((g_graph->_list[index] = gph_new_vtx(NULL, 0u)) == NULL)
    {
        msc_err("Critical memory error. Closing...");
        exit(EXIT_FAILURE);
    }

    /* Number of added arches */
    size_t added = 0u;

    /* For each vertex (again) */
    for(int i = 1; i < argc; ++i)
    {
        if(strcmp(argv[i], "last") == 0)
            arch = GPH_LAST;

        else if(sscanf(argv[i], "%hu", &arch) < 1)
        {
            msc_err("Expected positive integer or \'last\'."); 
            return NULL;
        }

        size_t result = gph_con(g_graph, index, arch, GPH_ADD);
        if(result == (size_t) -1)
        {
            msc_err("Critical memory error. Closing...");
            exit(EXIT_FAILURE);
        }

        added += result;  
    }

    /* Printing info (success) */
    {
        char buf[GLO_MAX_MSG_OUTPUT];
        snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Set %hu/%d arch(es).", added, argc - 1);
        msc_inf(buf);
    }

    return NULL;
}

/* CMD: For "size" command */
/* Resizes the graph */
void *_command_size(char **argv, int argc)
{
#define FLAG_FORCE       (1 << 0)

    /* Validation */
    if(argc < 1)
    {
        msc_err("Missing parameter.");
        return NULL;
    }

    /* 1st param */
    size_t n = 0u;
    if(sscanf(argv[0u], "%zu", &n) < 1)
    {
        msc_err("Expected positive integer.");
        return NULL;
    }

    /* Check flags */
    int settings = 0;
    for(int i = 1; i < argc; ++i)
    {
        if(strcmp(argv[i], "-f") == 0)
            settings |= FLAG_FORCE;

        /* Wrong flag */
        else
        {
            /* Printing info (failure) */
            char buf[GLO_MAX_MSG_OUTPUT] = {0, };
            snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "Invalid flag (%s).", argv[i]);
            msc_err(buf);
            return NULL;
        }
    }

    /* Previous size */
    const size_t prevs = g_graph->_n;

    /* No changes */
    if(n == g_graph->_n)
    {
        msc_inf("Nothing changed.");
        return NULL;
    }

    /* Deleting */
    else if(n < g_graph->_n)
    {
        /* Asking (no force) */
        if(! (settings & FLAG_FORCE))
        {
            /* Input */
            char c = 0;
            do
            {
                msc_war("Do you really want to shrink the graph? y/n [ ]");
                cur_move(UP, 1u);
                cur_move(RIGHT, strlen("Do you really want to shrink the graph? y/n [ ]") + 1u);

                c = getchar();
                fflush(stdin);

            } while (tolower(c) != 'y' && tolower(c) != 'n');

            if(tolower(c) == 'y')
                /* OK */;
            else
                return NULL; /* No permission */
        }

        /* Deleting approved */
        while(g_graph->_n > n)
        {
            if(gph_del(g_graph, GPH_LAST) > 1u)
            {
                msc_err("Could not finish this operation.");
                return NULL;
            }
        }
    }

    /* Adding */
    else
    {
        while(g_graph->_n < n)
        {
            if(gph_add(g_graph, NULL) != 1u)
            {
                msc_err("Could not finish this operation.");
                return NULL;
            }
        }
    }

    /* Success */
    {
        char buf[GLO_MAX_MSG_OUTPUT];
        snprintf(buf, GLO_MAX_MSG_OUTPUT - 1u, "%s %zu vertex(vertices).", ((prevs < g_graph->_n) ? "Added" : "Deleted"), abs((long long int) prevs - (long long int) g_graph->_n));
        msc_inf(buf);
    }

    return NULL;

#undef FLAG_FORCE
}

/* CMD: For "tell" command */
/* Prints details */
void *_command_tell(char **argv, int argc)
{
    /* Getting info */
    size_t oway = 0u, tway = 0u, isol = 0u;
    gph_cnt(g_graph, &oway, &tway, &isol);

    /* Just printing info */
    fprintf(stdout, "\tsize:               %zu\n", g_graph->_n);
    fprintf(stdout, "\t1-way arches:       %zu\n", oway);
    fprintf(stdout, "\t2-way arches:       %zu\n", tway);
    fprintf(stdout, "\tisolated vertices:  %zu\n", isol);

    return NULL;
}


int main(int argc, char **argv)
{
    /* Initialisation */

    fprintf(stdout, "------------------------\nSimple Graph Generator\n------------------------\n");

//#define MAIN_DBG
#ifdef MAIN_DBG

    g_graph = gph_new(128);
    if(! g_graph)
    {
        return 1;
    }

    size_t r = 0u;

    r += gph_add(g_graph, NULL);
    r += gph_add(g_graph, NULL);
    r += gph_add(g_graph, NULL);
    r += gph_add(g_graph, NULL);
    r += gph_add(g_graph, NULL);
    r += gph_add(g_graph, NULL);

    r += gph_con(g_graph, 0, 1, GPH_ADD);
    r += gph_con(g_graph, 1, 2, GPH_ADD);
    r += gph_con(g_graph, 2, 3, GPH_ADD);
    r += gph_con(g_graph, 3, 4, GPH_ADD);

    gph_out(g_graph, stdout, 0);
    printf("%zu\n", r);

#else
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
    cmd_add("add",      _command_add);
    cmd_add("arch",     _command_arch);
    cmd_add("cls",      _command_cls);
    cmd_add("del",      _command_del);

    cmd_add("exit",     _command_exit);
    cmd_add("quit",     _command_exit);
    cmd_add("q",        _command_exit);

    cmd_add("find",     _command_find);
    cmd_add("help",     _command_help);
    cmd_add("list",     _command_list);
    cmd_add("new",      _command_new);
    cmd_add("set",      _command_set);
    cmd_add("size",     _command_size);
    cmd_add("tell",     _command_tell);
    cmd_add("ai",       _command_ai);
    cmd_add("aitest",   _command_ai_test);
    cmd_add("aimodel",  _command_ai_model);

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

#endif

    return EXIT_SUCCESS;
}
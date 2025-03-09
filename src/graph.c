/*
 *  graph.c
 *
 *  Extends "graph.h".
 * 
 *  By Aleksander Slepowronski.
 */

 #include "graph.h" 

/* Creates new vertex.
 *
 *  conn        - list of connections, can be NULL
 *  nconn       - the list length
 * 
 * Returns NULL if failed.
 */
vertex_t *gph_new_vtx(index_t *conn, index_t nconn)
{
    vertex_t *v = NULL;

    /* Struct alloc */
    if((v = (vertex_t *) calloc(1u, sizeof(vertex_t))) == NULL)
        return NULL;

    /* List alloc */
    if((v->_arch = (index_t *) calloc(1u, sizeof(index_t))) == NULL)
    {
        free(v);
        return NULL;
    }

    /* List copy */
    if(conn && nconn > 0u)
    {
        memcpy((void *)(v->_arch), (void *)(conn), nconn * sizeof(conn[0u]));
        v->_narch = nconn;
    }

    return v;
}

/* Allocates memory for a new, empty graph.
 *
 *  n           - # of vertices to preallocate 
 *
 * Returns NULL if failed.
 */
graph_t *gph_new(size_t n)
{
    assert(n > 0u && n < UINT16_MAX);

    graph_t *g = NULL;

    /* Struct alloc */
    if((g = (graph_t *) malloc(sizeof(graph_t))) == NULL)
        return NULL;

    /* List alloc */
    if((g->_list = (vertex_t **) malloc(sizeof(vertex_t *) * n)) == NULL)
    {
        free(g);
        return NULL;
    }

    /* List init */
    for(size_t i = 0u; i < n; ++i)
    {
        if((g->_list[i] = gph_new_vtx(NULL, 0u)) == NULL)
        {
            /* List freeing if failed */
            for(size_t j = 0u; j < i; ++j)
            {
                free(g->_list[j]->_arch);
                free(g->_list[j]);
            }

            /* Freeing */
            free(g->_list);
            free(g);
            return NULL;
        }
    }

    g->_n    = 0u;
    g->_nmem = n;

    return g;
}

/* Frees graph.
 *
 *  graph       - the victim
 */
void gph_fre(graph_t *graph)
{
    assert(graph);

    /* For each vertex */
    for(size_t i = 0; i < graph->_n; ++i)
    {
        free(graph->_list[i]->_arch);
        free(graph->_list[i]);
    }

    free(graph->_list);
    free(graph);
    graph = NULL;
}

/* Adds new vertex to the graph. Resizes if needed. 
 *
 * graph        - destination
 * copy         - the shallow copy to be used, if NULL - zero-ed instead 
 * 
 * Returns # of added vertices or -1 if failed.
 */
size_t gph_add(graph_t *graph, vertex_t *copy)
{
    assert(graph);

    /* Reallocating if needed */
    if(graph->_n >= graph->_nmem)
    {
        vertex_t **temp = NULL;
        if((temp = (vertex_t **) realloc(graph->_list, sizeof(vertex_t *) * graph->_nmem * 2u)) == NULL)
            return (size_t) -1;

        graph->_list = temp;

        /* New vertices init (if copy is NULL) */
        for(size_t i = graph->_n; (i < graph->_nmem * 2u) && (copy == NULL); ++i)
        {
            if((graph->_list[i] = gph_new_vtx(NULL, 0u)) == NULL)
            {
                /* Partial list freeing if failed */
                for(size_t j = graph->_n; j < i; ++j)
                    free(graph->_list[j]);

                return (size_t) -1;
            }
        }

        graph->_nmem *= 2u;
    }

    /* No copy, creating fresh vertex */
    else if(copy == NULL)
    {
        free(graph->_list[graph->_n]);
        if((graph->_list[graph->_n] = gph_new_vtx(NULL, 0u)) == NULL)
            return (size_t) -1; 
    }

    /* Copy */
    else 
    {
        graph->_list[graph->_n] = copy;
    }
    
    (graph->_n)++;
    return 1u;
}

/* Removes vertex from graph. Moves
 * right vertices to the left.
 *
 * graph        - the graph to be affected
 * n            - the vertex index
 * 
 * Returns # of deleted vertices or -1 if failed.
 */
size_t gph_del(graph_t *graph, index_t index)
{
    assert(graph);

    if(index == GPH_LAST && graph->_n > 0u)
        index = graph->_n - 1u;
    else if(index == GPH_LAST)
        index = 0u;

    /* Validation */
    if(index >= graph->_n)
        return 0u;

    /* How many have been affected? */
    size_t result = 0u;

    /* Moving pointers to the left */
    for(size_t i = index; i < graph->_n - 1u; ++i)
    {
        graph->_list[i] = graph->_list[i + 1u];
        ++result;
    }
    (graph->_n)--;

    /* Fixing arches for each vertex */
    for(size_t i = 0u; i < graph->_n; ++i)
    {
        /* Removing arches to the deleted vertex */
        gph_con(graph, i, index, GPH_DELETE);

        /* Decrementing all the arches that are after the point */
        for(size_t j = 0u; j < graph->_list[i]->_narch; ++j)
        {
            if(graph->_list[i]->_arch[j] >= index && graph->_list[i]->_arch[j] > 0u)
                (graph->_list[i]->_arch[j])--;
        }

        /* Deleting dups */
        for(size_t j = 0u; j < graph->_list[i]->_narch; ++j)
        {
            for(size_t k = 0u; k < graph->_list[i]->_narch; ++k)
            {
                if(k == j)
                    continue;

                if(graph->_list[i]->_arch[j] == graph->_list[i]->_arch[k])
                    gph_con(graph, i, k, GPH_DELETE);
            }
        }
    }

    

    return result;
}

/* Modifies arch from vertex A to vertex B (A -> B).
 *
 *  graph       - the graph containing A and B
 *  a           - A index
 *  b           - B index
 *  op          - operation (add/delete)
 * 
 * Returns # of modified connections or -1 if failed.
 */
size_t gph_con(graph_t *graph, index_t a, index_t b, int op)
{
    assert(graph && (op == GPH_ADD || op == GPH_DELETE));

    /* Conversion */
    if(a == GPH_LAST && graph->_n > 0u)
        a = graph->_n - 1u;
    else if(a == GPH_LAST)
        a = 0u;
    
    if(b == GPH_LAST && graph->_n > 0u)
        b = graph->_n - 1u;
    else if(b == GPH_LAST)
        b = 0u;

    /* Validation */
    if(a >= graph->_n || b >= graph->_n)
        return 0u;

    /* ADDING */
    
    if(op != GPH_ADD)
        goto DEL;

    /* Search for a duplicate */
    for(size_t i = 0u; i < graph->_list[a]->_narch; ++i)
    {
        if(graph->_list[a]->_arch[i] == b)
            return 0u;
    }

    /* List realloc */
    index_t *temp = NULL;
    if((temp = (index_t *) realloc(graph->_list[a]->_arch, sizeof(index_t) * (graph->_list[a]->_narch + 1u))) == NULL)
        return (size_t) -1;

    graph->_list[a]->_arch = temp;
    graph->_list[a]->_arch[graph->_list[a]->_narch] = b;
    ++(graph->_list[a]->_narch);
    
    return 1u;


    /* DELETING */

    DEL:;

    /* Validation */
    if(graph->_list[a]->_narch == 0u)
        return 0u;

    /* Looking for the arch */
    index_t arch_idx = 0u;
    for(; arch_idx < graph->_list[a]->_narch; ++arch_idx)
    {
        if(graph->_list[a]->_arch[arch_idx] == b)
            break;
        
    }

    /* Found? */
    if(arch_idx == graph->_list[a]->_narch)
        return 0u; /* Nah */

    /* Moving to the left */
    for(index_t i = arch_idx; i < graph->_list[a]->_narch - 1u; ++i)
        graph->_list[a]->_arch[i] = graph->_list[a]->_arch[i + 1u];

    (graph->_list[a]->_narch)--;
    return 1u;
}

/* Indicates the type of arch between A and B.
 *
 *  graph       - the graph to be analysed
 *  a           - A index
 *  b           - B index
 * 
 * Returns appropiate type (GPH_NONE/ONEWAY/TWOWAY) or -1 if failed. 
 */
int gph_typ(const graph_t *graph, index_t a, index_t b)
{
    assert(graph);

    /* Conversion */
    if(a == GPH_LAST && graph->_n > 0u)
        a = graph->_n - 1u;
    else if(a == GPH_LAST)
        a = 0u;
    
    if(b == GPH_LAST && graph->_n > 0u)
        b = graph->_n - 1u;
    else if(b == GPH_LAST)
        b = 0u;

    /* Validation */
    if(a >= graph->_n || b >= graph->_n)
        return -1;

    int type = GPH_NONE;

    /* Searching A -> B */
    for(index_t i = 0u; i < graph->_list[a]->_narch; ++i)
    {
        if(graph->_list[a]->_arch[i] == b)
        {
            /* Found */
            ++type;
            break;
        }
    }

    /* Searching B -> A */
    for(index_t i = 0u; i < graph->_list[b]->_narch; ++i)
    {
        if(graph->_list[b]->_arch[i] == a && type == GPH_ONEWAY)
        {
            /* Found */
            ++type;
            break;
        }
    }

    return type;
}

/* Prints graph, line by line.
 *
 *  graph       - the graph
 *  stream      - output stream
 *  settings    - additional info (GPH_SET_*)
 */
void gph_out(const graph_t *graph, FILE *stream, int settings)
{
    assert(graph && stream && stream != stdin);

    /* If size == 0 */
    if(graph->_n == 0u)
    {
        fprintf(stream, "\tEmpty.\n");
        return;
    }

    for(size_t i = 0u; i < graph->_n; ++i)
    {
        /* GPH_SET_SORT_ASC */
        if(settings & GPH_SET_SORT_ASC)
            qsort(graph->_list[i]->_arch, graph->_list[i]->_narch, sizeof(index_t), _gph_sort_asc);

        /* GPH_SET_SORT_DES */
        else if(settings & GPH_SET_SORT_DES)
            qsort(graph->_list[i]->_arch, graph->_list[i]->_narch, sizeof(index_t), _gph_sort_des);


        fprintf(stream, "%16zu: [", i);

        for(size_t j = 0u; j < graph->_list[i]->_narch; ++j)
        {
            /* GPH_SET_MARK_DUAL */
            if(settings & GPH_SET_MARK_DUAL && (stream == stdout || stream == stderr))
            {
                if(i == graph->_list[i]->_arch[j])
                    col_set(MAGENTA);
            
                else if(gph_typ(graph, i, graph->_list[i]->_arch[j]) == GPH_TWOWAY)
                    col_set(CYAN);
            }
            
            fprintf(stream, "%hu", graph->_list[i]->_arch[j]);

            if(settings & GPH_SET_MARK_DUAL && (stream == stdout || stream == stderr))
                col_set(COLOR_DEFAULT);

            if(j < graph->_list[i]->_narch - 1u)
                fprintf(stream, ", ");
        }
        fprintf(stream, "]\t");

        fprintf(stream, "\n");
    }
}

/* Gives statistics
 *
 *  graph       - the graph to be analysed 
 *  o_single    - OUT, # of 1-way arches 
 *  o_double    - OUT, # of 2-way arches 
 *  o_isolated  - OUT, # of isolated vertices
 */
void gph_cnt(const graph_t *graph, size_t *o_single, size_t *o_double, size_t *o_isolated)
{
    assert(graph && o_single && o_double && o_isolated);

    *o_single = 0u;
    *o_double = 0u;
    *o_isolated = 0u;

    /* For each vertex */
    for(size_t i = 0u; i < graph->_n; ++i)
    {
        /* If isolated */
        if(graph->_list[i]->_narch == 0u)
        {
            ++(*o_isolated);
            continue;
        }

        /* For each arch */
        for(size_t j = 0u; j < graph->_list[i]->_narch; ++j)
        {
            const int type = gph_typ(graph, i, graph->_list[i]->_arch[j]);
            if(type == GPH_ONEWAY)
                ++(*o_single);
            else if(type == GPH_TWOWAY)
                ++(*o_double);
        }
    }

    /* Don't count separately */
    (*o_double) /= 2u;
}



/* Sorts indexes ascending */
int _gph_sort_asc(const void *a, const void *b)
{
    index_t idxa = *((index_t *) a);
    index_t idxb = *((index_t *) b);

    return ((int32_t) idxa) - ((int32_t) idxb);
}

/* Sorts indexes descending */
int _gph_sort_des(const void *a, const void *b)
{
    index_t idxa = *((index_t *) a);
    index_t idxb = *((index_t *) b);

    return ((int32_t) idxb) - ((int32_t) idxa);
}
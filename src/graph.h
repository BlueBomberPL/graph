/*
 *  graph.h
 *
 *  Implements simple graph object.
 *  Each graph consists of finite number
 *  of vertices, connected via arches.
 * 
 *  By Aleksander Slepowronski.
 */

 #ifndef _GRAPH_GRAPH_H_FILE_
 #define _GRAPH_GRAPH_H_FILE_

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "terminal.h"

#define GPH_ADD                 (0x01)          /* ADD operation (on arch) */
#define GPH_DELETE              (0x02)          /* DELETE operation (on arch) */

#define GPH_LAST                ((index_t) -1) /* Last index of a graph */

#define GPH_NONE                (0x00)          /* No arch */
#define GPH_ONEWAY              (0x01)          /* One way arch (A -> B) XOR (B -> A) */
#define GPH_TWOWAY              (0x02)          /* Two way arch (A -> B) AND (B -> A) */

#define GPH_SET_MARK_DUAL       (1 << 0)        /* PRINT: Marks dual arches (A <-> B) */
#define GPH_SET_SORT_ASC        (1 << 1)        /* PRINT: Sorts arches for each vertex (ascending) */
#define GPH_SET_SORT_DES        (1 << 2)        /* PRINT: Sorts arches for each vertex (descending) */


/* An index */
typedef uint16_t index_t;

/* A vertex */
typedef struct _gph_vertx_t
{
    index_t   *_arch;          /* List of arches coming from this vertex */
    index_t   _narch;         /* The list length */

} vertex_t;

/* A graph */
typedef struct _gph_graph_t
{
    size_t      _n;              /* Current # of valid vertices. */
    size_t      _nmem;           /* Allocated memory size (# of possible vertices) */

    vertex_t  **_list;           /* List of vertices (pointers) */

} graph_t;


/* Creates new vertex.
 *
 *  conn        - list of connections (arches), can be NULL
 *  nconn       - the list length
 * 
 * Returns NULL if failed.
 */
vertex_t       *gph_new_vtx(index_t *conn, index_t nconn);

/* Allocates memory for new, empty graph.
 *
 *  n           - # of vertices to preallocate 
 *
 * Returns NULL if failed.
 */
graph_t        *gph_new(size_t n);

/* Frees graph.
 *
 *  graph       - the victim
 */
void            gph_fre(graph_t *graph);

/* Adds new vertex to graph. 
 *
 *  graph       - destination
 *  copy        - the shallow copy to be used, if NULL - zero-ed instead 
 * 
 * Returns # of added vertices or -1 if failed.
 */
size_t          gph_add(graph_t *graph, vertex_t *copy);

/* Removes vertex from graph. Moves
 * right vertices to the left.
 *
 *  graph       - the graph to be affected
 *  n           - the vertex index, GPH_LAST can be used
 * 
 * Returns # of deleted vertices or -1 if failed.
 */
size_t          gph_del(graph_t *graph, index_t index);

/* Modifies arch from vertex A to vertex B (A -> B).
 *
 *  graph       - the graph containing A and B
 *  a           - A index
 *  b           - B index
 *  op          - operation (add/delete)
 * 
 * Returns # of modified connections or -1 if failed.
 */
size_t          gph_con(graph_t *graph, index_t a, index_t b, int op);

/* Indicates the type of arch between A and B.
 *
 *  graph       - the graph to be analysed
 *  a           - A index
 *  b           - B index
 * 
 * Returns appropiate type (GPH_NONE/ONEWAY/TWOWAY) or -1 if failed. 
 */
int             gph_typ(const graph_t *graph, index_t a, index_t b);

/* Prints graph, line by line.
 *
 *  graph       - the graph
 *  stream      - output stream
 *  settings    - additional info (GPH_SET_*)
 */
void            gph_out(const graph_t *graph, FILE *stream, int settings);

/* Gives statistics
 *
 *  graph       - the graph to be analysed 
 *  o_single    - OUT, # of 1-way arches 
 *  o_double    - OUT, # of 2-way arches 
 *  o_isolated  - OUT, # of isolated vertices
 */
void            gph_cnt(const graph_t *graph, size_t *o_single, size_t *o_double, size_t *o_isolated);

#endif /* _GRAPH_GRAPH_H_FILE_ */
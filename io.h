#ifndef IO_H
#define IO_H

#include "graph.h"

int load_network(Graph *g, const char *filename);
int save_network(Graph *g, const char *filename);
int load_edge_list(Graph *g,const char *filename);
#endif
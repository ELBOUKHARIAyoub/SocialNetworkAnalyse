#ifndef ALGORITHMS_H
#define ALGORITHMS_H
#include "graph.h"


int shortest_path(Graph *g, int src, int dst, int *path);
int recommend_friends(Graph *g, int idx, int *result, int max_results);
int count_communities(Graph *g);
#endif

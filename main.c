#include <stdio.h>
#include "graph.h"

#include"io.h"

int main(void)
{
    Graph g;
    graph_init(&g);
    load_network(&g, "data/network.txt");
    return 0;
}
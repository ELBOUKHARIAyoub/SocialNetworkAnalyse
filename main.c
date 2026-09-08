#include <stdio.h>
#include "graph.h"

#include"io.h"

int main(void)
{
    static Graph g;
graph_init(&g);


load_network(&g, "data/facebook_named.txt");
printf("count = %d\n", g.count);
display(&g);
}
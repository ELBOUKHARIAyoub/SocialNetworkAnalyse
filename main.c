#include <stdio.h>
#include "graph.h"

#include <stdio.h>
#include "graph.h"

int main(void)
{
    Graph g;
    graph_init(&g);

    int a = add_user(&g, "Ayoub");
    int i = add_user(&g, "Ismail");
    int o = add_user(&g, "Omar");

    add_friendship(&g, a, i);
    add_friendship(&g, a, o);

    display(&g);
    printf("--- removing Ayoub-Ismail ---\n");
remove_friendship(&g, a, i);
display(&g);

remove_friendship(&g, a, i);   /* should say "they are not friends" */

    return 0;
}
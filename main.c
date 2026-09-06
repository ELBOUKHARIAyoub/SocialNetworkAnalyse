#include <stdio.h>
#include "graph.h"

int main(void)
{
    Graph g;
    graph_init(&g);

    printf("%d\n", add_user(&g, "Ayoub"));
    printf("%d\n", add_user(&g, "Ismail"));
    printf("%d\n", add_user(&g, "Ayoub"));   /* should be -1 */
    printf("%d\n", find_user(&g, "Ismail")); /* should be 1 */
    printf("%d\n", find_user(&g, "Omar"));   /* should be -1 */

    return 0;
}
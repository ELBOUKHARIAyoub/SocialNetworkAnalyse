#include <stdio.h>
#include "graph.h"
#include "io.h"
#include "algorithms.h"

static Graph g;
static void test_path(Graph *g, int a, int b);
static void test_recommend(Graph *g, int idx);

int main(void)
{
    graph_init(&g);
    load_network(&g, "data/facebook_named.txt");
    printf("count = %d\n", g.count);
test_path(&g, 0, 3000);
test_path(&g, 500, 4000);
test_path(&g, 0, 0);      /* same user, should be 0 hops */
test_recommend(&g, 0);
    free_graph(&g);
    return 0;
}
static void test_recommend(Graph *g, int idx)
{
    int result[5];
    int n = recommend_friends(g, idx, result, 5);
    printf("%d recommendations for %s:\n", n, g->users[idx].name);
    for (int i = 0; i < n; i++) {
        printf("  %s\n", g->users[result[i]].name);
    }
}
static void test_path(Graph *g, int a, int b)
{
    int path[MAX_USERS];
    int hops = shortest_path(g, a, b, path);

    if (hops >= 0) {
        for (int i = 0; i <= hops; i++) {
            printf("%s ", g->users[path[i]].name);
        }
        printf("(%d hops)\n", hops);
    }
}
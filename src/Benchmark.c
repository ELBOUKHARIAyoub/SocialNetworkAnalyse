/* benchmark.c — timing harness for the social network project.
 *
 * Generates random sparse graphs of increasing size, times BFS on each,
 * and writes output/timings.csv for the plotting notebook.
 * Also times the real SNAP dataset as a separate measurement.
 *
 * Build:  gcc -Wall -O2 graph.c io.c algorithms.c benchmark.c -o benchmark
 * Run:    ./benchmark
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "graph.h"
#include "io.h"
#include "algorithms.h"

#define AVG_DEGREE   20        /* edges per node — keeps the graph sparse as V grows */
#define BFS_RUNS    200        /* BFS calls per measurement, averaged */

static Graph g;                /* static: the struct is far too large for the stack */
static int   path[MAX_USERS];

/* ---------------------------------------------------------------- */

/* Count edges by walking every chain. Each edge appears twice, so halve it. */
static int count_edges(Graph *g)
{
    long total = 0;
    for (int i = 0; i < g->count; i++) {
        if (!g->users[i].active) continue;
        for (EdgeNode *c = g->users[i].head; c != NULL; c = c->next) {
            total++;
        }
    }
    return (int)(total / 2);
}

/* Build a random sparse graph: n users, about n*AVG_DEGREE/2 friendships.
 * Duplicate pairs and self-loops are rejected by add_friendship's own guards. */
static void generate_random_graph(Graph *g, int n)
{
    char name[MAX_NAME];

    graph_init(g);

    for (int i = 0; i < n; i++) {
        sprintf(name, "user_%d", i);
        add_user(g, name);
    }

    long target = (long)n * AVG_DEGREE / 2;
    long added = 0, attempts = 0;
    long limit = target * 10;          /* stop trying if the graph saturates */

    while (added < target && attempts < limit) {
        int a = rand() % n;
        int b = rand() % n;
        attempts++;
        if (a == b) continue;
        if (add_friendship(g, a, b) > 0) added++;
    }
}

/* Average milliseconds for one BFS, over BFS_RUNS random source/target pairs.
 * A single BFS on a small graph is below the clock resolution, so we
 * measure a batch and divide. */
static double time_bfs(Graph *g, int runs)
{
    if (g->count < 2) return 0.0;

    clock_t start = clock();
    for (int i = 0; i < runs; i++) {
        int src = rand() % g->count;
        int dst = rand() % g->count;
        shortest_path(g, src, dst, path);
    }
    clock_t end = clock();

    double total_ms = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
    return total_ms / runs;
}

/* Time one full connected-components pass. */
static double time_components(Graph *g, int runs)
{
    clock_t start = clock();
    for (int i = 0; i < runs; i++) {
        count_communities(g);
    }
    clock_t end = clock();

    double total_ms = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
    return total_ms / runs;
}

/* ---------------------------------------------------------------- */

int main(void)
{
    const int sizes[] = { 10, 50, 100, 500, 1000, 2000, 3000, 4000, 5000 };
    const int n_sizes = (int)(sizeof(sizes) / sizeof(sizes[0]));

    srand(42);                 /* fixed seed so results are reproducible */

    FILE *f = fopen("output/timings.csv", "w");
    if (f == NULL) {
        printf("Cannot open output/timings.csv — does the output/ folder exist?\n");
        return 1;
    }
    fprintf(f, "nodes,edges,milliseconds,components_ms\n");

    printf("%-8s %-10s %-14s %-14s\n", "nodes", "edges", "bfs (ms)", "components (ms)");
    printf("--------------------------------------------------------\n");

    for (int i = 0; i < n_sizes; i++) {
        int n = sizes[i];
        if (n > MAX_USERS) continue;

        generate_random_graph(&g, n);
        int e = count_edges(&g);

        /* Small graphs finish too fast to measure once — run more of them. */
        int runs = (n <= 100) ? BFS_RUNS * 20 : BFS_RUNS;

        double bfs_ms  = time_bfs(&g, runs);
        double comp_ms = time_components(&g, (n <= 100) ? 200 : 20);

        printf("%-8d %-10d %-14.6f %-14.6f\n", n, e, bfs_ms, comp_ms);
        fprintf(f, "%d,%d,%.6f,%.6f\n", n, e, bfs_ms, comp_ms);
        fflush(f);

        free_graph(&g);
    }

    fclose(f);
    printf("\nWritten to output/timings.csv\n");

    /* ---- the real dataset, measured separately ---- */
    printf("\nReal dataset (SNAP ego-Facebook)\n");
    printf("--------------------------------------------------------\n");

    graph_init(&g);
    if (load_network(&g, "data/facebook_named.txt")) {
        int e = count_edges(&g);
        double bfs_ms  = time_bfs(&g, BFS_RUNS);
        double comp_ms = time_components(&g, 20);
        double density = 2.0 * e / ((double)g.count * (g.count - 1));

        printf("nodes           = %d\n", g.count);
        printf("edges           = %d\n", e);
        printf("density         = %.5f\n", density);
        printf("avg degree      = %.1f\n", 2.0 * e / g.count);
        printf("bfs             = %.6f ms\n", bfs_ms);
        printf("components      = %.6f ms\n", comp_ms);

        FILE *r = fopen("output/real_dataset.csv", "w");
        if (r != NULL) {
            fprintf(r, "nodes,edges,density,bfs_ms,components_ms\n");
            fprintf(r, "%d,%d,%.6f,%.6f,%.6f\n",
                    g.count, e, density, bfs_ms, comp_ms);
            fclose(r);
            printf("\nWritten to output/real_dataset.csv\n");
        }
        free_graph(&g);
    } else {
        printf("Could not load data/facebook_named.txt — skipped.\n");
    }

    return 0;
}
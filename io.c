
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int load_network(Graph *g, const char *filename){
    char buffer[256];
FILE *f = fopen(filename, "r");
if (f==NULL) return 0;
while(fgets(buffer, sizeof(buffer), f)) printf("%s",buffer);
fclose(f);
return 1;
}
int save_network(Graph *g, const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (f == NULL) return 0;

    fprintf(f, "#USERS\n");
    for (int i = 0; i < g->count; i++) {
        if (!g->users[i].active) continue;
        fprintf(f, "%d %s\n", i, g->users[i].name);
    }

    fprintf(f, "#EDGES\n");
    for (int i = 0; i < g->count; i++) {
        if (!g->users[i].active) continue;
        EdgeNode *cur = g->users[i].head;
        while (cur != NULL) {
            if (i < cur->neighbor) {
                fprintf(f, "%d %d\n", i, cur->neighbor);
            }
            cur = cur->next;
        }
    }

    fclose(f);
    return 1;
}
int load_edge_list(Graph *g,const char *filename);

#include "io.h"
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int load_network(Graph *g, const char *filename){
    char buffer[256];
FILE *f = fopen(filename, "r");
if (f==NULL) return 0;
    int mode = 0; // 1 for users, 2 for edges

while(fgets(buffer, sizeof(buffer), f)){
    buffer[strcspn(buffer, "\r\n")] = '\0';
        if (buffer[0] == '\0' || buffer[0] == '\n' )
        continue;
    if (strcmp(buffer, "#USERS") == 0)
    {
        mode = 1; continue;
    }else if (strcmp(buffer, "#EDGES") == 0)
    {
        mode = 2; continue;
    }
    if (mode == 1) {
        int idx;
        char name[MAX_NAME];
        if (sscanf(buffer, "%d %31s", &idx, name) == 2) {
            add_user(g, name);
        }
        
    }else if (mode == 2) {
        int idx1, idx2;
        if (sscanf(buffer, "%d %d", &idx1, &idx2) == 2) {
            add_friendship(g, idx1, idx2);
        }
        
    }
    
     printf("%s",buffer);
}
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
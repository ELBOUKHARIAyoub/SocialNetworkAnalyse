#include "algorithms.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int shortest_path(Graph *g, int src, int dst, int *path){

    if(src<0 || dst<0 || src>=g->count || dst>=g->count){
        printf("invalid index\n");
        return -1;
    }
    if(!g->users[src].active || !g->users[dst].active){
        printf("one or both users are not active\n");
        return -1;
    }
    if(src == dst){
        path[0] = src;
        return 0;
    }

    int *visited = calloc(g->count, sizeof(int));
    int *parent = malloc(g->count * sizeof(int));
    int *queue = malloc(g->count * sizeof(int));
    if(visited == NULL || parent == NULL || queue == NULL){
        printf("out of memory\n");
        free(visited);
        free(parent);
        free(queue);
        return -1;
    }
    for(int i=0;i<g->count;i++){
        parent[i] = -1;
    }

    int head = 0, tail = 0;
    visited[src] = 1;
    queue[tail++] = src;

    int found = 0;
    while(head < tail){
        int cur = queue[head++];

        if(cur == dst){
            found = 1;
            break;
        }

        EdgeNode *curr = g->users[cur].head;
        while(curr != NULL){
            int n = curr->neighbor;
            if(!visited[n]){
                visited[n] = 1;
                parent[n] = cur;
                queue[tail++] = n;
            }
            curr = curr->next;
        }
    }

    int hops = -1;
    if(found){
        int len = 0;
        for(int node = dst; node != -1; node = parent[node]){
            len++;
        }
        int node = dst;
        for(int i = len - 1; i >= 0; i--){
            path[i] = node;
            node = parent[node];
        }
        hops = len - 1;
    } else {
        printf("no path exists\n");
    }

    free(visited);
    free(parent);
    free(queue);
    return hops;
}

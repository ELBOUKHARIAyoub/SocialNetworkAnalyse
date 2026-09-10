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
int recommend_friends(Graph *g, int idx, int *result, int max_results){
    //firt lets guards that index is valid and user active
    if(idx<0 || idx>=g->count){
        printf("invalid index\n");
        return -1;
    }
    if(!g->users[idx].active){
        printf("user is not active\n");
        return -1;
    }

    int *counts = calloc(g->count, sizeof(int));
    if(counts == NULL){
        printf("out of memory\n");
        return -1;
    }

    EdgeNode *f = g->users[idx].head;
    while (f != NULL) {

        EdgeNode *c = g->users[f->neighbor].head;   /* inner: their friends */
        while (c != NULL) {
            int cand = c->neighbor;
            if(cand != idx && g->matrix[idx][cand] != 1){
                counts[cand]++;
            }
            c = c->next;
        }

        f = f->next;
    }

    // now pick the top max_results by count
    int filled = 0;
    for(int k = 0; k < max_results; k++){
        int best = -1;
        for(int c = 0; c < g->count; c++){
            if(best == -1 || counts[c] > counts[best]){
                best = c;
            }
        }
        if(best == -1 || counts[best] == 0){
            break;
        }
        result[filled++] = best;
        counts[best] = 0;
    }

    free(counts);
    return filled;
}

int count_communities(Graph *g){

    int *visited = calloc(g->count, sizeof(int));
    int *queue = malloc(g->count * sizeof(int));
    if(visited == NULL || queue == NULL){
        printf("out of memory\n");
        free(visited);
        free(queue);
        return -1;
    }

    int components = 0;

    for(int i = 0; i < g->count; i++){
        if(!g->users[i].active) continue;
        if(visited[i]) continue;

        components++;

        // BFS from i, marking everything reachable as visited
        int head = 0, tail = 0;
        visited[i] = 1;
        queue[tail++] = i;

        while(head < tail){
            int cur = queue[head++];

            EdgeNode *curr = g->users[cur].head;
            while(curr != NULL){
                int n = curr->neighbor;
                if(!visited[n]){
                    visited[n] = 1;
                    queue[tail++] = n;
                }
                curr = curr->next;
            }
        }
    }

    free(visited);
    free(queue);
    return components;
}
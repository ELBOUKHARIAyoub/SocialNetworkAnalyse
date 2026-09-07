#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void unlink_neighbor(User *u, int target);

void graph_init(Graph *g)
{
    g->count = 0;

    for (int i = 0; i < MAX_USERS; i++) {
        /* head, active, and optionally name[0] = '\0' */
        g->users[i].head = NULL;
        g->users[i].active = 0;
       /* g->users[i].name = NULL;*/
        for (int j = 0; j < MAX_USERS; j++) {
            g->matrix[i][j]=0;
            
        }
    }
}


int add_user(Graph *g, const char *name){
int idx = g->count;
    if(find_user(g,name) >=0){
        printf("user already exist\n");
        return -1;
    }else{
        if(g->count>=MAX_USERS){
            printf("NO SPACE, USER OVER FLOW\n");
        return -1;
        }else{
           
            strncpy(g->users[idx].name, name, MAX_NAME - 1);
            g->users[idx].name[MAX_NAME - 1] = '\0';
            g->users[idx].head = NULL;
            g->users[idx].active = 1;
            g->count++;
            return idx;
        }
    }

}
int find_user(Graph *g, const char *name){

    for(int i =0;i< g->count ; i++){
        if(strcmp(g->users[i].name,name)==0 && g->users[i].active == 1){
            printf("the user name is %s, his index is %d",name,i);
            return i;
        
}

    } 
printf("the user dosn t exist ");
return -1;
}   
int remove_user(Graph *g,int idx){
    
    if (idx < 0 || idx >= g->count) {
    printf("invalid index\n");
    return -1;
}
if (!g->users[idx].active) {
    printf("user is not active\n");
    return 0;
}
EdgeNode *cur = g->users[idx].head;
while (cur != NULL) {
    unlink_neighbor(&g->users[cur->neighbor], idx);
    
    cur = cur->next;
}
g->users[idx].active = 0;
g->users[idx].name[0] = '\0';
cur = g->users[idx].head;
while (cur != NULL) {
    EdgeNode *next = cur->next;   /* save before freeing */
    free(cur);
    cur = next;
}
g->users[idx].head = NULL;
for (int i = 0; i < g->count; i++) {
    g->matrix[idx][i] = 0;
    g->matrix[i][idx] = 0;
}
return 1;
}
int add_friendship(Graph *g, int idx1, int idx2){
if(idx1<0 || idx2<0 || idx1>=g->count || idx2>=g->count){
    printf("invalid index\n");
    return -1;
}else if(idx1==idx2){
    printf("cannot add friendship with self\n");
    return -1;
}else if (g->users[idx1].active == 0 || g->users[idx2].active == 0) {
    printf("users are not active\n");
    return -1;
}else if (g->matrix[idx1][idx2] == 1) {
    printf("already friends\n");
    return 0;
}
EdgeNode *node1 = malloc(sizeof(EdgeNode));
if (node1 == NULL) {
    printf("out of memory\n");
    return 0;
}
node1->neighbor = idx2;
node1->next = g->users[idx1].head;
g->users[idx1].head = node1;
EdgeNode *node2 = malloc(sizeof(EdgeNode));
if (node2 == NULL) {
    printf("out of memory\n");
   
    return 0;
}
node2->neighbor = idx1;
node2->next = g->users[idx2].head;
g->users[idx2].head = node2;
g->matrix[idx1][idx2] = 1;
g->matrix[idx2][idx1] = 1;
return 1;
}

static void unlink_neighbor(User *u, int target){
  EdgeNode *curr = u->head;
  EdgeNode *prev = NULL; 
  while (curr != NULL) {
    if (curr->neighbor == target) {
        if (prev == NULL) {
            u->head = curr->next;
        } else {
            prev->next = curr->next;
        }
        free(curr);
        return;
    }
    prev = curr;
    curr = curr->next;
}
}
int remove_friendship(Graph *g, int idx1, int idx2){

    if(idx1<0 || idx2<0 || idx1>=g->count || idx2>=g->count){
    printf("invalid index\n");
    return -1;
}
if(!g->users[idx1].active || !g->users[idx2].active){
    printf("one or both users are not active\n");
    return -1;
}
if (g->matrix[idx1][idx2] == 0) {
    printf("they are not friends\n");
    return 0;
}
unlink_neighbor(&g->users[idx1], idx2);
unlink_neighbor(&g->users[idx2], idx1);
g->matrix[idx1][idx2] = 0;
g->matrix[idx2][idx1] = 0;
return 1;
}
void display(Graph *g){
    for(int i=0;i<g->count;i++){
         if (!g->users[i].active) continue;
        if(g->users[i].active == 1){
            printf("user %s index %d\n",g->users[i].name,i);
        }
        EdgeNode *curr = g->users[i].head;
        while(curr != NULL){
            printf("friend index %d and name %s\n",curr->neighbor,g->users[curr->neighbor].name );
            curr = curr->next;
        }
    }
}
void free_graph(Graph *g){

    for (int i = 0; i < g->count; i++) {
        EdgeNode *cur = g->users[i].head;
        while (cur != NULL) {
            EdgeNode *next = cur->next;
            free(cur);
            cur = next;
        }
        g->users[i].head = NULL;
    }
    g->count = 0;
}
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
int remove_user(Graph *g,int idx);
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

int remove_friendship(Graph *g, int idx1, int idx2);
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
void free_graph(Graph *g);
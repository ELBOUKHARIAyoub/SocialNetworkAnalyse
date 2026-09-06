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
int add_friendship(Graph *g, int idx1, int idx2);
int remove_friendship(Graph *g, int idx1, int idx2);
void display(Graph *g);
void free_graph(Graph *g);
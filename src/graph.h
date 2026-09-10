#ifndef GRAPH_H
#define GRAPH_H

#define MAX_USERS 5000
#define MAX_NAME 32


struct EdgeNode {
    int neighbor;
    struct EdgeNode *next;
};

typedef struct EdgeNode EdgeNode;

typedef struct {
    char name[MAX_NAME];
    EdgeNode *head;
    int active;
} User;

typedef struct {
    User users[MAX_USERS];
    int matrix[MAX_USERS][MAX_USERS];
    int count;
} Graph;

void graph_init(Graph *g);
int add_user(Graph *g, const char *name);
int find_user(Graph *g, const char *name);
int remove_user(Graph *g,int idx);
int add_friendship(Graph *g, int idx1, int idx2);
int remove_friendship(Graph *g, int idx1, int idx2);
void display(Graph *g);
void free_graph(Graph *g);


#endif

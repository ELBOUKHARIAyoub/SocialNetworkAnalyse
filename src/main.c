#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "io.h"
#include "colors.h"
#include "algorithms.h"     /* add this at the top with your other includes */

#ifdef _WIN32
#include <windows.h>
static void enable_ansi(void)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD mode = 0;
    if (!GetConsoleMode(hOut, &mode)) return;
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#endif

#define DEFAULT_NETWORK_FILE "data/facebook_named.txt"

/* Read one line of input, stripping the trailing newline.
 * Returns 0 on success, -1 on EOF/read error. */
static int read_line(char *buf, size_t size, const char *prompt)
{
    printf(COLOR_MAGENTA "%s" COLOR_RESET, prompt);
    if (!fgets(buf, (int)size, stdin)) {
        return -1;
    }
    buf[strcspn(buf, "\r\n")] = '\0';
    return 0;
}

static void press_enter(void)
{
    printf(COLOR_YELLOW "\nPress Enter to continue..." COLOR_RESET);
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static void print_banner(void)
{
    printf(COLOR_CYAN COLOR_BOLD
           "==================================================\n"
           "        SOCIAL NETWORK ANALYSIS - MAIN MENU\n"
           "==================================================\n"
           COLOR_RESET);
}

static void print_menu(int user_count)
{
    printf(COLOR_BLUE "Loaded users: " COLOR_RESET "%d\n\n", user_count);

    printf(COLOR_GREEN " 1." COLOR_RESET " Load network from file\n");
    printf(COLOR_GREEN " 2." COLOR_RESET " Display network\n");
    printf(COLOR_GREEN " 3." COLOR_RESET " Add user\n");
    printf(COLOR_GREEN " 4." COLOR_RESET " Add friendship\n");
    printf(COLOR_GREEN " 5." COLOR_RESET " Remove friendship\n");
    printf(COLOR_GREEN " 6." COLOR_RESET " Remove user\n");
    printf(COLOR_YELLOW " 7." COLOR_RESET " Shortest path\n");
    printf(COLOR_YELLOW " 8." COLOR_RESET " Recommend friends\n");
    printf(COLOR_YELLOW " 9." COLOR_RESET " Count communities\n");
    printf(COLOR_YELLOW "10." COLOR_RESET " Save network\n");
    printf(COLOR_RED  " 0." COLOR_RESET " Exit\n");
    printf(COLOR_CYAN "==================================================\n" COLOR_RESET);
}

/* Prints the menu choice prompt and reads it.
 * Returns the choice, or -1 on EOF (treated as "exit"). */
static int get_choice(void)
{
    char buf[64];
    printf(COLOR_MAGENTA COLOR_BOLD "\nEnter your choice: " COLOR_RESET);
    if (!fgets(buf, sizeof(buf), stdin)) return -1;

    int value;
    if (sscanf(buf, "%d", &value) != 1) return -2; /* not a number */
    return value;
}

static void do_load_network(Graph *g)
{
    char filename[256];
    printf(COLOR_WHITE "(default: %s)\n" COLOR_RESET, DEFAULT_NETWORK_FILE);
    if (read_line(filename, sizeof(filename), "Enter filename: ") < 0) return;
    if (filename[0] == '\0') {
        strcpy(filename, DEFAULT_NETWORK_FILE);
    }

    /* Loading replaces whatever is currently in memory. */
    free_graph(g);
    graph_init(g);

    if (load_network(g, filename)) {
        printf(COLOR_GREEN "\nNetwork loaded from '%s' -- %d users.\n" COLOR_RESET,
               filename, g->count);
    } else {
        printf(COLOR_RED "\nCould not open '%s'.\n" COLOR_RESET, filename);
    }
}


static void do_save_network(Graph *g)
{
    char filename[256];
    if (read_line(filename, sizeof(filename), "Enter filename to save to: ") < 0) return;
    if (filename[0] == '\0') {
        printf(COLOR_RED "\nNo filename given.\n" COLOR_RESET);
        return;
    }

    if (save_network(g, filename)) {
        printf(COLOR_GREEN "\nNetwork saved to '%s' -- %d users.\n" COLOR_RESET,
               filename, g->count);
    } else {
        printf(COLOR_RED "\nCould not write to '%s'.\n" COLOR_RESET, filename);
    }
}

static void do_display_network(Graph *g)
{
    if (g->count == 0) {
        printf(COLOR_YELLOW "Network is empty.\n" COLOR_RESET);
        return;
    }
    display(g);
}

static void do_add_user(Graph *g)
{
    char name[MAX_NAME];
    if (read_line(name, sizeof(name), "Enter name: ") < 0) return;

    int idx = add_user(g, name);
    if (idx >= 0) {
        printf(COLOR_GREEN "\nUser '%s' added with index %d.\n" COLOR_RESET, name, idx);
    } else {
        printf(COLOR_RED "\nCould not add user '%s'.\n" COLOR_RESET, name);
    }
}

/* Reads a name and resolves it to an index. Returns -1 if not found or on EOF. */
static int read_user_index(Graph *g, const char *prompt)
{
    char name[MAX_NAME];
    if (read_line(name, sizeof(name), prompt) < 0) return -1;

    int idx = find_user(g, name);
    if (idx < 0) {
        printf(COLOR_RED "\nUser '%s' not found.\n" COLOR_RESET, name);
    }
    return idx;
}

static void do_add_friendship(Graph *g)
{
    int idx1 = read_user_index(g, "Enter first user name: ");
    if (idx1 < 0) return;
    int idx2 = read_user_index(g, "Enter second user name: ");
    if (idx2 < 0) return;

    if (add_friendship(g, idx1, idx2) > 0) {
        printf(COLOR_GREEN "\nFriendship added between %s and %s.\n" COLOR_RESET,
               g->users[idx1].name, g->users[idx2].name);
    }
}

static void do_remove_friendship(Graph *g)
{
    int idx1 = read_user_index(g, "Enter first user name: ");
    if (idx1 < 0) return;
    int idx2 = read_user_index(g, "Enter second user name: ");
    if (idx2 < 0) return;

    if (remove_friendship(g, idx1, idx2) > 0) {
        printf(COLOR_GREEN "\nFriendship removed between %s and %s.\n" COLOR_RESET,
               g->users[idx1].name, g->users[idx2].name);
    }
}

static void do_remove_user(Graph *g)
{
    char name[MAX_NAME];
    if (read_line(name, sizeof(name), "Enter user name to remove: ") < 0) return;

    int idx = find_user(g, name);
    if (idx < 0) {
        printf(COLOR_RED "\nUser '%s' not found.\n" COLOR_RESET, name);
        return;
    }

    if (remove_user(g, idx) > 0) {
        printf(COLOR_GREEN "\nUser '%s' removed.\n" COLOR_RESET, name);
    }
}

static void do_shortest_path(Graph *g)
{
    char name1[MAX_NAME], name2[MAX_NAME];
    if (read_line(name1, sizeof(name1), "Enter first user name: ") < 0) return;
    if (read_line(name2, sizeof(name2), "Enter second user name: ") < 0) return;

    int src = find_user(g, name1);
    int dst = find_user(g, name2);
    if (src < 0 || dst < 0) {
        printf(COLOR_RED "\nOne or both users not found.\n" COLOR_RESET);
        return;
    }

    int path[MAX_USERS];
    int hops = shortest_path(g, src, dst, path);

    if (hops >= 0) {
        printf(COLOR_GREEN "\n");
        for (int i = 0; i <= hops; i++) {
            printf("%s ", g->users[path[i]].name);
        }
        printf("(%d hops)\n" COLOR_RESET, hops);
    }
    /* shortest_path already prints its own error on failure */
}

static void do_recommend(Graph *g)
{
    char name[MAX_NAME];
    if (read_line(name, sizeof(name), "Enter user name: ") < 0) return;

    int idx = find_user(g, name);
    if (idx < 0) {
        printf(COLOR_RED "\nUser '%s' not found.\n" COLOR_RESET, name);
        return;
    }

    int result[5];
    int n = recommend_friends(g, idx, result, 5);
    if (n < 0) return; /* recommend_friends already printed the error */

    printf(COLOR_GREEN "\n%d recommendation(s) for %s:\n" COLOR_RESET, n, g->users[idx].name);
    for (int i = 0; i < n; i++) {
        printf("  %s\n", g->users[result[i]].name);
    }
}

static void do_count_communities(Graph *g)
{
    int n = count_communities(g);
    if (n >= 0) {
        printf(COLOR_GREEN "\nNumber of communities: %d\n" COLOR_RESET, n);
    }
}

int main(void)
{
#ifdef _WIN32
    enable_ansi();
#endif

    static Graph g;
    graph_init(&g);

    int running = 1;
    while (running) {
        /* printf(CLEAR_SCREEN); */   /* uncomment for a clean demo screen */
        print_banner();
        print_menu(g.count);

        int choice = get_choice();

        switch (choice) {
            case 1:  do_load_network(&g); break;
            case 2:  do_display_network(&g); break;
            case 3:  do_add_user(&g); break;
            case 4:  do_add_friendship(&g); break;
            case 5:  do_remove_friendship(&g); break;
            case 6:  do_remove_user(&g); break;
            case 7:  do_shortest_path(&g); break;
            case 8:  do_recommend(&g); break;
            case 9:  do_count_communities(&g); break;
            case 10: do_save_network(&g); break;
            case 0:
                printf(COLOR_CYAN "\nGoodbye!\n" COLOR_RESET);
                running = 0;
                break;
            case -1: /* EOF on stdin */
                running = 0;
                break;
            default:
                printf(COLOR_RED "\nInvalid choice, please try again.\n" COLOR_RESET);
                break;
        }

        if (running) {
            press_enter();
        }
    }

    free_graph(&g);
    return 0;
}

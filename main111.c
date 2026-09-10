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

/* Read one integer, re-prompting on bad input.
 * Returns 0 on success (value written to *out), -1 on EOF. */
static int read_int(const char *prompt, int *out)
{
    char buf[64];
    while (1) {
        if (read_line(buf, sizeof(buf), prompt) < 0) return -1;
        if (sscanf(buf, "%d", out) == 1) return 0;
        printf(COLOR_RED "Invalid number, please try again.\n" COLOR_RESET);
    }
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

static void do_add_friendship(Graph *g)
{
    int idx1, idx2;
    if (read_int("Enter first user index: ", &idx1) < 0) return;
    if (read_int("Enter second user index: ", &idx2) < 0) return;

    if (add_friendship(g, idx1, idx2) > 0) {
        printf(COLOR_GREEN "\nFriendship added between %d and %d.\n" COLOR_RESET, idx1, idx2);
    } else {
        printf(COLOR_RED "\nCould not add friendship between %d and %d.\n" COLOR_RESET, idx1, idx2);
    }
}

static void do_remove_friendship(Graph *g)
{
    int idx1, idx2;
    if (read_int("Enter first user index: ", &idx1) < 0) return;
    if (read_int("Enter second user index: ", &idx2) < 0) return;

    if (remove_friendship(g, idx1, idx2) > 0) {
        printf(COLOR_GREEN "\nFriendship removed between %d and %d.\n" COLOR_RESET, idx1, idx2);
    } else {
        printf(COLOR_RED "\nCould not remove friendship between %d and %d.\n" COLOR_RESET, idx1, idx2);
    }
}

static void do_remove_user(Graph *g)
{
    int idx;
    if (read_int("Enter user index to remove: ", &idx) < 0) return;

    if (remove_user(g, idx) > 0) {
        printf(COLOR_GREEN "\nUser %d removed.\n" COLOR_RESET, idx);
    } else {
        printf(COLOR_RED "\nCould not remove user %d.\n" COLOR_RESET, idx);
    }
}

static void not_implemented(const char *feature)
{
    printf(COLOR_YELLOW "\n'%s' is not implemented yet.\n" COLOR_RESET, feature);
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
        printf(CLEAR_SCREEN);
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
            case 7:  not_implemented("Shortest path"); break;
            case 8:  not_implemented("Recommend friends"); break;
            case 9:  not_implemented("Count communities"); break;
            case 10: not_implemented("Save network"); break;
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

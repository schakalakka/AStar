// astar.c
// file for the main AStar algorithm


#include "astar.h"

long get_node_by_id(node *nodes, unsigned long n, unsigned long id) {
    // returns the index of a node in the array for a given id
    // uses binary search
    // returns -1 if node is not found

    unsigned long first = 0;
    unsigned long last = n - 1;
    unsigned long middle = (first + last) / 2;

    while (first <= last) {
        if (nodes[middle].id < id)
            first = middle + 1;
        else if (nodes[middle].id == id)
            return middle;
        else
            last = middle - 1;
        middle = (first + last) / 2;
    }
    return -1;
}



int main(int argc, char *argv[]) {
    char *filename;
    strcpy(filename, argv[1]);
    int nr_of_nodes;
    node *nodes = NULL;
    nr_of_nodes = read_csv_file(filename, &nodes);
    printf("%i\n", nr_of_nodes);
}
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


void astar(node **nodes, int nr_of_nodes) {

}


int main(int argc, char *argv[]) {
    // depending on how the input file (only parameter) is named it will read a file and run the astar algorithm
    //
    // if file is named *.csv it will read the csv file and construct the graph (i.e. node list)
    // and then write this information to a binary file for faster reading at a later stage
    //
    // if the file is named *.bin it will read the binary, construct the graph and run the A* algorithm
    //
    // usage:   ./astar /path/to/my/file.csv  OR
    //          ./astar /path/to/my/file.bin

    char *filename;
    bool binary = false;

    strcpy(filename, argv[1]);
    if (strcmp(strrchr(filename, '.'), ".bin")==0) {
        binary=true;
    }

    int nr_of_nodes;
    node *nodes = NULL;

    if (binary==true){
       nr_of_nodes = read_binary_file(filename, &nodes, nr_of_nodes);
    }
    else{
        nr_of_nodes = read_csv_file(filename, &nodes);
    }
    printf("%i\n", nr_of_nodes);
}
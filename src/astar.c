// astar.c
// file for the main AStar algorithm


#include "astar.h"

unsigned long get_node_by_id(node *nodes, unsigned long n, unsigned long id) {
    // returns the index of a node in the array for a given id
    // uses binary search
    // returns ULONG_MAX (biggest possible unsigned long) if node is not found
    //
    // why does ULONG_MAX work as a "not found" return value?
    // the number of nodes n is an unsigned long, let's assume n=ULONG_MAX
    // then we have the indices 0..ULONG_MAX-1
    // -> ULONG_MAX can never be an index and means therefore it is not found
    // Note (by such a number of nodes our RAM probably would have killed us anyway :D)

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
    return ULONG_MAX;
}


double get_weight(unsigned node_a_index,unsigned long node_b_index, node * nodes, unsigned long nr_of_nodes){
    // returns the weight between two neighbouring nodes
    // the weight is computed via the euclidean distance
    // given are two indices (not IDs) of nodes in the nodes list, the nodes list itself and the length of the list.
    // if the nodes are not neighboured the return value is -1

    //check if nodes are adjacent
    for (int i = 0; i < nodes[node_a_index].nsucc; ++i) {
        if (nodes[node_a_index].successors[i] == node_b_index) {
            return -1.0;
        }
    }

    double lat_a = nodes[node_a_index].lat;
    double lon_a = nodes[node_a_index].lon;
    double lat_b = nodes[node_b_index].lat;
    double lon_b = nodes[node_b_index].lon;

    double weight;

    weight = sqrt((lat_a-lat_b)*(lat_a-lat_b) + (lon_a-lon_b)*(lon_a-lon_b));

    return weight;
}


double heuristic_distance(unsigned node_a_index,unsigned long node_b_index, node * nodes, unsigned long nr_of_nodes){
    // returns the heuristic distance
    // i.e. the direct shortest distance on the air surface
    // given are two indices (not IDs) of nodes in the nodes list, the nodes list itself and the length of the list.
    // not clear yet which computation method to use
    // look here http://www.movable-type.co.uk/scripts/latlong.html

    // if the indices are bigger or equal than the length of the node list, exit with error code 1
    if (nr_of_nodes<= node_a_index && nr_of_nodes<= node_b_index) exit(1);

    double lat_a = nodes[node_a_index].lat;
    double lon_a = nodes[node_a_index].lon;
    double lat_b = nodes[node_b_index].lat;
    double lon_b = nodes[node_b_index].lon;

    double distance;

    // computation for heuristic here


    return distance;
}



void astar(unsigned long node_start, unsigned long node_goal, node **nodes, unsigned long nr_of_nodes) {
    //
    // node_start is the source node id
    // node_goal is the goal node id
    // the indices in the nodes list have to be obtained by get_node_by_id
    // nr_of_nodes states the length of the nodes list
    unsigned long source_index = get_node_by_id(*nodes, nr_of_nodes, node_start);
    unsigned long goal_index = get_node_by_id(*nodes, nr_of_nodes, node_goal);
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
    if (strcmp(strrchr(filename, '.'), ".bin") == 0) {
        binary = true;
    }

    unsigned long nr_of_nodes;
    node *nodes;

    if (binary == true) {
        nr_of_nodes = read_binary_file(filename, &nodes);
    } else {
        nr_of_nodes = read_csv_file(filename, &nodes);
    }
    printf("%lu\n", nr_of_nodes);
}
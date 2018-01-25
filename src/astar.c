// astar.c
// file for the main AStar algorithm


#include "astar.h"

list_elem *add_element_to_list(unsigned long index_to_add, list_elem *start_of_list, AStarStatus *astar_status_list) {
    // adds an element to a list
    // the list is always sorted in ascending order
    // the sorting key is the fscore computed from the AStarStatus
    //
    // returned is the beginning of the list (by a pointer)
    // i.e. either the new element if it is the lowest or
    // the start from before

    list_elem *next_elem = start_of_list;
    list_elem *current_elem = NULL;
    list_elem *new_elem = NULL;

    // check if element is actually the lowest
    if (get_fscore(astar_status_list[index_to_add]) <= get_fscore(astar_status_list[next_elem->index])) {
        // add element to the beginning of the list
        // and return new element as the start of the list
        *new_elem = (list_elem){.index=index_to_add, .next=start_of_list};
        return new_elem;
    }

    // go through the list
    while (next_elem->next != NULL) {
        current_elem = next_elem;
        next_elem = next_elem->next;
        if (get_fscore(astar_status_list[index_to_add]) <= get_fscore(astar_status_list[next_elem->index])) {
            // insert the element to the list between current_elem and next_elem
            // and return the start of the list (it did'nt change)
            *new_elem = (list_elem){.index=index_to_add, .next=next_elem};
            current_elem->next = new_elem;
            return start_of_list;
        }
    }
    // we land here if we reached the end
    // i.e. the element has the worst fscore
    *new_elem = (list_elem){.index=index_to_add, .next=NULL};
    next_elem->next = new_elem;
    return start_of_list;
}

list_elem *remove_element_from_list(unsigned long index_to_remove, list_elem *start_of_list) {
    // removes an element from a list
    //
    // returns a pointer to the start of the list

    list_elem *current_elem = start_of_list;
    list_elem *next_elem = NULL;

    // check if first element is the wanted one
    if (current_elem->index == index_to_remove) {
        start_of_list = current_elem->next;
        free(current_elem);
        return start_of_list;
    }

    // go through the list
    next_elem = current_elem->next;
    while (next_elem != NULL) {
        if (next_elem->index == index_to_remove) {
            current_elem->next = next_elem->next;
            free(next_elem);
            return start_of_list;
        }
        current_elem = next_elem;
        next_elem = current_elem->next;
    }

    // if we are here we could not find the element in the list
    // not sure how to handle this so far
    // for now we throw an error and exit TODO
    exit(1);
}

double get_fscore(AStarStatus astarstatus) {
    // returns the fscore from a node, in particular from an AStarStatus
    return astarstatus.g + astarstatus.h;
}

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


double get_weight(unsigned node_a_index, unsigned long node_b_index, node *nodes) {
    // returns the weight between two neighbouring nodes
    // the weight is computed via the euclidean distance
    // given are two indices (not IDs) of nodes in the nodes list, the nodes list itself and the length of the list.
    // if the nodes are not adjacent the return value is -1

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

    weight = sqrt((lat_a - lat_b) * (lat_a - lat_b) + (lon_a - lon_b) * (lon_a - lon_b));

    return weight;
}


double
heuristic_distance(unsigned long node_a_index, unsigned long node_b_index, node *nodes, unsigned long nr_of_nodes) {
    // returns the heuristic distance
    // i.e. the direct shortest distance on the air surface
    // given are two indices (not IDs) of nodes in the nodes list, the nodes list itself and the length of the list.
    // not clear yet which computation method to use
    // look here http://www.movable-type.co.uk/scripts/latlong.html

    // if the indices are bigger or equal than the length of the node list, exit with error code 1
    if (nr_of_nodes <= node_a_index && nr_of_nodes <= node_b_index) exit(1);

    double lat_a = nodes[node_a_index].lat;
    double lon_a = nodes[node_a_index].lon;
    double lat_b = nodes[node_b_index].lat;
    double lon_b = nodes[node_b_index].lon;

    double distance = 0;

    // computation for heuristic here


    return distance;
}


void astar(unsigned long node_start, unsigned long node_goal, node *nodes, unsigned long nr_of_nodes) {
    //
    // node_start is the source node id
    // node_goal is the goal node id
    // the indices in the nodes list have to be obtained by get_node_by_id
    // nr_of_nodes states the length of the nodes list
    unsigned long source_index = get_node_by_id(nodes, nr_of_nodes, node_start);
    unsigned long goal_index = get_node_by_id(nodes, nr_of_nodes, node_goal);

    AStarStatus *status_list = malloc(nr_of_nodes * sizeof(AStarStatus));

    status_list[source_index].g = 0;
    status_list[source_index].h = heuristic_distance(source_index, goal_index, nodes, nr_of_nodes);
    status_list[source_index].parent = ULONG_MAX;
    status_list[source_index].whq = OPEN;


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
        astar(8670491, 8670492, nodes, nr_of_nodes);
    } else {
        nr_of_nodes = read_csv_file(filename, &nodes);
    }
    printf("%lu\n", nr_of_nodes);
}
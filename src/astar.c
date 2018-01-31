// astar.c
// file for the main AStar algorithm


#include "astar.h"

void add_element_to_list(unsigned long index_to_add, list_elem **start_of_list, AStarStatus *astar_status_list) {
    // adds an element to a list
    // the list is always sorted in ascending order
    // the sorting key is the fscore computed from the AStarStatus
    //
    // returned is the beginning of the list (by a pointer)
    // i.e. either the new element if it is the lowest or
    // the start from before

    list_elem *next_elem = NULL;
    list_elem *current_elem = NULL;
    list_elem *new_elem = NULL;

    double fscore_of_index_to_add = get_fscore(astar_status_list[index_to_add]);

    // we have to allocate memory to make it count across the borders of this method
    new_elem = malloc(sizeof(list_elem));

    next_elem = *start_of_list;


    // check if element is actually the lowest
    if ((next_elem == NULL) ||
        (fscore_of_index_to_add <= get_fscore(astar_status_list[next_elem->index]))) {
        // add element to the beginning of the list
        // and return new element as the start of the list
        new_elem->index = index_to_add;
        new_elem->next = next_elem;
        *start_of_list = new_elem;
        return;
    }

    // go through the list
    while (next_elem->next != NULL) {
        current_elem = next_elem;
        next_elem = next_elem->next;
        if (fscore_of_index_to_add <= get_fscore(astar_status_list[next_elem->index])) {
            // insert the element to the list between current_elem and next_elem
            // and return the start of the list (it did'nt change)
            new_elem->index = index_to_add;
            new_elem->next = next_elem;
            current_elem->next = new_elem;
            return;
        }
    }
    // we land here if we reached the end
    // i.e. the element has the worst fscore
    new_elem->index = index_to_add;
    new_elem->next = NULL;
    next_elem->next = new_elem;
}

void remove_element_from_list(unsigned long index_to_remove, list_elem **start_of_list) {
    // removes an element from a list
    //
    // returns a pointer to the start of the list

    list_elem *current_elem = *start_of_list;
    list_elem *next_elem = NULL;

    // check if first element is the wanted one
    if (current_elem->index == index_to_remove) {
        *start_of_list = current_elem->next;
        free(current_elem);
        return;
    }

    // go through the list
    next_elem = current_elem->next;
    while (next_elem != NULL) {
        if (next_elem->index == index_to_remove) {
            current_elem->next = next_elem->next;
            free(next_elem);
            return;
        }
        current_elem = next_elem;
        next_elem = current_elem->next;
    }

    // if we are here we could not find the element in the list
    // not sure how to handle this so far
    // for now we throw an error and exit TODO
    exit(99);
}

double get_fscore(AStarStatus astarstatus) {
    // returns the fscore from a node, in particular from an AStarStatus
    // convenience function
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


double get_weight(unsigned long node_a_index, unsigned long node_b_index, node *nodes) {
    // returns the Equirectangular approximation distance between two neighbouring nodes
    // this approximation is less accurate than the heuristic 'haversine' distance but we use this only for really close nodes
    // advantage: it is faster than the heuristic distance
    // given are two indices (not IDs) of nodes in the nodes list, the nodes list itself and the length of the list.
    // if the nodes are not adjacent the return value is -1

    double lat_a = nodes[node_a_index].lat * M_PI / 180.0;;
    double lon_a = nodes[node_a_index].lon * M_PI / 180.0;;
    double lat_b = nodes[node_b_index].lat * M_PI / 180.0;;
    double lon_b = nodes[node_b_index].lon * M_PI / 180.0;;
    double diff_lat = lat_a - lat_b;
    double diff_lon = lon_a - lon_b;

    double weight;

    double x = diff_lon * cos((lat_a + lat_b) * 0.5);
    weight = R * sqrt(x * x + diff_lat * diff_lat);

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

    double lat_a = nodes[node_a_index].lat * M_PI / 180.0;
    double lon_a = nodes[node_a_index].lon * M_PI / 180.0;
    double lat_b = nodes[node_b_index].lat * M_PI / 180.0;
    double lon_b = nodes[node_b_index].lon * M_PI / 180.0;
    double diff_lat = lat_a - lat_b;
    double diff_lon = lon_a - lon_b;
//    double R = 6371000; // earth's radius

    double a = sin(diff_lat / 2) * sin(diff_lat / 2) + cos(lat_a) * cos(lat_b) * sin(diff_lon / 2) * sin(diff_lon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double distance = R * c;

    return distance;
}


void astar(unsigned long node_start, unsigned long node_goal, node *nodes, unsigned long nr_of_nodes) {
    //
    // node_start is the source node id
    // node_goal is the goal node id
    // the indices in the nodes list have to be obtained by get_node_by_id
    // nr_of_nodes states the length of the nodes list

    unsigned long start_index = get_node_by_id(nodes, nr_of_nodes, node_start);
    unsigned long goal_index = get_node_by_id(nodes, nr_of_nodes, node_goal);

    AStarStatus *status_list = malloc(nr_of_nodes * sizeof(AStarStatus));
    list_elem *OPEN_LIST = NULL;
    // we do not have to store a linked list for the closed nodes
    // we can get this information from the AStarStatus/status_list
    list_elem *current_element = NULL;

    unsigned long node_successor_index;
    double successor_current_cost;

    // put node_start (i.e. start_index) in open list with fscore = hscore
    status_list[start_index].g = 0;
    status_list[start_index].h = heuristic_distance(start_index, goal_index, nodes, nr_of_nodes);
    status_list[start_index].parent = ULONG_MAX; //the parent is set to ULONG_MAX because the start node has no parent
    status_list[start_index].whq = OPEN;
    add_element_to_list(start_index, &OPEN_LIST, status_list);

    // while open list is not empty
    while (OPEN_LIST != NULL) {
        // find minimal node
        current_element = OPEN_LIST;
        double current_fscore = get_fscore(status_list[current_element->index]);
        unsigned long current_index = current_element->index;
        while (current_element->next != NULL) {
            current_element = current_element->next;
            if (current_fscore > get_fscore(status_list[current_element->index])) {
                current_fscore = get_fscore(status_list[current_element->index]);
                current_index = current_element->index;
            }
        }

        if (current_index == goal_index) {
            printf("Solution found. With length of %f.\n", status_list[current_index].g); //TODO
            return;
        }

        // generate for each neighbour of current_element the AStar state
        for (int i = 0; i < nodes[current_index].nsucc; ++i) {
            node_successor_index = nodes[current_index].successors[i];
            successor_current_cost =
                    status_list[current_index].g + get_weight(current_index, node_successor_index, nodes);
            if (status_list[node_successor_index].whq == OPEN) {
                if (status_list[node_successor_index].g <= successor_current_cost) continue;

            } else if (status_list[node_successor_index].whq == CLOSED) {
                if (status_list[node_successor_index].g <= successor_current_cost) continue;
                add_element_to_list(node_successor_index, &OPEN_LIST, status_list);
                status_list[node_successor_index].whq = OPEN;
                printf("Reopen node %lu\n", node_successor_index);
            } else {
                add_element_to_list(node_successor_index, &OPEN_LIST, status_list);
                status_list[node_successor_index].whq = OPEN;
                status_list[node_successor_index].h = heuristic_distance(node_successor_index, goal_index, nodes,
                                                                         nr_of_nodes);
            }
            status_list[node_successor_index].g = successor_current_cost;
            status_list[node_successor_index].parent = current_index;
        }
        // set current_element to closed and remove it from open list
        remove_element_from_list(current_index, &OPEN_LIST);
        status_list[current_index].whq = CLOSED;
    }

    if (goal_index != current_element->index) {
        printf("No solution found. The OPEN_LIST is empty.\n");
        exit(1);
    }
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
//        astar(8670491, 30307973, nodes, nr_of_nodes);
        astar(240949599, 195977239, nodes, nr_of_nodes);
    } else {
        nr_of_nodes = read_csv_file(filename, &nodes);
    }
    printf("Number of nodes: %lu\n", nr_of_nodes);
}
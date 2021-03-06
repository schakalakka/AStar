// astar.c
// file for the main AStar algorithm


#include "astar.h"

void add_element_to_list(unsigned long index_to_add, list_elem** start_of_list, AStarStatus* astar_status_list)
{
    // adds an element to a list
    // the list is always sorted in ascending order
    // the sorting key is the fscore computed from the AStarStatus
    //
    // returned is the beginning of the list (by a pointer)
    // i.e. either the new element if it is the lowest or
    // the start from before

    list_elem* next_elem = NULL;
    list_elem* current_elem = NULL;
    list_elem* new_elem = NULL;

    double fscore_of_index_to_add = get_fscore(astar_status_list[index_to_add]);

    // we have to allocate memory to make it count across the borders of this method
    new_elem = malloc(sizeof(list_elem));

    next_elem = *start_of_list;


    // check if element is actually the lowest
    if ((next_elem==NULL) ||
            (fscore_of_index_to_add<=get_fscore(astar_status_list[next_elem->index]))) {
        // add element to the beginning of the list
        // and return new element as the start of the list
        new_elem->index = index_to_add;
        new_elem->next = next_elem;
        *start_of_list = new_elem;
        return;
    }

    // go through the list
    while (next_elem->next!=NULL) {
        current_elem = next_elem;
        next_elem = next_elem->next;
        if (fscore_of_index_to_add<=get_fscore(astar_status_list[next_elem->index])) {
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

void remove_element_from_list(unsigned long index_to_remove, list_elem** start_of_list)
{
    // removes an element from a list
    //
    // returns a pointer to the start of the list

    list_elem* current_elem = *start_of_list;
    list_elem* next_elem = NULL;

    // check if first element is the wanted one
    if (current_elem->index==index_to_remove) {
        *start_of_list = current_elem->next;
        free(current_elem);
        return;
    }

    // go through the list
    next_elem = current_elem->next;
    while (next_elem!=NULL) {
        if (next_elem->index==index_to_remove) {
            current_elem->next = next_elem->next;
            free(next_elem);
            return;
        }
        current_elem = next_elem;
        next_elem = current_elem->next;
    }

    // if we are here we could not find the element in the list
    // not sure how to handle this so far
    // for now we throw an error and exit
    exit(41);
}

double get_fscore(AStarStatus astarstatus)
{
    // returns the fscore from a node, in particular from an AStarStatus
    // convenience function
    return astarstatus.g+astarstatus.h;
}

unsigned long get_node_by_id(node* nodes, unsigned long n, unsigned long id)
{
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
    unsigned long last = n-1;
    unsigned long middle = (first+last)/2;

    while (first<=last) {
        if (nodes[middle].id<id)
            first = middle+1;
        else if (nodes[middle].id==id)
            return middle;
        else
            last = middle-1;
        middle = (first+last)/2;
    }
    return ULONG_MAX;
}

double equirectangular_distance(unsigned long node_a_index, unsigned long node_b_index, node* nodes)
{
    // returns the Equirectangular approximation distance between two neighbouring nodes
    // this approximation is less accurate than the heuristic 'haversine' distance but we use this only for really close nodes
    // advantage: it is faster than the heuristic distance
    // given are two indices (not IDs) of nodes in the nodes list, the nodes list itself and the length of the list.
    // if the nodes are not adjacent the return value is -1

    double lat_a = nodes[node_a_index].lat*M_PI/180.0;;
    double lon_a = nodes[node_a_index].lon*M_PI/180.0;;
    double lat_b = nodes[node_b_index].lat*M_PI/180.0;;
    double lon_b = nodes[node_b_index].lon*M_PI/180.0;;
    double diff_lat = lat_a-lat_b;
    double diff_lon = lon_a-lon_b;

    double weight;

    double x = diff_lon*cos((lat_a+lat_b)*0.5);
    weight = R*sqrt(x*x+diff_lat*diff_lat);

    return weight;
}

double haversine_distance(unsigned long node_a_index, unsigned long node_b_index, node* nodes)
{
    // returns the haversine distance
    // given are two indices (not IDs) of nodes in the nodes list, the nodes list itself

    double lat_a = nodes[node_a_index].lat*M_PI/180.0;
    double lon_a = nodes[node_a_index].lon*M_PI/180.0;
    double lat_b = nodes[node_b_index].lat*M_PI/180.0;
    double lon_b = nodes[node_b_index].lon*M_PI/180.0;
    double diff_lat = lat_a-lat_b;
    double diff_lon = lon_a-lon_b;

    double a = sin(diff_lat/2)*sin(diff_lat/2)+cos(lat_a)*cos(lat_b)*sin(diff_lon/2)*sin(diff_lon/2);
    double c = 2*atan2(sqrt(a), sqrt(1-a));
    double distance = R*c;

    return distance;
}

double
heuristic_distance(unsigned long node_a_index, unsigned long node_b_index, node* nodes, Heuristic distance_method)
{
    // returns the heuristic distance
    // i.e. the direct shortest distance on the air surface
    // given are two indices (not IDs) of nodes in the nodes list, the nodes list itself and the method to use for the
    // computation
    // possible options: HAVERSINE or EQUIRECTANGULAR
    // for more information look here http://www.movable-type.co.uk/scripts/latlong.html
    if (distance_method==HAVERSINE) {
        return haversine_distance(node_a_index, node_b_index, nodes);
    }
    else if (distance_method==EQUIRECTANGULAR) {
        return equirectangular_distance(node_a_index, node_b_index, nodes);
    }
    exit(51); // throw error if no correct distance_method is set
}

void write_solution_to_file(char* filename, unsigned long node_goal_index, node* nodes, AStarStatus* status_list)
{
    // if an optimal solution is found this function is called
    // and will write the path from destination to source (so in reverse order!) into a file like spain.out

    FILE* fout;
    strcpy(strrchr(filename, '.'), ".out");

    if ((fout = fopen(filename, "w"))==NULL) exit(31);

    fprintf(fout, "Optimal Path found: \n");

    unsigned long current_index = node_goal_index;
    while (current_index!=ULONG_MAX) {
        fprintf(fout, "Node id:\t %lu\t| Distance:\t%.2f\n", nodes[current_index].id, status_list[current_index].g);
        current_index = status_list[current_index].parent;
    }
    fclose(fout);
    printf("Optimal Path is written to %s", filename);

}

void astar(unsigned long node_start, unsigned long node_goal, node* nodes, unsigned long nr_of_nodes,
        Heuristic distance_method, char* filename)
{
    // node_start is the source node id
    // node_goal is the goal node id
    // the indices in the nodes list have to be obtained by get_node_by_id
    // nr_of_nodes states the length of the nodes list
    // the distance_method is either HAVERSINE (more accurate) or EQUIRECTANGULAR (faster) for the distance computation
    // filename is used to pass the parameter to write_solution_to_file function for the output solution file

    unsigned long start_index = get_node_by_id(nodes, nr_of_nodes, node_start);
    unsigned long goal_index = get_node_by_id(nodes, nr_of_nodes, node_goal);

    AStarStatus* status_list = malloc(nr_of_nodes*sizeof(AStarStatus));
    list_elem* OPEN_LIST = NULL;
    // we do not have to store a linked list for the closed nodes
    // we can get this information from the AStarStatus/status_list
    list_elem* current_element = NULL;
    unsigned long current_index;

    unsigned long node_successor_index;
    double successor_current_cost;

    // put node_start (i.e. start_index) in open list with fscore = hscore
    status_list[start_index].g = 0;
    status_list[start_index].h = heuristic_distance(start_index, goal_index, nodes, distance_method);
    status_list[start_index].parent = ULONG_MAX; //the parent is set to ULONG_MAX because the start node has no parent
    status_list[start_index].whq = OPEN;
    add_element_to_list(start_index, &OPEN_LIST, status_list);

    // while open list is not empty
    while (OPEN_LIST!=NULL) {
        // get minimal node
        current_element = OPEN_LIST;
        current_index = current_element->index;

        if (current_index==goal_index) {
            printf("Solution found. With length of %f.\n", status_list[current_index].g);
            write_solution_to_file(filename, goal_index, nodes, status_list);
            return;
        }

        // generate for each neighbour of current_element the AStar state
        for (int i = 0; i<nodes[current_index].nsucc; ++i) {
            node_successor_index = nodes[current_index].successors[i];
            successor_current_cost =
                    status_list[current_index].g+
                            heuristic_distance(current_index, node_successor_index, nodes, distance_method);
            if (status_list[node_successor_index].whq==OPEN) {
                if (status_list[node_successor_index].g<=successor_current_cost) continue;
                else {
                    status_list[node_successor_index].g = successor_current_cost;
                    status_list[node_successor_index].parent = current_index;
                    // we have to remove and add the element again because the distances were updated
                    // and we want to keep a sorted list
                    remove_element_from_list(node_successor_index, &OPEN_LIST);
                    add_element_to_list(node_successor_index, &OPEN_LIST, status_list);
                }
            }
            else if (status_list[node_successor_index].whq==CLOSED) {
                if (status_list[node_successor_index].g<=successor_current_cost) continue;
                status_list[node_successor_index].g = successor_current_cost;
                status_list[node_successor_index].parent = current_index;
                add_element_to_list(node_successor_index, &OPEN_LIST, status_list);
                status_list[node_successor_index].whq = OPEN;
            }
            else {
                status_list[node_successor_index].g = successor_current_cost;
                status_list[node_successor_index].parent = current_index;
                status_list[node_successor_index].whq = OPEN;
                status_list[node_successor_index].h = heuristic_distance(node_successor_index, goal_index, nodes,
                        distance_method);
                add_element_to_list(node_successor_index, &OPEN_LIST, status_list);
            }
        }
        // set current_element to closed and remove it from open list
        remove_element_from_list(current_index, &OPEN_LIST);
        status_list[current_index].whq = CLOSED;
    }

    // if we reach this we did not find a solution
    printf("No solution found. The OPEN_LIST is empty.\n");
    exit(11);
}

int main(int argc, char* argv[])
{
    // depending on how the input file (only parameter) is named it will read a file and run the astar algorithm
    //
    // if file is named *.csv it will read the csv file and construct the graph (i.e. node list)
    // and then write this information to a binary file for faster reading at a later stage
    //
    // if the file is named *.bin it will read the binary, construct the graph and run the A* algorithm
    //
    // usage:   ./astar /path/to/my/file.csv  OR
    //          ./astar /path/to/my/file.bin

    char filename[100];
    bool binary = false; // switch for reading a .csv or a .bin file, depends on the line ending

    Heuristic distance_method = HAVERSINE; //possible options HAVERSINE or EQUIRECTANGULAR, change here if wanted
    unsigned long nr_of_nodes;
    node* nodes;
    unsigned long node_start = 240949599; //default start node id for the spain.csv
    unsigned long node_goal = 195977239; //default end node id for the spain.csv

    //parse command line arguments
    if (argc<=1) {
        printf("Please specify at least a .csv for parsing or a .bin for computing a route.\nUsage: ./astar spain.csv");
        exit(1);
    }
    else if (argc>=2) {
        //set filename
        strcpy(filename, argv[1]);
        if (argc==4) {
            //set source and destination ids, ignored if a .csv file is read
            node_start = strtoul(argv[2], NULL, 10);
            node_goal = strtoul(argv[3], NULL, 10);
        }
    }


    //check if binary file or not (otherwise a csv file is assumed)
    if (strcmp(strrchr(filename, '.'), ".bin")==0) {
        binary = true;
    }

    //read either a .csv file and create a binary file
    // or read a binary file and compute a route
    if (binary==false) {
        nr_of_nodes = read_csv_file(filename, &nodes);
    }
    else {
        nr_of_nodes = read_binary_file(filename, &nodes);
        astar(node_start, node_goal, nodes, nr_of_nodes, distance_method, filename);
    }
}
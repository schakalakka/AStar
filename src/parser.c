// parser.c
// contains general .csv parsing functionality for building the graph, i.e. nodes array


#include "astar.h"




unsigned long get_nr_of_nodes(char *filename) {
    // returns number of nodes
    //assumes the first three #lines and that there are now ways or relation line between nodes
    // i.e. if it does not read a node anymore it will finish completely.

    unsigned long nr_of_nodes = 0;
    char *buffer;
    size_t characters;


    FILE *fp = fopen(filename, "r");
    buffer = (char *) malloc(characters * sizeof(char));

    //skip the first three lines
    characters = getline(&buffer, &characters, fp);
    characters = getline(&buffer, &characters, fp);
    characters = getline(&buffer, &characters, fp);


    // count number of nodes for array initialization
    while ((characters = getline(&buffer, &characters, fp)) > 0) {
        if (buffer[0] == 'n') {
            nr_of_nodes++;
        } else {
            // break when not reading any more nodes
            break;
        }
    }
    fclose(fp);
    return nr_of_nodes;
}

void get_nodes(char *filename, node **nodes, int nr_of_nodes) {
    // returns the complete nodes array with all nodes and edges
    // first reads all the node lines
    // the array will be initialized by a length of nr_of_nodes which is computed (read) before
    // secondly it calls get_edges to build the graph structure, i.e. edges
    const char delimiters[] = "|";
    char *buffer;
    size_t characters;
    FILE *fp = fopen(filename, "r");

    *nodes = malloc(nr_of_nodes * sizeof(node));
    buffer = (char *) malloc(characters * sizeof(char));

    //skip the first three lines of the file because they start with #
    characters = getline(&buffer, &characters, fp);
    characters = getline(&buffer, &characters, fp);
    characters = getline(&buffer, &characters, fp);

    unsigned long id;
    double lon;
    double lat;

    // count number of nodes for array initialization
    int counter = 0;

    while ((characters = getline(&buffer, &characters, fp)) > 0) {
        if (buffer[0] == 'n') {
            strsep(&buffer, delimiters);
            id = strtoul(strsep(&buffer, delimiters), NULL, 0);
            strsep(&buffer, delimiters);
            strsep(&buffer, delimiters);
            strsep(&buffer, delimiters);
            strsep(&buffer, delimiters);
            strsep(&buffer, delimiters);
            strsep(&buffer, delimiters);
            strsep(&buffer, delimiters);
            lat = strtod(strsep(&buffer, delimiters), NULL);
            lon = strtod(strsep(&buffer, delimiters), NULL);
            node current_node = {.id=id, .lat=lat, .lon=lon};
            (*nodes)[counter] = current_node;
            counter++;
        } else if (buffer[0] == 'w') {
            get_edges(fp, buffer, delimiters, nodes, nr_of_nodes);
        } else {
            // break when not reading any more nodes or ways
            break;
        }
    }
    fclose(fp);
}


unsigned long get_next_edge_node(char **buffer, const char *delimiters) {
    // helper function to return the id while reading the member nodes
    // returns 0 if there are no more member nodes to read, i.e. end of line
    // we assume that there is no nodes with id 0, which is the case for cataluna.csv and spain.csv
    char *text = strsep(buffer, delimiters);
    if (text == NULL) {
        return 0;
    } else {
        return strtoul(text, NULL, 0);
    }
}


void get_edges(FILE *fp, char *buffer, const char *delimiters, node **nodes, int nr_of_nodes) {
    // method for adding the edges to the nodes array
    // gets called within the get_nodes function after reading all the nodes

    bool oneway;
    unsigned long head_id;
    unsigned long tail_id;
    long tail_index;
    long head_index;
    size_t characters;



    while (buffer[0] == 'w') {
        for (int i = 0; i < 7; ++i) {
            strsep(&buffer, delimiters); // skip the first 7 useless entries
        }
        if (strcmp(strsep(&buffer, delimiters), "oneway")) {
            oneway = false;
        } else {
            oneway = true;
        }
        strsep(&buffer, delimiters); // skip maxspeed

        // now the member nodes
        // we assume that there is no node with id=0 which is the case for cataluna and spain
        // this is important because strtoul returns 0 if the input is not a valid unsigned long
        // which would be ambiguous in the caseof a node with id=0
        tail_id = strtoul(strsep(&buffer, delimiters), NULL, 0);
        head_id = strtoul(strsep(&buffer, delimiters), NULL, 0);
        while (tail_id != 0 && head_id != 0) { // while not reached end of line
            tail_index = get_node_by_id(*nodes, nr_of_nodes, tail_id);
            head_index = get_node_by_id(*nodes, nr_of_nodes, head_id);
            if (tail_index != -1 && head_index != -1) {
                add_edge(nodes, tail_index, head_index);
                if (oneway == true)
                    add_edge(nodes, head_index, tail_index);
                tail_id = head_id;
                head_id = get_next_edge_node(&buffer, delimiters);
            } else if (tail_index != -1 && head_index == -1) {
                tail_id = get_next_edge_node(&buffer, delimiters);
                head_id = get_next_edge_node(&buffer, delimiters);
            } else if (tail_index == -1 && head_index != -1) {
                tail_id = head_id;
                head_id = get_next_edge_node(&buffer, delimiters);
            } else {
                tail_id = get_next_edge_node(&buffer, delimiters);
                head_id = get_next_edge_node(&buffer, delimiters);
            }
        }
        characters = getline(&buffer, &characters, fp);
    }
}


int read_csv_file(char *filename, node **nodes) {

    int nr_of_nodes = 0;
    //count number of nodes for a proper array initialization no reallocs
    // not sure what is faster here: read node lines twice and malloc once
    // or read once and realloc if necessary
    nr_of_nodes = get_nr_of_nodes(filename);
    get_nodes(filename, nodes, nr_of_nodes);

    return nr_of_nodes;
}


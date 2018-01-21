// parser.c
// contains general .csv parsing functionality for building the graph, i.e. nodes array


#include "astar.h"


unsigned long get_nr_of_nodes(char *filename) {
    // returns number of nodes
    // assumes the first three #lines and that there are now ways or relation line between nodes
    // i.e. if it does not read a node anymore it will finish completely.

    unsigned long nr_of_nodes = 0;
    char *buffer;
    size_t characters = 0;


    FILE *fp = fopen(filename, "r");
    buffer = (char *) malloc(characters * sizeof(char));

    //skip the first three lines
    characters = (size_t) getline(&buffer, &characters, fp);
    characters = (size_t) getline(&buffer, &characters, fp);
    characters = (size_t) getline(&buffer, &characters, fp);


    // count number of nodes for array initialization
    while ((characters = (size_t) getline(&buffer, &characters, fp)) > 0) {
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

void get_nodes(char *filename, node **nodes, unsigned long nr_of_nodes) {
    // returns the complete nodes array with all nodes and edges
    // first reads all the node lines
    // the array will be initialized by a length of nr_of_nodes which is computed (read) before
    // secondly it will read the 'way' lines
    // as first step it will only parse how many neighbours each node has
    // this is done in get_edges with the parameters nsucc_only=true
    // in a later step get_edges will be called with nsucc_only=false and a current neighbour number list
    // and then build the real edges
    const char delimiters[] = "|";
    char *buffer;
    size_t characters = 0;
    FILE *fp = fopen(filename, "r");

    *nodes = malloc(nr_of_nodes * sizeof(node));
    buffer = (char *) malloc(characters * sizeof(char));

    //skip the first three lines of the file because they start with #
    characters = (size_t) getline(&buffer, &characters, fp);
    characters = (size_t) getline(&buffer, &characters, fp);
    characters = (size_t) getline(&buffer, &characters, fp);

    unsigned long id;
    double lon;
    double lat;

    // counter to get the current position in the nodes array
    int counter = 0;

    while ((characters = (size_t) getline(&buffer, &characters, fp)) > 0) {
        if (buffer[0] == 'n') {
            // we 'kind of' skip the strsep without an assignment
            strsep(&buffer, delimiters); //this element says 'node'
            id = strtoul(strsep(&buffer, delimiters), NULL, 0);
            strsep(&buffer, delimiters); //this element says 'name'
            strsep(&buffer, delimiters); //this element says 'place'
            strsep(&buffer, delimiters); //this element says 'highway'
            strsep(&buffer, delimiters); //this element says 'route'
            strsep(&buffer, delimiters); //this element says 'ref'
            strsep(&buffer, delimiters); //this element says 'oneway'
            strsep(&buffer, delimiters); //this element says 'maxspeed'
            lat = strtod(strsep(&buffer, delimiters), NULL);
            lon = strtod(strsep(&buffer, delimiters), NULL);
            node current_node = {.id=id, .lat=lat, .lon=lon};
            (*nodes)[counter] = current_node;
            counter++;
        } else if (buffer[0] == 'w') {
            // first call of get_edges with nsucc_only=true
            // this is only for the nsucc computation for each node
            // no edges added yet
            // the last '0' parameter is only placeholder, later it will be of use
            get_edges(fp, buffer, delimiters, nodes, nr_of_nodes, true, 0);
        } else {
            // break when not reading any more nodes or ways
            break;
        }
    }
    fclose(fp);

}

void build_edges(char *filename, node **nodes, unsigned long nr_of_nodes, unsigned int *current_nsucc) {
    // this is method is called after all the nodes have been read
    // and the number of neighbours of each node is known
    // this function will go through the csv file again and add the edges to the
    // adjacency list of the nodes
    // this is mainly done by calling get_edges and providing nsucc_only=false and current_nsucc
    // array of the current added neighbours in the adjacency list

    const char delimiters[] = "|";
    char *buffer;
    size_t characters = 0;
    FILE *fp = fopen(filename, "r");

    buffer = (char *) malloc(characters * sizeof(char));

    //skip the first three lines of the file because they start with #
    characters = (size_t) getline(&buffer, &characters, fp);
    characters = (size_t) getline(&buffer, &characters, fp);
    characters = (size_t) getline(&buffer, &characters, fp);


    while ((characters = (size_t) getline(&buffer, &characters, fp)) > 0) {
        if (buffer[0] == 'n') {
            continue;
        } else if (buffer[0] == 'w') {
            get_edges(fp, buffer, delimiters, nodes, nr_of_nodes, false, current_nsucc);
        } else {
            // break when not reading any more nodes or ways
            break;
        }
    }
    fclose(fp);
}


unsigned long get_next_edge_node(char **buffer, const char *delimiters) {
    // helper function to return the id while reading the member nodes in the ways
    // returns 0 if there are no more member nodes to read, i.e. end of line
    // we assume that there is no nodes with id 0, which is the case for cataluna.csv and spain.csv
    char *text = strsep(buffer, delimiters);
    if (text == NULL) {
        return 0;
    } else {
        return strtoul(text, NULL, 0);
    }
}

void add_edge(node **nodes, unsigned long tail_index, unsigned long head_index, unsigned int position) {
    // method which adds edges to the nodes list
    // adds edge tail_index -> head_index
    // tail and ead are indexes, i.e. positions in the nodes array, not ids.
    // position is used to get the current position in the adjacency list of the tail node
    *((((*nodes) + tail_index)->successors) + position) = head_index;
}

void get_edges(FILE *fp, char *buffer, const char *delimiters, node **nodes, unsigned long nr_of_nodes, bool nsucc_only,
               unsigned int *current_nsucc) {
    // method for adding the edges to the nodes array
    // gets called within the get_nodes function after reading all the nodes
    // (to collect the number of neighbours for each node, nsucc_only=true)
    // and from build_edges to really build the edges in the graph (nsucc_only=false)
    // current_nsucc is provided by build_edges and provides the /current/ number of neighbours during the adding


    bool oneway;
    unsigned long head_id;
    unsigned long tail_id;
    unsigned long tail_index;
    unsigned long head_index;
    size_t characters;

    while (buffer[0] == 'w') {
        for (int i = 0; i < 7; ++i) {
            strsep(&buffer, delimiters); // skip the first 7 useless entries
        }
        if (strcmp(strsep(&buffer, delimiters), "oneway") == 0) {
            oneway = false;
        } else {
            oneway = true;
        }
        strsep(&buffer, delimiters); // skip maxspeed

        // now the member nodes
        // we assume that there is no node with id=0 which is the case for catalunya and spain
        // this is important because strtoul returns 0 if the input is not a valid unsigned long
        // which would be ambiguous in the case of a node with id=0
        tail_id = strtoul(strsep(&buffer, delimiters), NULL, 0);
        head_id = strtoul(strsep(&buffer, delimiters), NULL, 0);
        while (tail_id != 0 && head_id != 0) { // while not reached end of line
            // we need the indices to access the nodes array/list
            tail_index = get_node_by_id(*nodes, nr_of_nodes, tail_id);
            head_index = get_node_by_id(*nodes, nr_of_nodes, head_id);
            // we can only add edges (or count the number of neighbours
            // both tail and head exists in the node list
            // otherwise we have to ignore them and move on (see the elses)
            if (tail_index != -1 && head_index != -1) {
                if (nsucc_only == true) {
                    ((*nodes) + tail_index)->nsucc++;
                    if (oneway == false) ((*nodes) + head_index)->nsucc++;
                } else {
                    add_edge(nodes, tail_index, head_index, current_nsucc[tail_index]);
                    current_nsucc[tail_index]++;
                    if (oneway == false) {
                        add_edge(nodes, head_index, tail_index, current_nsucc[head_index]);
                        current_nsucc[head_index]++;
                    }
                }
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
        characters = (size_t) getline(&buffer, &characters, fp);
    }

    // nsucc_only is true we have know now the number of neighbours for each node
    // therefore we allocate the memory for the adjacency list once for all nodes
    // to avoid reallocations
    if (nsucc_only == true) {
        for (int i = 0; i < nr_of_nodes; ++i) {
            ((*nodes) + i)->successors = malloc(((*nodes) + i)->nsucc * sizeof(unsigned long));
        }
    }

}


void write_binary_file(char *filename, node *nodes, unsigned long nr_of_nodes) {
    // writes a constructed graph (i.e. nodes list) to a binary file for a later very fast re-read

    FILE *fin;
    unsigned long ntotnsucc = 0UL;
    for (int i = 0; i < nr_of_nodes; i++) ntotnsucc += nodes[i].nsucc;

    strcpy(strrchr(filename, '.'), ".bin");

    if ((fin = fopen(filename, "wb")) == NULL) exit(31);

    /* Global data --- header */
    if (fwrite(&nr_of_nodes, sizeof(unsigned long), 1, fin) + fwrite(&ntotnsucc, sizeof(unsigned long), 1, fin) != 2)
        exit(32);

    /* Writing all nodes */
    if (fwrite(nodes, sizeof(node), nr_of_nodes, fin) != nr_of_nodes) exit(32);

    /* Writing sucessors in blocks */
    for (int i = 0; i < nr_of_nodes; i++)
        if (nodes[i].nsucc) {
            if (fwrite(nodes[i].successors, sizeof(unsigned long), nodes[i].nsucc, fin) !=
                nodes[i].nsucc)
                exit(32);
        }
    fclose(fin);
}


unsigned long read_binary_file(char *filename, node **nodes) {
    // reads a binary files which has been written before by write_binary_file
    // this is method is way faster than read_csv_file

    unsigned long nr_of_nodes;
    FILE *fin;
    unsigned long ntotnsucc = 0UL;
    unsigned long *allsuccessors;

    if ((fin = fopen(filename, "r")) == NULL) exit(11);

    /* Global data --- header */
    if (fread(&nr_of_nodes, sizeof(unsigned long), 1, fin) + fread(&ntotnsucc, sizeof(unsigned long), 1, fin) != 2)
        exit(12);

    /* getting memory for all data */
    if ((*nodes = (node *) malloc(nr_of_nodes * sizeof(node))) == NULL) exit(13);

    if ((allsuccessors = (unsigned long *) malloc(ntotnsucc * sizeof(unsigned long))) == NULL) exit(15);

    /* Reading all data from file */
    if (fread(*nodes, sizeof(node), nr_of_nodes, fin) != nr_of_nodes) exit(17);

    if (fread(allsuccessors, sizeof(unsigned long), ntotnsucc, fin) != ntotnsucc) exit(18);

    fclose(fin);
    /* Setting pointers to successors */
    for (int i = 0; i < nr_of_nodes; i++)
        if ((*nodes)[i].nsucc) {
            (*nodes)[i].successors = allsuccessors;
            allsuccessors += (*nodes)[i].nsucc;
        }
    return nr_of_nodes;
}


unsigned long read_csv_file(char *filename, node **nodes) {
    // reads a .csv file and writes it to a binary file for a later fast re-read

    unsigned long nr_of_nodes = 0;
    // count number of nodes for a proper array initialization no reallocs
    // not sure what is faster here: read node lines twice and malloc once
    // or read once and realloc if necessary
    // also number of neighbours for each nodes are read beforehand and stored
    // therefore we only have to allocate memory for the adjacency lists once
    nr_of_nodes = get_nr_of_nodes(filename);
    get_nodes(filename, nodes, nr_of_nodes);
    unsigned int *current_nsucc = (unsigned int *) calloc(nr_of_nodes, sizeof(unsigned int));
    build_edges(filename, nodes, nr_of_nodes, current_nsucc);
    free(current_nsucc);

    write_binary_file(filename, *nodes, nr_of_nodes);
    return nr_of_nodes;
}



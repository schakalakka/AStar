
#ifndef ASTAR_ASTAR_H
#define ASTAR_ASTAR_H

#endif //ASTAR_ASTAR_H


#define _GNU_SOURCE  //necessary for getline method

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    unsigned long id;
    char *name;
    double lat, lon; //node coordinates
    unsigned short nsucc; //number of node successors, i.e. length of successors
    unsigned long *successors;
} node;

typedef char Queue;
enum whichQueue {
    NONE, OPEN, CLOSED
};

typedef struct {
    double g, h;
    unsigned long parent;
    Queue whq;
} AStarStatus;

//functions in parser.h
int read_csv_file(char *, node **);

unsigned long get_nr_of_nodes(char *);

void get_nodes(char *, node **, int);

unsigned long get_next_edge_node(char **, const char *);

void get_edges(FILE *, char *, const char *, node **, int, bool, unsigned int *);

void add_edge(node **, unsigned long, unsigned long, unsigned int);

// functions in astar.c
long get_node_by_id(node *, unsigned long, unsigned long);



// possible functions to dump the read nodes with edges to a binary file and re-read it faster
unsigned long read_binary_file(char *, node **, unsigned long );
void write_binary_file(char *, node *, unsigned long);

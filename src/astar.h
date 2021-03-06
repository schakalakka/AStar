
#ifndef ASTAR_ASTAR_H
#define ASTAR_ASTAR_H

#endif //ASTAR_ASTAR_H


#define _GNU_SOURCE  //necessary for getline method

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>


/////////////////////////////////////////////////////////////////////////////
// CONSTANTS
#define R 6371000 // Earth's radius


/////////////////////////////////////////////////////////////////////////////
// STRUCTS
typedef struct {
    unsigned long id;
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

typedef struct list_elem {
    unsigned long index;
    struct list_elem *next;
} list_elem;

typedef char Heuristic;
enum heuristic {
    HAVERSINE, EQUIRECTANGULAR
};


/////////////////////////////////////////////////////////////////////////////
// METHODS

//functions in parser.h
unsigned long read_csv_file(char *, node **);

unsigned long get_nr_of_nodes(char *);

void get_nodes(char *, node **, unsigned long);

unsigned long get_next_edge_node(char **, const char *);

void get_edges(FILE *, char *, const char *, node **, unsigned long, bool, unsigned int *);

void add_edge(node **, unsigned long, unsigned long, unsigned int);

unsigned long read_binary_file(char *, node **);

void write_binary_file(char *, node *, unsigned long);

void build_edges(char *, node **, unsigned long, unsigned int *);


// functions in astar.c
unsigned long get_node_by_id(node *, unsigned long, unsigned long);

double heuristic_distance(unsigned long, unsigned long, node *, Heuristic distance_method);

double haversine_distance(unsigned long, unsigned long, node *);

double equirectangular_distance(unsigned long, unsigned long, node *);

void astar(unsigned long, unsigned long, node *, unsigned long, Heuristic, char *);

double get_fscore(AStarStatus);

void add_element_to_list(unsigned long, list_elem **, AStarStatus *);

void remove_element_from_list(unsigned long, list_elem **);

void write_solution_to_file(char* , unsigned long , node* , AStarStatus* );

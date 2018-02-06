# AStar
A* implementation in C

AUTHORS: Moran Gybels, Andreas Radke

Compiling:
    Either use cmake and the delivered CMakeLists.txt:
        cmake CMakeLists.txt
        make
    OR with a simple gcc compilation:
        gcc -Ofast -lm -std=c99 src/* -o astar
        OR
        gcc -Ofast -lm -std=c99 src/astar.h src/astar.c src/parser.c -o astar

USAGE:
For binary file creation (creates name.bin for given name.csv):
    ./astar spain.csv
For computing an optimal route:
    ./astar spain.bin
    OR
    ./astar spain.bin source_node_id goal_node_id

The source_node_id and goal_node_id are optional. The default values are
source_node_id: 240949599 Basílica de Santa Maria del Mar (Plaça de Santa Maria) in Barcelona,
goal_node_id: 195977239 Giralda (Calle Mateos Gago) in Sevilla.
If you want to change the source and goal you can optionally provide different IDs.



EXIT CODES:
0   SUCCESS
1   FAILURE
11  No Solution found, open list is empty
31  Problems during file opening
32  Problems during file reading
33  Problems during file writing
41  Could not find element in list for removal
51  No heuristic distance method is set
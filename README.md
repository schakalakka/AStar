# AStar
A* implementation in C

AUTHORS: Moran Gybels, Andreas Radke

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
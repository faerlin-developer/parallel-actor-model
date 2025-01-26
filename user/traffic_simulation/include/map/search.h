#ifndef SEARCH_H
#define SEARCH_H

#include <cstddef>
#include <vector>
#include "map/graph.h"
#include "map/data.h"

/**
 * Type of element in priority queue when running Dijktra's algorithm.
 */
struct Node {
    int id;
    int parent_id;
    int priority;

    Node(int id, int parent_id, int priority);
};

/**
 * Comparator for min-heap behavior
 */
struct CompareNode {
    bool operator()(const Node &n1, const Node &n2) {
        return n1.priority > n2.priority;
    }
};

int plan_route(std::vector<node::Junction> &road_map, int source_id, int dest_id,
               std::vector<data::Road> *source_roads = NULL);

#endif

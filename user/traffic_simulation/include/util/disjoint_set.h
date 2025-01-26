#ifndef DISJOINT_SET_H
#define DISJOINT_SET_H

#include <vector>
#include <unordered_map>

/**
 * A data structure that keeps track of a collection of non-overlapping subsets of a universal set.
 *
 * It supports two primary operations efficiently:
 * (1) find: Determines which subset a particular element belongs to.
 *           This operation helps check if two elements are in the same subset.
 * (2) connect: Merges two subsets into a single subset.
 */
class DisjointSet {
public:

    std::vector<int> root;
    std::vector<int> rank;

public:
    DisjointSet() = default;

    explicit DisjointSet(int size);

    int find(int x);

    void connect(int x, int y);

    std::unordered_map<int, std::vector<int>> get_connected_components();
};


#endif

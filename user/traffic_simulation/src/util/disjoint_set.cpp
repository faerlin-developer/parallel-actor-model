#include <iostream>
#include "util/disjoint_set.h"

DisjointSet::DisjointSet(int size) {

    rank = std::vector<int>(size, 1);
    root = std::vector<int>(size);
    for (int i = 0; i < size; i++) {
        root[i] = i;
    }
}

// Path compression
int DisjointSet::find(int x) {

    if (x == root[x]) {
        return x;
    } else {
        root[x] = find(root[x]);
        return root[x];
    }
}

void DisjointSet::connect(int x, int y) {

    int root_x = find(x);
    int root_y = find(y);

    if (root_x != root_y) {

        if (rank[root_x] < rank[root_y]) {
            root[root_x] = root_y;
        } else if (rank[root_x] > rank[root_y]) {
            root[root_y] = root_x;
        } else {
            root[root_y] = root_x;
            rank[root_x] += 1;
        }
    }
}

std::unordered_map<int, std::vector<int>> DisjointSet::get_connected_components() {

    auto components = std::unordered_map<int, std::vector<int>>();
    for (int x = 0; x < root.size(); x++) {
        auto root_x = find(x);
        components[root_x].push_back(x);
    }

    return components;
}


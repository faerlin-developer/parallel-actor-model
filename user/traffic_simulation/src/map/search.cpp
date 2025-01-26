
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <cstdio>
#include <cstdlib>
#include "map/search.h"
#include "map/data.h"
#include "constants/constants.h"

Node::Node(int id, int parent_id, int priority) : id(id), parent_id(parent_id), priority(priority) {}

/**
 * Run Dijkstra's algorithm and verify if there is path from the given source and destination junctions.
 * If there is a valid path, return the ID of the next junction. Otherwise, return -1.
 * The source_roads parameter are the outgoing roads of the source junction.
 *
 * This implementation uses a priority queue to determine which junction to visit.
 */
int
plan_route(std::vector<node::Junction> &road_map, int source_id, int dest_id, std::vector<data::Road> *source_roads) {

    if (source_id == dest_id) {
        fprintf(stderr, "source and destination IDs must not be identical\n");
        return -1;
    }

    std::unordered_set<int> visited;
    std::unordered_map<int, int> prev;
    std::priority_queue<Node, std::vector<Node>, CompareNode> pq;

    pq.emplace(source_id, -1, -1);

    bool found = false;
    while (!pq.empty()) {

        auto node = pq.top();
        pq.pop();

        if (node.id == dest_id) {
            prev[dest_id] = node.parent_id;
            found = true;
            break;
        } else if (visited.count(node.id) == 0) {

            for (int i = 0; i < road_map[node.id].roads.size(); i++) {

                auto road = road_map[node.id].roads[i];
                auto road_speed = road.max_speed;
                if (source_roads != NULL && node.id == source_id) {
                    road_speed = source_roads->at(i).current_speed;
                }

                auto priority = node.priority + road.road_length / road_speed;
                pq.emplace(road.dest->id, node.id, priority);
            }

            visited.insert(node.id);
            prev[node.id] = node.parent_id;
        }
    }

    if (found) {
        int node = dest_id;
        while (prev[node] != source_id) {
            node = prev[node];
        }
        return node;
    } else {
        return -1;
    }
}

/**
 * Finds the index of the input array that is active and has the smallest number
 **/
int findIndexOfMinimum(double *dist, char *active, int num_junctions) {
    double min_dist = LARGE_NUM + 1;
    int current_min = -1;
    for (int i = 0; i < num_junctions; i++) {
        if (active[i] && dist[i] < min_dist) {
            min_dist = dist[i];
            current_min = i;
        }
    }
    return current_min;
}


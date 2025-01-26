#include <cstdlib>
#include <vector>
#include "actor/framework.h"
#include "actor/actor.h"
#include "datatype.h"
#include "actors.h"

/**
 * Example 2 - Sum Reduction In a Linked List
 *
 * This program creates five actors, each representing a node in a linked list.
 * Each node is initialized with a value and transmits its value to the subsequent node.
 * Additionally, each node forwards any received value to the next node in the chain.
 * At the end of the program, the tail node prints the cumulative sum of all values on the console.
 */

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);

    // Create instance of actor model framework
    auto num_actors_per_procs = 3;
    auto framework = ParallelActorModel(num_actors_per_procs);

    // Create custom MPI_Datatype called MPI_TERMINATE
    // When an actor receives MPI_TERMINATE, it terminates.
    MPI_Create_terminate_datatype();

    // Register MPI_Datatype
    framework.addType(mail::Type{sizeof(int), MPI_INT});
    framework.addType(mail::Type{sizeof(Terminate), MPI_TERMINATE});

    // Create actors
    auto node1 = new HeadNode(1, 10);   // Head of linked list
    auto node2 = new Node(2, 20);
    auto node3 = new Node(3, 30);
    auto node4 = new Node(4, 40);
    auto node5 = new TailNode(5, 50);   // Tail of linked list

    // Register actors
    framework.addActor(node1);
    framework.addActor(node2);
    framework.addActor(node3);
    framework.addActor(node4);
    framework.addActor(node5);

    // Initialize and run actors
    framework.start();

    MPI_Finalize();

    return EXIT_SUCCESS;
}

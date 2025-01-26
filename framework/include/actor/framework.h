#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <vector>
#include <unordered_map>
#include "actor/actor.h"
#include "mail/types.h"

#define MPI_BUFFER_SIZE 1024*1024*10
#define MAX_NUM_MESSAGE_PER_ITERATION 20

/**
 * A framework for the actor model.
 *
 * - The framework distinguishes between two categories of actors when assigning them to an MPI process:
 *   (1) grouped actors
 *   (2) isolated actors
 * - An MPI process managing a group of actors is referred to as handling grouped actors.
 *   These actors are added to the framework using the `addActor` method.
 * - An MPI process managing a single actor is referred to as handling an isolated actor.
 *   Such an actor is added to the framework using the `addIsolatedActor` method.
 */
class ParallelActorModel {
public:

    // Constructor Parameters
    int num_actors_per_procs = 0;        // Number of actors per MPI process
    bool ingress_mode;                   // Active ingress mode when true
    bool log_debug;                      // Active DEBUG logging when true
    int max_num_message_per_iteration;   // Maximum number of messages received per ingress

    int grouped_actors_size;             // Current number of grouped actors across all MPI processes
    int num_procs_for_grouped_actors;    // Current number of processes that manages grouped actors
    std::unordered_map<actor::id, actor::Actor *> actors;        // Collection of actors managed by current MPI process
    std::unordered_map<actor::id, mail::Address> id_to_address;  // Map of actor ID to its mailbox address
    std::vector<mail::Type> mail_types;  // List of data types supported by the messaging system between actors
    int rank = 0;
    int num_procs = 0;

public:

    explicit ParallelActorModel(int num_actors_per_procs,
                                bool ingress_mode = false,
                                bool log_debug = false,
                                int max_num_message_per_iteration = MAX_NUM_MESSAGE_PER_ITERATION);

    bool addActor(actor::Actor *actor);

    bool addIsolatedActor(actor::Actor *actor);

    void addType(mail::Type type);

    void start();

private:

    bool initialize_actors();

    void finalize_actors(std::vector<actor::id> &stopped_actors);

};

#endif

#include <iostream>
#include "mpi.h"
#include "actor/actor.h"
#include "mail/mailbox.h"
#include "mail/message.h"
#include "actor/framework.h"

ParallelActorModel::ParallelActorModel(
        int num_actors_per_procs, bool ingress_mode, bool log_debug, int max_num_message_per_iteration)
        : num_actors_per_procs(num_actors_per_procs),
          ingress_mode(ingress_mode),
          log_debug(log_debug),
          max_num_message_per_iteration(max_num_message_per_iteration) {

    char *buffer = (char *) malloc(MPI_BUFFER_SIZE);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Buffer_attach(buffer, MPI_BUFFER_SIZE);
    num_procs_for_grouped_actors = num_procs;
    grouped_actors_size = 0;
}

/**
 * Add an actor to an MPI process that manages multiple actors.
 */
bool ParallelActorModel::addActor(actor::Actor *actor) {

    // Verify actor ID is unique
    if (actors.find(actor->id) != actors.end()) {
        fprintf(stderr, "ERROR: actor %d already exists!\n", actor->id);
        return false;
    }

    // Verify availability of space for actor
    if (grouped_actors_size == num_procs_for_grouped_actors * num_actors_per_procs) {
        fprintf(stderr, "ERROR: framework is full\n");
        return false;
    }

    // Assign an address to the new actor
    auto actor_rank = grouped_actors_size / num_actors_per_procs;
    auto actor_tag = grouped_actors_size % num_actors_per_procs;
    auto address = mail::Address(actor_rank, actor_tag);
    id_to_address[actor->id] = address;
    grouped_actors_size++;

    // Check if this actor is handled by current process; destroy otherwise.
    if (address.rank != rank) {
        actor->finalize();
        return false;
    }

    // Current MPI process stores new actor
    auto context = mail::Context{&mail_types, &id_to_address};
    actor->mailbox = mail::Mailbox(address, context);
    actors[actor->id] = actor;

    return true;
}

/**
 * Add an actor to an MPI process that exclusively manages a single actor.
 */
bool ParallelActorModel::addIsolatedActor(actor::Actor *actor) {

    // Verify actor ID is unique
    if (actors.find(actor->id) != actors.end()) {
        fprintf(stderr, "ERROR: actor %d already exists!\n", actor->id);
        return false;
    }

    // Verify availability for an isolated actor
    auto available_isolated_actor_rank = num_procs_for_grouped_actors - 1;
    auto available_group_actor_rank = grouped_actors_size / num_actors_per_procs;
    if (available_isolated_actor_rank < available_group_actor_rank) {
        fprintf(stderr, "ERROR: no available process for isolated actor.\n");
        return false;
    }

    auto available_group_actor_rank_is_empty = (grouped_actors_size % num_actors_per_procs) == 0;
    if (available_isolated_actor_rank == available_group_actor_rank && !available_group_actor_rank_is_empty) {
        fprintf(stderr, "ERROR: no available process for isolated actor.\n");
        return false;
    }

    // Assign an address to the new actor
    auto address = mail::Address(available_isolated_actor_rank, 0);
    id_to_address[actor->id] = address;
    num_procs_for_grouped_actors--;

    // Check if this actor is handled by current process; destroy otherwise.
    if (address.rank != rank) {
        actor->finalize();
        return false;
    }

    // Current MPI process stores new actor
    auto context = mail::Context{&mail_types, &id_to_address};
    actor->mailbox = mail::Mailbox(address, context);
    actors[actor->id] = actor;

    return true;
}

/**
 * Register a data type for actors to use as a payload in their message exchanges.
 */
void ParallelActorModel::addType(mail::Type type) {
    mail_types.push_back(type);
}

/**
 * Run the actor model execution cycle.
 *
 * (1) Initialize all actors (see `initialize_actors` method)
 * (2) For each actor
 *     - Receive and process messages via its `ingress` method
 *     - Call its `run` method
 *     - Upon termination, remove an actor from the execution cycle.
 */
void ParallelActorModel::start() {

    auto success = initialize_actors();
    if (!success) {
        fprintf(stderr, "failed to initialize all actors.\n");
        return;
    }

    while (!actors.empty()) {

        // Maintain a list of stopped actors
        std::vector<actor::id> stopped_actors;

        // Run actors
        for (const auto &kv: actors) {

            auto id = kv.first;
            auto next_step = actor::CONTINUE;

            // Actor receives and process messages via the `ingress` method
            if (ingress_mode) {
                int messages = 0;
                while (next_step == actor::CONTINUE
                       && actors[id]->mailbox.hasMessage()
                       && messages < max_num_message_per_iteration) {
                    auto message = actors[id]->mailbox.receive();
                    next_step = actors[id]->ingress(message);
                    message.discard();
                    messages++;
                }
            }

            // Actor calls the `run` method
            if (next_step == actor::CONTINUE) {
                next_step = actors[id]->run();
            }

            // Keep track of stopped actors
            if (next_step == actor::STOP) {
                stopped_actors.push_back(id);
            }
        }

        // Remove stopped actors from execution cycle
        finalize_actors(stopped_actors);
    }
}

/**
 * Initialize the actors by executing the following steps in sequence:
 *
 * (1) For each actor, execute their `pre_barrier_init` methods.
 * (2) Wait for all actors to complete the execution of their `pre_barrier_init`.
 * (3) For each actor, execute their `post_barrier_init` methods.
 */
bool ParallelActorModel::initialize_actors() {

    double start_time = MPI_Wtime();

    // Run `pre_barrier_init` for each actor
    for (const auto &kv: actors) {
        auto success = kv.second->pre_barrier_init();
        if (!success) {
            return false;
        }
    }

    // Wait for all actors to finish initialization
    MPI_Barrier(MPI_COMM_WORLD);

    // Run `post_barrier_init` for each actor
    for (const auto &kv: actors) {
        auto success = kv.second->post_barrier_init();
        if (!success) {
            return false;
        }
    }

    double end_time = MPI_Wtime();
    if (log_debug && rank == 0) {
        printf("[DEBUG] Framework initialize_actors() takes %f seconds\n", end_time - start_time);
        fflush(stdout);
    }

    return true;
}

/**
 * Remove actors from the execution cycle that have terminated.
 */
void ParallelActorModel::finalize_actors(std::vector<actor::id> &stopped_actors) {
    for (const auto &id: stopped_actors) {
        auto actor = actors[id];
        actors.erase(id);
        actor->finalize();
    }
}

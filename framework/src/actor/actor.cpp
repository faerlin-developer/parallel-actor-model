
#include "actor/actor.h"

/**
 * The derived class constructor must call this constructor.
 */
actor::Actor::Actor(int id) : id(id) {}

actor::Actor::~Actor() = default;

/**
 * Initialization method called before the synchronisation barrier.
 * All actors must complete executing `pre_barrier_init` before proceeding.
 */
bool actor::Actor::pre_barrier_init() {
    return true;
}

/**
 * Initialization method called after the synchronisation barrier.
 * This method is useful for initializing variables that are dependent on time.
 * To be effective, its execution time must be kept brief.
 */
bool actor::Actor::post_barrier_init() {
    return true;
}

/**
 * Processes a message received from other actors.
 * In the execution cycle, this method is called multiple times before calling the `run` method.
 */
actor::next_step actor::Actor::ingress(mail::Message &message) {
    return actor::CONTINUE;
}

void actor::Actor::finalize() {
    delete this;
}

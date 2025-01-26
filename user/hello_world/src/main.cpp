#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "mpi.h"
#include "actor/framework.h"

/**
 * Example 1 - Hello World
 *
 * This program creates two actors: Bob and Alice.
 * Bob sends the null terminated string "Hello World!" to Alice.
 * Alice prints the message it received to the console.
 */

class Bob : public actor::Actor {
public:

    // Note the required call to base class constructor
    Bob(int id) : Actor(id) {};

    actor::next_step run() override {

        auto payload = "Hello World!";

        // Create message
        mail::Message message;
        message.count = strlen(payload) + 1;    // include the null terminator '\0'
        message.mpi_datatype = MPI_CHAR;        // Don't forget to register MPI_CHAR
        message.data = (void *) payload;        // Pass payload as void*

        // Send message
        auto receiver_id = 2;
        mailbox.send(message, receiver_id);

        return actor::STOP;
    }
};

class Alice : public actor::Actor {
public:

    // Note the required call to base class constructor
    Alice(int id) : Actor(id) {};

    actor::next_step run() override {

        if (mailbox.hasMessage()) {
            auto message = mailbox.receive();
            printf("%s\n", (char *) message.data);
            return actor::STOP;
        } else {
            // Invoke run method again
            return actor::CONTINUE;
        }

    }
};

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);

    // Create instance of actor model framework
    int num_actors_per_process = 5;
    auto framework = ParallelActorModel(num_actors_per_process);

    // Create actors
    auto bob_id = 1;
    auto alice_id = 2;
    auto bob = new Bob(bob_id);
    auto alice = new Alice(alice_id);

    // Register actors
    framework.addActor(bob);
    framework.addActor(alice);

    // Register MPI_Datatype used by actors
    framework.addType(mail::Type{sizeof(char), MPI_CHAR});

    // Run actors
    framework.start();

    MPI_Finalize();

    return EXIT_SUCCESS;
}
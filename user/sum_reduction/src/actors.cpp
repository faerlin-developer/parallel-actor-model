
#include <cstdio>
#include <cstdlib>
#include "actors.h"
#include "datatype.h"

/*************************************************************************
 * HeadNode                                                              *
 *************************************************************************/

HeadNode::HeadNode(int id, int value) : Actor(id) {
    this->value = value;
}

actor::next_step HeadNode::run() {

    sleep(1);

    // Send initial value to the next node
    mail::Message message{};
    message.count = 1;
    message.mpi_datatype = MPI_INT;
    message.data = &value;
    mailbox.send(message, id + 1);
    printf("[Node %d] sent value=%d to node_id=%d\n", id, value, id + 1);
    fflush(stdout);

    // Send terminate message
    Terminate terminate;
    message.count = 1;
    message.mpi_datatype = MPI_TERMINATE;
    message.data = &terminate;
    mailbox.send(message, id + 1);

    return actor::STOP;
}

/*************************************************************************
 * Node                                                                  *
 *************************************************************************/

Node::Node(int id, int value) : Actor(id) {
    this->value = value;
    this->has_sent_initial_value = false;
}

actor::next_step Node::run() {

    sleep(1);

    // Send initial value to the next node
    if (!has_sent_initial_value) {
        mail::Message message;
        message.count = 1;
        message.mpi_datatype = MPI_INT;
        message.data = &value;
        mailbox.send(message, id + 1);
        printf("[Node %d] sent value=%d to node_id=%d\n", id, value, id + 1);
        fflush(stdout);
        has_sent_initial_value = true;
    }

    // Forward any received value to the next node
    while (mailbox.hasMessage()) {

        auto message = mailbox.receive();

        if (message.mpi_datatype == MPI_INT) {
            auto data = (int *) message.data;
            mailbox.send(message, id + 1);
            printf("[Node %d] sent value=%d to node_id=%d\n", id, *data, id + 1);
            fflush(stdout);
            return actor::CONTINUE;

        } else if (message.mpi_datatype == MPI_TERMINATE) {

            Terminate terminate;

            // Send terminate message
            message.count = 1;
            message.mpi_datatype = MPI_TERMINATE;
            message.data = &terminate;
            mailbox.send(message, id + 1);

            return actor::STOP;
        } else {
            // Should not happen
            fprintf(stderr, "received unexpected mpi datatype\n");
            return actor::STOP;
        }
    }

    return actor::CONTINUE;
}

/*************************************************************************
 * TailNode                                                              *
 *************************************************************************/

TailNode::TailNode(int id, int value) : Actor(id) {
    this->value = value;
}

actor::next_step TailNode::run() {

    sleep(1);

    // Receive values from the previous node and accumulate sum
    while (mailbox.hasMessage()) {

        auto message = mailbox.receive();

        if (message.mpi_datatype == MPI_INT) {
            auto data = (int *) message.data;
            printf("[Node %d] received value=%d\n", id, *data);
            fflush(stdout);
            value += *data;
            return actor::CONTINUE;
        } else if (message.mpi_datatype == MPI_TERMINATE) {
            printf("[Node %d] sum=%d\n", id, value);
            fflush(stdout);
            return actor::STOP;
        } else {
            // Should not happen
            fprintf(stderr, "received unexpected mpi datatype\n");
            return actor::STOP;
        }
    }

    return actor::CONTINUE;
}
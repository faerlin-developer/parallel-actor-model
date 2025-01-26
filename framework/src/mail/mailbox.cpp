
#include <iostream>
#include <cstdlib>
#include "mail/mailbox.h"
#include "mail/message.h"
#include "actor/types.h"

mail::Mailbox::Mailbox() = default;

/**
 * Each actor is assigned a Mailbox.
 * Through the Mailbox API, an actor is able to send and receive messages.
 */
mail::Mailbox::Mailbox(mail::Address address, mail::Context context) : address(address), context(context) {}

/**
 * Returns true if there is a message waiting to be received by the actor.
 * Call the `receive` method to retrieve message.
 */
bool mail::Mailbox::hasMessage() const {
    int flag = 0;
    MPI_Iprobe(MPI_ANY_SOURCE, address.tag, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
    return flag != 0;
}

/**
 * Retrieve a message from the receive buffer.
 * If the receive buffer is empty, this method blocks until a message arrives.
 */
mail::Message mail::Mailbox::receive() const {

    // Receive payload metadata (i.e. data type and count)
    auto metadata = new int[2];
    MPI_Status status_source;
    MPI_Recv(metadata, 2, MPI_INT, MPI_ANY_SOURCE, address.tag, MPI_COMM_WORLD, &status_source);

    auto source = status_source.MPI_SOURCE;  // Rank of sender
    auto tag = status_source.MPI_TAG;        // For a given rank, this tag identifies the sending actor
    auto index = metadata[0];                // Identifies the datatype of the payload
    auto count = metadata[1];                // Indicates the count of data elements received
    delete[] metadata;

    auto type = context.mail_types->at(index);
    auto mpi_datatype = type.mpi_datatype;     // MPI_Datatype of the payload
    auto size_datatype = type.size_bytes;      // The size of the MPI_Datatype

    // Receive actual payload (i.e. array of MPI_Datatype)
    void *data = malloc(size_datatype * count);
    MPI_Recv(data, count, mpi_datatype, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Return message
    mail::Message message;
    message.count = count;
    message.data = data;
    message.mpi_datatype = mpi_datatype;

    return message;
}

/**
 * Sends a message to an actor specified by its id.
 */
void mail::Mailbox::send(Message &message, actor::id to) const {

    // Determine the unique index that corresponds to the payload's data type.
    int index = -1;
    for (int i = 0; i < context.mail_types->size(); i++) {
        if (message.mpi_datatype == context.mail_types->at(i).mpi_datatype) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        // This should not happen
        fprintf(stderr, "ERROR: failed to find datatype\n");
        fflush(stderr);
        return;
    }

    // Send metadata payload (i.e data type and count), followed by the actual payload
    auto to_address = context.id_to_address->at(to);
    std::vector<int> metadata = {index, message.count};
    MPI_Bsend(metadata.data(), (int) metadata.size(), MPI_INT, to_address.rank, to_address.tag, MPI_COMM_WORLD);
    MPI_Bsend(message.data, message.count, message.mpi_datatype, to_address.rank, to_address.tag, MPI_COMM_WORLD);
}

mail::Mailbox::~Mailbox() {}


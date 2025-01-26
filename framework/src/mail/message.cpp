
#include <cstdlib>
#include <functional>
#include "mail/message.h"

mail::Address::Address() = default;

mail::Address::Address(int rank, int tag) : rank(rank), tag(tag) {}

bool mail::Address::operator==(const mail::Address &other) const {
    return rank == other.rank && tag == other.tag;
}

mail::Message::Message() = default;

mail::Message::Message(void *data, int count, MPI_Datatype mpi_datatype) :
        data(data), count(count), mpi_datatype(mpi_datatype) {}

/**
 * Free data of message.
 */
void mail::Message::discard() {
    free(this->data);
    this->data = NULL;
}



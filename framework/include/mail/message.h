#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstddef>
#include "mpi.h"
#include "actor/types.h"

namespace mail {

    /**
     * The combination of rank and tag serve as the unique address of an actor
     * in the framework's messaging system.
     */
    class Address {
    public:

        int rank = 0;
        int tag = 0;

        Address();

        Address(int rank, int tag);

        bool operator==(const Address &other) const;
    };

    /**
     * Encapsulation of a payload sent or received by an actor.
     */
    class Message {
    public:
        void *data = NULL;
        int count = 0;
        MPI_Datatype mpi_datatype = MPI_DATATYPE_NULL;

        Message();

        Message(void *data, int count, MPI_Datatype mpi_datatype);

        void discard();
    };
}

#endif
#ifndef TYPE_H
#define TYPE_H

#include "mpi.h"

namespace mail {

    /**
     * Type supported by the framework's messaging system.
     */
    struct Type {
        int size_bytes;
        MPI_Datatype mpi_datatype;
    };

}

#endif

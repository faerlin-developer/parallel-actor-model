#ifndef DATATYPE_H
#define DATATYPE_H

#include "mpi.h"

struct Terminate {
    int dummy = 0;
};

extern MPI_Datatype MPI_TERMINATE;

void MPI_Create_terminate_datatype();

#endif

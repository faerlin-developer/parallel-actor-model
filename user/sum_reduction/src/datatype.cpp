
#include "datatype.h"

MPI_Datatype MPI_TERMINATE;

void MPI_Create_terminate_datatype() {

    Terminate terminate;

    const int count = 1;
    int block_lengths[count] = {1};
    MPI_Aint displacements[count];
    MPI_Datatype types[count] = {MPI_INT};

    MPI_Aint start_address;
    MPI_Aint offset_address;
    MPI_Get_address(&terminate, &start_address);
    MPI_Get_address(&terminate.dummy, &offset_address);
    displacements[0] = MPI_Aint_diff(offset_address, start_address);

    MPI_Type_create_struct(count, block_lengths, displacements, types, &MPI_TERMINATE);
    MPI_Type_commit(&MPI_TERMINATE);
}
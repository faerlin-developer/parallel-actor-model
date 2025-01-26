
#include "payload/datatype.h"
#include "payload/vehicle.h"
#include "payload/terminate.h"
#include "payload/summary.h"

// Vehicle object sent between junction actors
MPI_Datatype MPI_VEHICLE;

// Terminate message sent from the summary actor to all other actors.
MPI_Datatype MPI_TERMINATE;

// Vehicle statistics periodically sent from junction and fatory actors to summary actor.
MPI_Datatype MPI_PERIODIC_SUMMARY;

// Detailed summary of junction sent to summary actor at the end simulation.
MPI_Datatype MPI_JUNCTION_SUMMARY;

// Detailed summary of a road sent to summary actor at the end of simulation.
MPI_Datatype MPI_ROAD_SUMMARY;

/**
 * Create and commit MPI_VEHICLE.
 */
void MPI_Create_vehicle_datatype() {

    payload::Vehicle vehicle;

    const int count = 2;
    int num_integers = 9;
    int num_double = 1;

    int block_lengths[count] = {num_integers, num_double};
    MPI_Aint displacements[count];
    MPI_Datatype types[count] = {MPI_INT, MPI_DOUBLE};

    // Determine displacements
    MPI_Aint start_address;
    MPI_Get_address(&vehicle, &start_address);

    MPI_Aint offset_address;
    MPI_Get_address(&vehicle.id, &offset_address);
    displacements[0] = MPI_Aint_diff(offset_address, start_address);

    MPI_Get_address(&vehicle.remaining_distance, &offset_address);
    displacements[1] = MPI_Aint_diff(offset_address, start_address);

    // Create MPI type
    MPI_Type_create_struct(count, block_lengths, displacements, types, &MPI_VEHICLE);
    MPI_Type_commit(&MPI_VEHICLE);
}

/**
 * Create and commit MPI_TERMINATE.
 */
void MPI_Create_terminate_datatype() {

    payload::Terminate terminate;

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

/**
 * Create and commit MPI_PERIODIC_SUMMARY.
 */
void MPI_Create_periodic_summary_datatype() {

    payload::PeriodicSummary summary;

    const int count = 1;
    int block_lengths[count] = {5};
    MPI_Aint displacements[count];
    MPI_Datatype types[count] = {MPI_INT};

    MPI_Aint start_address;
    MPI_Aint offset_address;
    MPI_Get_address(&summary, &start_address);
    MPI_Get_address(&summary.delivered_passengers, &offset_address);
    displacements[0] = MPI_Aint_diff(offset_address, start_address);

    MPI_Type_create_struct(count, block_lengths, displacements, types, &MPI_PERIODIC_SUMMARY);
    MPI_Type_commit(&MPI_PERIODIC_SUMMARY);
}

/**
 * Create and commit MPI_JUNCTION_SUMMARY.
 */
void MPI_Create_junction_summary_datatype() {

    payload::JunctionSummary summary;

    const int count = 1;
    int block_lengths[count] = {3};
    MPI_Aint displacements[count];
    MPI_Datatype types[count] = {MPI_INT};

    MPI_Aint start_address;
    MPI_Aint offset_address;
    MPI_Get_address(&summary, &start_address);
    MPI_Get_address(&summary.id, &offset_address);
    displacements[0] = MPI_Aint_diff(offset_address, start_address);

    MPI_Type_create_struct(count, block_lengths, displacements, types, &MPI_JUNCTION_SUMMARY);
    MPI_Type_commit(&MPI_JUNCTION_SUMMARY);
}

/**
 * Create and commit MPI_ROAD_SUMMARY.
 */
void MPI_Create_road_summary_datatype() {

    payload::RoadSummary summary;

    const int count = 1;
    int block_lengths[count] = {4};
    MPI_Aint displacements[count];
    MPI_Datatype types[count] = {MPI_INT};

    MPI_Aint start_address;
    MPI_Aint offset_address;
    MPI_Get_address(&summary, &start_address);
    MPI_Get_address(&summary.source_id, &offset_address);
    displacements[0] = MPI_Aint_diff(offset_address, start_address);

    MPI_Type_create_struct(count, block_lengths, displacements, types, &MPI_ROAD_SUMMARY);
    MPI_Type_commit(&MPI_ROAD_SUMMARY);
}
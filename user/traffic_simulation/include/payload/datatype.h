#ifndef DATATYPE_H
#define DATATYPE_H

#include "mpi.h"

// Vehicle object sent between junction actors
extern MPI_Datatype MPI_VEHICLE;

// Terminate message sent from the summary actor to all other actors.
extern MPI_Datatype MPI_TERMINATE;

// Vehicle statistics periodically sent from junction and fatory actors to summary actor.
extern MPI_Datatype MPI_PERIODIC_SUMMARY;

// Detailed summary of junction sent to summary actor at the end simulation.
extern MPI_Datatype MPI_JUNCTION_SUMMARY;

// Detailed summary of a road sent to summary actor at the end of simulation.
extern MPI_Datatype MPI_ROAD_SUMMARY;

void MPI_Create_vehicle_datatype();

void MPI_Create_terminate_datatype();

void MPI_Create_periodic_summary_datatype();

void MPI_Create_junction_summary_datatype();

void MPI_Create_road_summary_datatype();

#endif

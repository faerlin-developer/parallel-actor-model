#include <cstring>
#include "actors/summary.h"
#include "constants/constants.h"
#include "payload/datatype.h"
#include "payload/terminate.h"
#include "payload/summary.h"
#include "mail/message.h"
#include "util/timer.h"

actor::Summary::Summary(actor::id id, int factory_id, int num_junctions, int initial_vehicles, int max_mins) :
        Actor(id),
        factory_id(factory_id),
        num_junctions(num_junctions),
        initial_vehicles(initial_vehicles),
        max_mins(max_mins) {}

/**
 * Initialize summary actor.
 */
bool actor::Summary::pre_barrier_init() {
    total_vehicles = initial_vehicles;
    delivered_passengers = 0;
    stranded_passengers = 0;
    crashed_vehicles = 0;
    exhausted_vehicles = 0;
    remaining_detailed_summaries = num_junctions;
    termination_sent = false;
    junction_summaries.resize(num_junctions);
    road_summaries.resize(num_junctions);
    return true;
}

/**
 * Initialize `timer`.
 */
bool actor::Summary::post_barrier_init() {
    timer = Timer(MIN_LENGTH_SECONDS);
    return true;
}

/**
 * Handle incoming messages based on their data type:
 * (1) MPI_PERIODIC_SUMMARY
 *     - Data meant to for printing to stdio
 * (2) MPI_JUNCTION_SUMMARY
 *     - Junction data meant to be written on a file
 * (3) MPI_ROAD_SUMMARY
 *     - Road data meant to be written on a file
 * (4) MPI_TERMINATE
 *     - Terminate this actor
 */
actor::next_step actor::Summary::ingress(mail::Message &message) {

    if (message.mpi_datatype == MPI_PERIODIC_SUMMARY) {

        // Receive summary from junction actors
        auto summary = (payload::PeriodicSummary *) message.data;
        delivered_passengers += summary->delivered_passengers;
        stranded_passengers += summary->stranded_passengers;
        crashed_vehicles += summary->crashed_vehicles;
        exhausted_vehicles += summary->exhausted_vehicles;
        total_vehicles += summary->total_vehicles;

        return actor::CONTINUE;

    } else if (message.mpi_datatype == MPI_JUNCTION_SUMMARY) {

        // Receive junction summary at the end of simulation
        auto *data = (payload::JunctionSummary *) message.data;
        junction_summaries[data->id] = *data;
        return actor::CONTINUE;

    } else if (message.mpi_datatype == MPI_ROAD_SUMMARY) {

        // Receive road summary at the end of simulation
        auto count = message.count;
        auto *data = (payload::RoadSummary *) message.data;
        for (int i = 0; i < count; i++) {
            road_summaries[data[i].source_id].push_back(data[i]);
        }

        // Keep track of remaining detailed summaries to be received
        remaining_detailed_summaries--;

        return actor::CONTINUE;

    } else {
        // This should not happen
        fprintf(stderr, "unexpected mpi datatype at ingress\n");
        return actor::STOP;
    }
}

actor::next_step actor::Summary::run() {

    if (timer.simulation_minutes < max_mins) {

        if (timer.update_simulation_minutes()) {

            // Print simulation progress periodically
            if (timer.simulation_minutes % SUMMARY_FREQUENCY == 0) {
                printf("[Time: %d mins] %d vehicles, %d passengers delivered, %d stranded passengers, %d crashed vehicles, %d vehicles exhausted fuel\n",
                       timer.simulation_minutes,
                       total_vehicles,
                       delivered_passengers,
                       stranded_passengers,
                       crashed_vehicles,
                       exhausted_vehicles);
                fflush(stdout);
            }

            // Print final summary at the end of simulation
            if (timer.simulation_minutes >= max_mins) {
                printf("Finished after %d mins: %d vehicles, %d passengers delivered, %d passengers stranded, %d crashed vehicles, %d vehicles exhausted fuel\n",
                       max_mins,
                       total_vehicles,
                       delivered_passengers,
                       stranded_passengers,
                       crashed_vehicles,
                       exhausted_vehicles);
                fflush(stdout);
            }
        }

        return actor::CONTINUE;

    } else {

        // Send termination messages to all actors
        if (!termination_sent) {

            double start_time = MPI_Wtime();
            send_terminate();
            double end_time = MPI_Wtime();

            if (LOG_DEBUG) {
                printf("[DEBUG] Summary send_terminate() takes %f seconds\n", end_time - start_time);
                fflush(stdout);
            }

            termination_sent = true;
            return actor::CONTINUE;
        }

        // Wait until detailed summaries from all junction actors have been received
        if (remaining_detailed_summaries != 0) {
            return actor::CONTINUE;
        }

        // Write detailed summaries to a file
        double start_time = MPI_Wtime();
        write_detailed_info();
        double end_time = MPI_Wtime();

        if (LOG_DEBUG) {
            printf("[DEBUG] Summary write_detailed_info() takes %f seconds\n", end_time - start_time);
            fflush(stdout);
        }

        return actor::STOP;
    }
}

/**
 * Send terminate message to all actors.
 */
void actor::Summary::send_terminate() {

    payload::Terminate terminate{};
    mail::Message message;
    message.count = 1;
    message.data = &terminate;
    message.mpi_datatype = MPI_TERMINATE;

    // Send terminate message to all junction actors
    for (int i = 0; i < num_junctions; i++) {
        mailbox.send(message, i);
    }

    // Send terminate message to factory actor
    mailbox.send(message, factory_id);
}

/**
 * Write detailed summary to a file.
 */
void actor::Summary::write_detailed_info() {

    FILE *f = fopen("results", "w");
    for (int i = 0; i < num_junctions; i++) {
        fprintf(f, "Junction %d: %d total vehicles and %d crashes\n",
                i, junction_summaries[i].total_number_vehicles, junction_summaries[i].total_number_crashes);
        for (int j = 0; j < road_summaries[i].size(); j++) {
            fprintf(f, "--> Road from %d to %d: Total vehicles %d and %d maximum concurrently\n",
                    road_summaries[i][j].source_id,
                    road_summaries[i][j].dest_id,
                    road_summaries[i][j].total_number_vehicles,
                    road_summaries[i][j].peak_number_vehicles);
        }
    }
    fclose(f);
}
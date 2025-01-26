#ifndef TIMER_H
#define TIMER_H

#include <string>
#include "sys/time.h"

class Timer {
public:

    int start_seconds;                        // The Unix time at which this object was created
    int simulation_minutes;                   // The elapsed simulation minutes
    int real_seconds_to_simulation_minutes;   // The ratio of real seconds to simulation minutes

    Timer();

    explicit Timer(int real_seconds_to_simulation_minutes);

    bool update_simulation_minutes();

    int get_simulation_minutes() const;

    static int get_current_seconds();

    static long get_elapsed_in_microseconds(struct timeval &start, struct timeval &end);
};

#endif

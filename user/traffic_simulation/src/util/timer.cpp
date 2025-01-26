#include <cstddef>
#include "sys/time.h"
#include "constants/constants.h"
#include "util/timer.h"

Timer::Timer() : Timer(MIN_LENGTH_SECONDS) {}

Timer::Timer(int real_seconds_to_simulation_minutes) {
    this->simulation_minutes = 0;
    this->start_seconds = get_current_seconds();
    this->real_seconds_to_simulation_minutes = real_seconds_to_simulation_minutes;
}

/**
 * Returns the current Unix time in seconds.
 */
int Timer::get_current_seconds() {
    struct timeval curr_time{};
    gettimeofday(&curr_time, NULL);
    return static_cast<int>(curr_time.tv_sec);
}

// every MIN_LENGTH_SECONDS=2 is one simulated minute

/**
 * Return the elapsed simulation minutes.
 *
 * Conversion between elapsed wall clock seconds to elapsed simulation minutes is
 * computed using real_seconds_to_simulation_minutes.
 */
int Timer::get_simulation_minutes() const {
    const auto current_seconds = get_current_seconds();
    const auto delta = current_seconds - this->start_seconds;
    return delta / this->real_seconds_to_simulation_minutes;
}

/**
 * Update the elapsed simulation minutes.
 * Returns true if simulation_minutes changed value.
 */
bool Timer::update_simulation_minutes() {

    auto t = get_simulation_minutes();
    if (t == this->simulation_minutes) {
        return false;
    }

    this->simulation_minutes = t;
    return true;
}

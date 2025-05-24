#include "usage.h"
#include <time.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <sys/time.h>

// ? Calculate the RAM Usage (In KB)
long get_memory_usage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; // ? Ram usage in KB Format (Linux)
}

// ? Calculate the CPU Usage (In %)
float get_cpu_usage() {
    static struct timeval last_time;
    static clock_t last_clock;
    static int first_call = 1;

    // ? Save the data in the first call.
    if (first_call) {
        gettimeofday(&last_time, NULL);
        last_clock = clock();
        first_call = 0;
        return 0.0;
    }

    // ? Calculate time difference
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    double time_diff = (current_time.tv_sec - last_time.tv_sec) +
                       (current_time.tv_usec - last_time.tv_usec) / 1e6;

    clock_t current_clock = clock();
    double clock_diff = (double)(current_clock - last_clock) / CLOCKS_PER_SEC;

    // ? Calculate the CPU percentage
    float cpu_usage = (clock_diff / time_diff) * 100.0;

    // ? Update the last variables.
    last_time = current_time;
    last_clock = current_clock;

    return cpu_usage;
}
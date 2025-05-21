#include "logger.h"
#include <stdio.h>
#include <time.h>

void log_message(const char* message) {
    time_t t = time(NULL); // ? Creating the timer.
    struct tm* tm = localtime(&t); // ? Using local timer.

    char filename[50]; 
    snprintf(filename, sizeof(filename), "%04d-%02d-%02d-%02d.log",
             tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour); // ? Using snprintf for writing the filename to the "filename".

    FILE* pF = fopen(filename, "a"); // ? Creating/Opening the file in append mode.
    if (pF == NULL) {
        perror("Log file can't be opened.");
        return;
    }

    fprintf(pF, "[%02d:%02d:%02d] %s\n", 
            tm->tm_hour, tm->tm_min, tm->tm_sec, message); // ? Appending the logs to the file.
    fclose(pF); // ? Closing the file.
}
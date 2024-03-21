#include "Timer.h"

unsigned Timer::time_start = now();

unsigned Timer::now() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    unsigned ns = SEC_TO_NS((unsigned)ts.tv_sec) + (unsigned)ts.tv_nsec;
    return ns;
}

unsigned Timer::time() {
	return now() - time_start;
}

/* EOF */
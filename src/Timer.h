#ifndef TIMER_H
#define TIMER_H

// This line **must** come **before** including <time.h> in order to
// bring in the POSIX functions such as `clock_gettime() from <time.h>`!
// #define _POSIX_C_SOURCE 199309L
        
#include <time.h>

#define SEC_TO_NS(sec) ((sec)*1000000000)

class Timer {
private:
	static unsigned time_start;
	static unsigned now();
	
public:
	static unsigned time();
};

#endif
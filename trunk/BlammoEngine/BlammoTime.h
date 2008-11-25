#ifndef __BLAMMOTIME_H__
#define __BLAMMOTIME_H__

#include "BasicIncludes.h"

/**
 * A class for cross-platform, high granularity time.
 */
class BlammoTime {

private:
	BlammoTime(){};
	~BlammoTime(){};

public:
	/**
	 * Platform independent method of obtaining 
	 * the system time (since machine start) in milliseconds.
	 */
	static unsigned long GetSystemTimeInMillisecs() {
#ifdef WIN32
		return GetTickCount();
#endif
		// TODO: other types of time...
	}

	/**
	 * Platform independent sleep function for the current thread.
	 */
	static void SystemSleep(unsigned long ms) {
#ifdef WIN32
		Sleep(ms);
#endif
		// TODO: other types of sleep...
	}

	static void SetVSync(int vSyncOn);
};
#endif
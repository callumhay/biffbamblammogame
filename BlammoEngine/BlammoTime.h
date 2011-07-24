/**
 * BlammoTime.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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
		return SDL_GetTicks();
	}

	/**
	 * Platform independent sleep function for the current thread.
	 */
	static void SystemSleep(unsigned long ms) {
		SDL_Delay(ms);
	}

	static void SetVSync(bool vSyncOn);
};
#endif
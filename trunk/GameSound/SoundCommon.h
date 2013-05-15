/**
 * SoundCommon.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __SOUNDCOMMON_H__
#define __SOUNDCOMMON_H__

typedef int SoundID;
static const int INVALID_SOUND_ID = -1;

static inline SoundID GenerateSoundID() {
    static int COUNTER = 0;
    if (COUNTER < 0) { COUNTER = 0; }
    return COUNTER++;
}

#endif // __SOUNDCOMMON_H__
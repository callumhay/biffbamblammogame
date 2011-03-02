/**
 * SoundProbabilityPair.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __SOUNDPROBABILITYPAIR_H__
#define __SOUNDPROBABILITYPAIR_H__

#include "../BlammoEngine/BasicIncludes.h"

// Used for pairing sound with a probability interval - used when an event has multiple
// possible sounds that it can play
class SoundProbabilityPair {
private:
	float probabilityIntervalMin;
	float probabilityIntervalMax;
	Mix_Chunk* soundChunk;           // The sound that plays when this probability interval is hit, not owned by this object

public:
	SoundProbabilityPair(float intervalMin, float intervalMax, Mix_Chunk* sound) :
	   probabilityIntervalMin(intervalMin), probabilityIntervalMax(intervalMax), soundChunk(sound) {}
   ~SoundProbabilityPair() {};

	Mix_Chunk* GetSoundChunk() const { return this->soundChunk; }
	float GetMinProb() const { return this->probabilityIntervalMin; }
	float GetMaxProb() const { return this->probabilityIntervalMax; }
	void SetMaxProb(float amt) { this->probabilityIntervalMax = amt; }
	bool IsInInterval(float val) const { return val >= this->GetMinProb() && val < this->GetMaxProb(); }
};

#endif // __SOUNDPROBABILITYPAIR_H__
#ifndef _SOUNDMASK_H_
#define _SOUNDMASK_H_

#include "Sound.h"

/**
 * A sound mask is a sound that loops forever (until it is manually told to stop).
 * The sound mask has no position or velocity in the world space, it does not fade in
 * or out. It is a basic sound that is broadcast throughout the world space. 
 */
class SoundMask : public Sound {
	friend class MSFReader;

public:
	~SoundMask();

	//void Play() { Sound::Play(); }
	//void Stop() { Sound::Stop(); }
	void Tick(double dT);

private:
	SoundMask(const std::string& filepath);

};

#endif // _SOUNDMASK_H_
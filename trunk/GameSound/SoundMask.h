#ifndef _SOUNDMASK_H_
#define _SOUNDMASK_H_

#include "Sound.h"

class SoundMask : public Sound {
	friend class MSFReader;

public:
	~SoundMask();

	void Play();
	void Tick(double dT);

private:
	SoundMask(const std::string& filepath);

};

#endif // _SOUNDMASK_H_
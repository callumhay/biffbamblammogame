#ifndef _SOUNDEVENT_H_
#define _SOUNDEVENT_H_

#include "Sound.h"

class SoundEvent : public Sound {
	friend class MSFReader;

public:
	~SoundEvent();

	void Play();
	void Tick(double dT);

private:
	SoundEvent(const std::string& filepath, int delay, int loops, int fadein, int fadeout);
	
};

#endif // _SOUNDEVENT_H_
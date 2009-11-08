#ifndef _SOUNDEVENT_H_
#define _SOUNDEVENT_H_

#include "Sound.h"

class SoundEvent : public Sound {
	friend class MSFReader;

public:
	static const int DEFAULT_DELAY = 0;
	static const int DEFAULT_LOOPS = 1;
	static const int DEFAULT_FADEIN = 0;
	static const int DEFAULT_FADEOUT = 0;

	~SoundEvent();

	//void Play() { Sound::Play(); }
	//void Stop() { Sound::Stop(); }
	void Tick(double dT);

	void SetDelay(int delayInMS) { this->startDelay = delayInMS; }
	void SetLoops(int numLoops) { this->numLoops = numLoops; }
	void SetFadein(int fadeinMS) { this->fadein  = fadeinMS; }
	void SetFadeout(int fadeoutMS) { this->fadeout = fadeoutMS; }

private:
	int startDelay;
	int numLoops;
	int fadein;
	int fadeout;

	SoundEvent(const std::string& name, const std::string& filepath);
	
};

#endif // _SOUNDEVENT_H_
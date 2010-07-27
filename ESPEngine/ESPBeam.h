	
#ifndef __ESPBEAM_H__
#define __ESPBEAM_H__

#include "ESPUtil.h"
#include "ESPBeamSegment.h"

class ESPBeam {
public:
	static const int INFINITE_BEAM_LIFETIME				 = -1;

	ESPBeam();
	~ESPBeam();

	ESPBeamSegment* GetStartSegment() const;
	void SetLifeTime(double lifeInSeconds);

	void Tick(double dT);
	void Draw() const;
	bool IsDead() const;

private:
	double currLifeTickCount;
	double lifeTimeInSecs;


	ESPBeamSegment* startSegment;	// The root/starting beam segment

	// Disallow copy and assign
	ESPBeam(const ESPBeam& b);
	ESPBeam& operator=(const ESPBeam& b);
};

inline ESPBeamSegment* ESPBeam::GetStartSegment() const {
	return this->startSegment;
}

inline void ESPBeam::SetLifeTime(double lifeInSeconds) {
	this->lifeTimeInSecs = lifeInSeconds;
}

inline void ESPBeam::Tick(double dT) {
	this->currLifeTickCount += dT;
}

inline bool ESPBeam::IsDead() const {
	return (this->lifeTimeInSecs != ESPBeam::INFINITE_BEAM_LIFETIME) && (this->lifeTimeInSecs <= this->currLifeTickCount);
}

#endif // __ESPBEAM_H__
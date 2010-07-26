	
#ifndef __ESPBEAM_H__
#define __ESPBEAM_H__

#include "ESPUtil.h"
#include "ESPBeamSegment.h"

class ESPBeam {
public:
	ESPBeam() : startSegment(new ESPBeamSegment()), lifeTimeInSecs(0.0), currLifeTickCount(0.0) {}
	
	~ESPBeam() {
		delete startSegment;
		startSegment = NULL;
	}

	ESPBeamSegment* GetStartSegment() const {
		return this->startSegment;
	}
	void SetLifeTime(double lifeInSeconds) {
		this->lifeTimeInSecs = lifeInSeconds;
	}

private:
	double currLifeTickCount;
	double lifeTimeInSecs;
	ESPBeamSegment* startSegment;	// The root/starting beam segment
};


#endif // __ESPBEAM_H__
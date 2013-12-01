/**
 * ESPBeamSegment.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#include "ESPBeamSegment.h"
#include "ESPBeam.h"

const double ESPBeamSegment::ANIMATION_TIME = 0.075;

ESPBeamSegment::ESPBeamSegment(ESPBeamSegment* parent) : parentSegment(parent), timeCounter(ANIMATION_TIME) {
}

ESPBeamSegment::~ESPBeamSegment() {
	for (std::list<ESPBeamSegment*>::iterator iter = this->childSegments.begin(); iter != this->childSegments.end(); ++iter) {
		ESPBeamSegment* seg = *iter;
		delete seg;
		seg = NULL;
	}
	this->childSegments.clear();
}

void ESPBeamSegment::Tick(double dT, const ESPBeam& parentBeam) {
	// Modify the position of the segment based on the set variation, this gives
	// the impression of a noisy lightning-like beam in some situations - do this for every mid-beam segment point
	if (timeCounter >= ANIMATION_TIME && this->childSegments.size() != 0 && this->parentSegment != NULL) {

		Vector3D randomVec = Matrix4x4::rotationMatrix(
            static_cast<float>(2*M_PI*Randomizer::GetInstance()->RandomNumNegOneToOne()), parentBeam.GetBeamLineVec()) * 
			parentBeam.GetOrthoBeamLineVec();

        this->startPt = this->endPt;
        this->animationToPt = this->defaultPtOnLine + parentBeam.GetRandomLineDistanceVariation() * parentBeam.GetBeamLineVec() + 
            parentBeam.GetRandomAmplitudeVariation() * randomVec;

		timeCounter = 0.0;
	}
    else {
        timeCounter += dT;
    }

    this->endPt = NumberFuncs::LerpOverTime<Point3D>(0.0, ANIMATION_TIME, this->startPt, this->animationToPt, this->timeCounter);

	ESPBeamSegment* childSegment;
	for (std::list<ESPBeamSegment*>::iterator iter = this->childSegments.begin(); iter != this->childSegments.end(); ++iter) {
		childSegment = *iter;
		childSegment->Tick(dT, parentBeam);
	}
}
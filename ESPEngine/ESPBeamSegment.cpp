
#include "ESPBeamSegment.h"
#include "ESPBeam.h"

ESPBeamSegment::ESPBeamSegment(ESPBeamSegment* parent) : parentSegment(parent), timeCounter(0.1) {
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
	// the impression of a noisey lightning-like beam in some situations - do this for every mid-beam segment point
	if (timeCounter >= 0.1 && this->childSegments.size() != 0 && this->parentSegment != NULL) {
		Vector3D randomVec = Matrix4x4::rotationMatrix(static_cast<float>(2*M_PI*Randomizer::GetInstance()->RandomNumNegOneToOne()), parentBeam.GetBeamLineVec()) * 
												 parentBeam.GetOrthoBeamLineVec();
		this->endPt = this->defaultPtOnLine + parentBeam.GetRandomLineDistanceVariation() * parentBeam.GetBeamLineVec() + parentBeam.GetRandomAmplitudeVariation() * randomVec;
		timeCounter = 0.0;
	}
	
	ESPBeamSegment* childSegment;
	for (std::list<ESPBeamSegment*>::iterator iter = this->childSegments.begin(); iter != this->childSegments.end(); ++iter) {
		childSegment = *iter;
		childSegment->Tick(dT, parentBeam);
	}

	timeCounter += dT;
}
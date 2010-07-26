/**
 * ESPPointToPointBeam.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "ESPPointToPointBeam.h"
#include "ESPBeam.h"

ESPPointToPointBeam::ESPPointToPointBeam() : startPt(0,0,0), endPt(0,0,0), colour(1,1,1,1), texture(NULL), 
numMainESPBeamSegments(10), numBeamsShot(0), timeSinceLastBeamSpawn(0.0), timeUntilNextBeamSpawn(0.0), 
mainBeamAmplitude(ESPInterval(0.0f)) {
	// By default there is a single beam shot that lasts forever
	this->SetNumBeamShots(1);
	this->SetTimeBetweenBeamShots(ESPInterval(0));
	this->SetBeamLifetime(ESPInterval(INFINITE_BEAM_LIFETIME));
}

ESPPointToPointBeam::~ESPPointToPointBeam() {
	// Clean up any left over beams
	for (std::list<ESPBeam*>::iterator iter = this->aliveBeams.begin(); iter != this->aliveBeams.end(); ++iter) {
		ESPBeam* beam = *iter;
		delete beam;
		beam = NULL;
	}
	this->aliveBeams.clear();
}

void ESPPointToPointBeam::Tick(double dT) {
	// If we haven't shot all the beams yet and the time since the last beam spawn is greater than the time until
	// the next beam spawn then we make a new beam...
	if (this->ThereAreStillMoreBeamsToFire() && this->timeSinceLastBeamSpawn >= this->timeUntilNextBeamSpawn) {
		// Spawn a new beam
		this->SpawnBeam();
	}

	// Go through each beam to check if it's life has expired yet


	timeSinceLastBeamSpawn += dT;
}

void ESPPointToPointBeam::Draw(const Camera& camera) {

}

void ESPPointToPointBeam::SpawnBeam() {
	ESPBeam* newBeam = new ESPBeam();

	std::list<Point3D> beamMidPts;
	Vector3D beamVec = endPt - startPt;
	float beamLength = beamVec.length();
	beamVec.Normalize();

	// Space out the points along the line from start to end point based on the 
	// number of segments that make up the main beam
	int numPts = this->numMainESPBeamSegments - 1;
	
	const float MIN_FRACTION = 0.25f;
	
	// Maximum distance from the start point along the line to the end point that
	// a point can be placed in the main beam
	const float maxPtDist = beamLength - MIN_FRACTION * (beamLength / this->numMainESPBeamSegments);

	float lengthLeftOver = maxPtDist;
	float currSegLength, currSegDistFromStart;
	float minSegmentLength, maxSegmentLength, minMaxSegLengthDiff;

	for (int i = 0; i < numPts; i++) {
		maxSegmentLength = lengthLeftOver / (numPts - i);
		minSegmentLength = MIN_FRACTION * maxSegmentLength;
		minMaxSegLengthDiff = maxSegmentLength - minSegmentLength;

		currSegLength = minSegmentLength + Randomizer::GetInstance()->RandomNumZeroToOne() * minMaxSegLengthDiff;
		currSegDistFromStart = lengthLeftOver + currSegLength;
		assert(maxPtDist >= currSegDistFromStart);

		beamMidPts.push_back(startPt + currSegDistFromStart * beamVec);
		lengthLeftOver -= currSegLength;
	}

	// Calculate an orthogonal vector for the beam line/vector
	Vector3D orthToBeamVec = Vector3D::cross(beamVec, Vector3D(1, 0, 0));
	if (orthToBeamVec == Vector3D(0, 0, 0)) {
		orthToBeamVec = Vector3D::cross(beamVec, Vector3D(0, 1, 0));
		if (orthToBeamVec == Vector3D(0, 0, 0)) {
			orthToBeamVec = Vector3D::cross(beamVec, Vector3D(0, 0, 1));
		}
	}
	assert(orthToBeamVec != Vector3D(0, 0, 0));
	orthToBeamVec = Vector3D::cross(beamVec, orthToBeamVec);
	orthToBeamVec.Normalize();

	// Apply amplitude to the beam points and add them to the beam
	ESPBeamSegment* currESPBeamSegment = newBeam->GetStartSegment();
	assert(currESPBeamSegment != NULL);

	for (std::list<Point3D>::iterator iter = beamMidPts.begin(); iter != beamMidPts.end(); ++iter) {
		
		Matrix4x4 randomRotation = Matrix4x4::rotationMatrix(static_cast<float>(2*M_PI*Randomizer::GetInstance()->RandomNumNegOneToOne()), beamVec);
		Vector3D rotatedVec = randomRotation * orthToBeamVec;
		
		Point3D& currPt = *iter;
		currPt = currPt + this->mainBeamAmplitude.MeanValueInInterval() * rotatedVec;
		currESPBeamSegment->SetEndPoint(currPt);

		currESPBeamSegment = currESPBeamSegment->AddESPBeamSegment();
		assert(currESPBeamSegment != NULL);
	}
	currESPBeamSegment->SetEndPoint(this->endPt);

	// Set the lifetime on the beam...
	newBeam->SetLifeTime(this->lifeTimeInSecs.MeanValueInInterval());
	
	// Add the new beam as an alive beam in this beam emitter
	this->aliveBeams.push_back(newBeam);
}
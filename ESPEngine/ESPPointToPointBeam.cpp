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
	this->SetBeamLifetime(ESPInterval(ESPBeam::INFINITE_BEAM_LIFETIME));
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
	for (std::list<ESPBeam*>::iterator iter = this->aliveBeams.begin(); iter != this->aliveBeams.end();) {
		ESPBeam* currBeam = *iter;
		currBeam->Tick(dT);
		if (currBeam->IsDead()) {
			delete currBeam;
			currBeam = NULL;
			iter = this->aliveBeams.erase(iter);
		}
		else {
			++iter;
		}
	}

	timeSinceLastBeamSpawn += dT;
}

void ESPPointToPointBeam::Draw(const Camera& camera) {
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, GL_FILL);

	glPushMatrix();
	glTranslatef(this->startPt[0], this->startPt[1], this->startPt[2]);
	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->colour.A());

	// Draw all of the alive beams
	for (std::list<ESPBeam*>::iterator iter = this->aliveBeams.begin(); iter != this->aliveBeams.end(); ++iter) {
		ESPBeam* currBeam = *iter;
		currBeam->Draw();
	}

	glPopMatrix();

	glPopAttrib();
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

	const Point3D ORIGIN_PT(0,0,0);

	for (int i = 0; i < numPts; i++) {
		maxSegmentLength = lengthLeftOver / (numPts - i);
		minSegmentLength = MIN_FRACTION * maxSegmentLength;
		minMaxSegLengthDiff = maxSegmentLength - minSegmentLength;

		currSegLength = minSegmentLength + Randomizer::GetInstance()->RandomNumZeroToOne() * minMaxSegLengthDiff;
		currSegDistFromStart = lengthLeftOver + currSegLength;
		assert(maxPtDist >= currSegDistFromStart);

		// Place the beam starting point at the origin instead - so that everything is in a more reasonable local
		// space position for transforming the beam to face the viewer
		beamMidPts.push_back(ORIGIN_PT + currSegDistFromStart * beamVec);
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
	currESPBeamSegment->SetEndPoint(this->endPt - Vector3D(this->startPt[0], this->startPt[1], this->startPt[2]));

	// Set the lifetime on the beam...
	newBeam->SetLifeTime(this->lifeTimeInSecs.MeanValueInInterval());
	
	// Add the new beam as an alive beam in this beam emitter
	this->aliveBeams.push_back(newBeam);
}
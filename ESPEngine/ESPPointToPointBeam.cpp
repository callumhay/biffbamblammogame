/**
 * ESPPointToPointBeam.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ESPPointToPointBeam.h"
#include "ESPBeam.h"
#include "ESPEffector.h"

ESPPointToPointBeam::ESPPointToPointBeam() : ESPAbstractEmitter(), 
startPt(0,0,0), endPt(0,0,0), colour(1,1,1,1), depthEnabled(true),/*texture(NULL),*/
numMainESPBeamSegments(10), numBeamsShot(0), timeSinceLastBeamSpawn(0.0), timeUntilNextBeamSpawn(0.0), 
mainBeamAmplitude(ESPInterval(0.0f)), mainBeamThickness(ESPInterval(1.0f)) {
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

    this->ClearEffectors();
}

void ESPPointToPointBeam::Tick(double dT) {
	// If we haven't shot all the beams yet and the time since the last beam spawn is greater than the time until
	// the next beam spawn then we make a new beam...
	if (this->ThereAreStillMoreBeamsToFire() && this->timeSinceLastBeamSpawn >= this->timeUntilNextBeamSpawn) {
		// Spawn a new beam
		this->SpawnBeam();
		this->numBeamsShot++;
	}

	// Go through each beam to check if it's life has expired yet
	for (std::list<ESPBeam*>::iterator iter = this->aliveBeams.begin(); iter != this->aliveBeams.end();) {
		ESPBeam* currBeam = *iter;
		

		if (currBeam->IsDead()) {
			delete currBeam;
			currBeam = NULL;
			iter = this->aliveBeams.erase(iter);
		}
		else {
            currBeam->Tick(dT);

            // Affect the beam with the various effectors...
            for (std::list<ESPEffector*>::const_iterator effectorIter = this->effectors.begin(); 
                 effectorIter != this->effectors.end(); ++effectorIter) {

                (*effectorIter)->AffectBeamOnTick(dT, currBeam);
            }

			++iter;
		}
	}

	timeSinceLastBeamSpawn += dT;
}

void ESPPointToPointBeam::Draw(const Camera& camera) {
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT);
	
    if (this->depthEnabled) {
        glEnable(GL_DEPTH_TEST);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glTranslatef(this->startPt[0], this->startPt[1], this->startPt[2]);
	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->colour.A());

	// Draw all of the alive beams
	for (std::list<ESPBeam*>::iterator iter = this->aliveBeams.begin(); iter != this->aliveBeams.end(); ++iter) {
		ESPBeam* currBeam = *iter;
		currBeam->Draw(this->startPt, this->endPt, camera);
	}

	glPopMatrix();
	glPopAttrib();

	debug_opengl_state();
}

void ESPPointToPointBeam::AddCopiedEffector(const ESPEffector& effector) {
    this->effectors.push_back(effector.Clone());
}

void ESPPointToPointBeam::ClearEffectors() {
    for (std::list<ESPEffector*>::iterator iter = this->effectors.begin(); iter != this->effectors.end(); ++iter) {
        ESPEffector* effector = *iter;
        delete effector;
        effector = NULL;
    }
    this->effectors.clear();
}

void ESPPointToPointBeam::SpawnBeam() {
	Vector3D beamVec = endPt - startPt;
	float beamLength = beamVec.length();
	beamVec.Normalize();

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

	const Point3D ORIGIN_PT(0,0,0);

	const float BEAM_LINE_DIST_VARIATION_FRACTION = 0.6f;
	float avgSegLength = beamLength / this->numMainESPBeamSegments;
	float fractionAvgSegLength = BEAM_LINE_DIST_VARIATION_FRACTION * avgSegLength;

	ESPBeam* newBeam = new ESPBeam(this->colour, beamVec, orthToBeamVec, 
        this->mainBeamAmplitude, ESPInterval(-fractionAvgSegLength, fractionAvgSegLength));
	ESPBeamSegment* currESPBeamSegment = newBeam->GetStartSegment();
	assert(currESPBeamSegment != NULL);
	
	for (int i = 0; i < static_cast<int>(this->numMainESPBeamSegments); i++) {
		currESPBeamSegment->SetDefaultPointOnLine(ORIGIN_PT + (i * avgSegLength) * beamVec);
		currESPBeamSegment = currESPBeamSegment->AddESPBeamSegment();
		assert(currESPBeamSegment != NULL);
	}
	currESPBeamSegment->SetDefaultPointOnLine(ORIGIN_PT + (this->numMainESPBeamSegments * avgSegLength) * beamVec);

	// Set the lifetime on the beam...
	newBeam->SetLifeTime(this->lifeTimeInSecs.RandomValueInInterval());
	newBeam->SetThickness(this->mainBeamThickness.RandomValueInInterval());
	
	// Add the new beam as an alive beam in this beam emitter
	this->aliveBeams.push_back(newBeam);
}
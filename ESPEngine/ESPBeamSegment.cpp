/**
 * ESPBeamSegment.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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
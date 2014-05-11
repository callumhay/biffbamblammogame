/**
 * ESPParticleRotateEffector.cpp
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

#include "ESPParticleRotateEffector.h"
#include "ESPParticle.h"

ESPParticleRotateEffector::ESPParticleRotateEffector(float rotationSpd, RotationDirection dir) : 
rotationSpd(rotationSpd), rotDir(dir), startRot(0), numRots(0), useRotationSpd(true) {
}

ESPParticleRotateEffector::ESPParticleRotateEffector(float initialRotation, float numRotations, RotationDirection dir) :
startRot(initialRotation), numRots(numRotations), rotDir(dir), rotationSpd(0), useRotationSpd(false) {
}

void ESPParticleRotateEffector::AffectParticleOnTick(double dT, ESPParticle* particle) {
	float particleLifespan = particle->GetLifespanLength();

	// Figure out how much to rotate based on the number of rotations before end of lifetime...
	// Do this if we're dealing with a discrete lifetime or if this rotate effector is specified to use the rotation speed
	if (particleLifespan != ESPParticle::INFINITE_PARTICLE_LIFETIME && !this->useRotationSpd) {

		float rotDegsPerSecond = this->rotDir * this->numRots * 360 / particleLifespan;	
		float currRotation = this->startRot + particle->GetCurrentLifeElapsed() * rotDegsPerSecond;

		particle->SetRotation(currRotation);
	}
	else {
		float currParticleRot = particle->GetRotation();
		particle->SetRotation(currParticleRot + this->rotDir*this->rotationSpd*dT);
	}
}

void ESPParticleRotateEffector::AffectBeamOnTick(double, ESPBeam*) {
    assert(false);
    // NOT IMPLEMENTED YET
}
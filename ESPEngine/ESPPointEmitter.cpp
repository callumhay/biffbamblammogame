/**
 * ESPPointEmitter.cpp
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

#include "ESPPointEmitter.h"

ESPPointEmitter::ESPPointEmitter() : ESPEmitter(), emitPt(Point3D(0, 0, 0)),
emitDir(Vector3D(0, 1, 0)),  emitAngleInRads(0.0f), emitOnPlane(false), planeNormal(0, 0, 1) {
}

ESPPointEmitter::~ESPPointEmitter() {
}

/**
 * Private helper function for calculating a vector in some allowable random
 * direction, along which a particle from this emitter will initially travel.
 * Returns: The random unit vector direction of a particle.
 */
Vector3D ESPPointEmitter::CalculateRandomInitParticleDir() const {
	// We start with the emit direction and transform it within the allowable
	// angle range for this emitter.
	if (this->emitOnPlane) {

		// Calculate a rotation matrix based on a random angle to emit at
		double randomAngleInRads = static_cast<double>(this->emitAngleInRads) * Randomizer::GetInstance()->RandomNumNegOneToOne();
		Matrix4x4 rotationMatrix = Matrix4x4::rotationMatrix(randomAngleInRads, this->planeNormal);

		Vector3D randomEmitDir = Vector3D::Normalize(rotationMatrix * this->emitDir);
		return randomEmitDir;
	}
	else {
		// Convert to spherical coordinates to do this...
		Vector3D sphCoordEmitDir = Vector3D::ToSphericalFromCartesian(this->emitDir);
		
		// Now just manipulate the theta and phi values based on the range (cone) of the emitter...
		double thetaVariation = this->emitAngleInRads * Randomizer::GetInstance()->RandomNumNegOneToOne();
		double phiVariation   = this->emitAngleInRads * Randomizer::GetInstance()->RandomNumNegOneToOne();
		sphCoordEmitDir[1] += thetaVariation;
		sphCoordEmitDir[2] += phiVariation;

		// Make sure the values are still in range:
		// (0 <= theta <= pi)

		if (sphCoordEmitDir[1] > static_cast<float>(M_PI)) {
			sphCoordEmitDir[1] = sphCoordEmitDir[1] - static_cast<float>(M_PI);
		}
		else if (sphCoordEmitDir[1] < 0) {
			sphCoordEmitDir[1] = sphCoordEmitDir[1] + static_cast<float>(M_PI);
		}
		// (0 <= phi < 2*pi)
		if (sphCoordEmitDir[2] >= static_cast<float>(2.0*M_PI)) {
			sphCoordEmitDir[2] = sphCoordEmitDir[2] - static_cast<float>(2.0*M_PI);
		}
		else if (sphCoordEmitDir[2] < 0) {
			sphCoordEmitDir[2] = sphCoordEmitDir[2] + static_cast<float>(2.0*M_PI);
		}

		// Convert back to cartesian Coordinates
		Vector3D resultVec = Vector3D::ToCartesianFromSpherical(sphCoordEmitDir);
		if (resultVec == Vector3D(0,0,0)) {
			resultVec = this->emitDir;
		}

		return Vector3D::Normalize(resultVec);
	}
}

/** 
 * Private helper function that calculates an initial position for a
 * particle in this emitter to spawn at.
 * Returns: Initial position for particle spawn.
 */
Point3D ESPPointEmitter::CalculateRandomInitParticlePos() const {
	float randomPtX = this->radiusDeviationFromPtX.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	float randomPtY = this->radiusDeviationFromPtY.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	float randomPtZ = this->radiusDeviationFromPtZ.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	Point3D initialPt = this->emitPt + Vector3D(randomPtX, randomPtY, randomPtZ);

	return initialPt;
}

// Sets all future emit positions and the position of all alive particles
void ESPPointEmitter::OverwriteEmittedPosition(const Point3D& pt) {
    this->SetEmitPosition(pt);
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); ++iter) {
		ESPParticle* currParticle = *iter;
        currParticle->SetPosition(pt);
	}
}

/**
 * Tell the emitter to only emit particles on the plane defined by the given plane normal,
 * or tell it to not do any such thing.
 */
void ESPPointEmitter::SetToggleEmitOnPlane(bool emitOnPlane, const Vector3D& planeNormal) {
	this->emitOnPlane = emitOnPlane;
	this->planeNormal = planeNormal;
	if (this->emitOnPlane && this->planeNormal == Vector3D(0,0,0)) {
		assert(false);
		this->planeNormal = Vector3D(0,0,1);
	}
	else {
		this->planeNormal.Normalize();
	}

	// Check to see if the directional vector for emitting is perpendicular to the plane... if it is
	// then we must change it...
	if (this->emitOnPlane) {
		assert(Vector3D::cross(this->planeNormal, this->emitDir) != Vector3D(0,0,0));
	}
}

/**
 * Set the emit angle (in degrees) for this point emitter.
 */
void ESPPointEmitter::SetEmitAngleInDegrees(int degs) {
	if (degs > 180) {
		degs %= 180;
	}
	if (degs < 0) {
		degs += 180;
	}
	this->emitAngleInRads = Trig::degreesToRadians(degs);
}
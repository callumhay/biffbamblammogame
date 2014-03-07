/**
 * ESPVolumeEmitter.cpp
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

#include "ESPVolumeEmitter.h"

// Emit Volume Functions
// ************************************************************************
//Point3D EmitOval::GetRandomPointInArea() const {
//	// Pick a random angle [0, 360) 
//	ESPInterval angleInterval(0, 359.99f);
//	float randAngle = angleInterval.RandomValueInInterval();
//
//	Point3D randPoint = 
//	return randPoint;
//}

/** 
 * Obtain a random location in the area defined by this AABB.
 */
Point3D ESPVolumeEmitter::EmitAABB::GetRandomPointInVolume() const {
	ESPInterval xInterval(this->min[0], this->max[0]);
	ESPInterval yInterval(this->min[1], this->max[1]);
	ESPInterval zInterval(this->min[2], this->max[2]);

	return Point3D(xInterval.RandomValueInInterval(), yInterval.RandomValueInInterval(), zInterval.RandomValueInInterval());
}

// ************************************************************************

ESPVolumeEmitter::ESPVolumeEmitter() : emitDir(0, 1, 0), emitVolume(NULL) {
}

ESPVolumeEmitter::~ESPVolumeEmitter() {
	if (this->emitVolume != NULL) {
		delete this->emitVolume;
		this->emitVolume = NULL;
	}
}

Vector3D ESPVolumeEmitter::CalculateRandomInitParticleDir() const {
	// For now this is just returning the basic direction
	return this->emitDir;
}

Point3D ESPVolumeEmitter::CalculateRandomInitParticlePos() const {
	float randomPtX = this->radiusDeviationFromPtX.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	float randomPtY = this->radiusDeviationFromPtY.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	float randomPtZ = this->radiusDeviationFromPtZ.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	Vector3D initialDisp(randomPtX, randomPtY, randomPtZ);

	// If there is not emit volume then the emitter condenses into a point
	if (this->emitVolume == NULL) {
		assert(false);
		return Point3D(0,0,0) + initialDisp;
	}

	// Return a random point inside the emit volume with any random displacement that has been set
	Point3D randomPt = this->emitVolume->GetRandomPointInVolume() + initialDisp;
	return randomPt;		
}

/**
 * --------------
 * |     ^      |
 * |     |      |
 * |     *----->|
 * |            |
 * --------------
 * Defines an emit area rectangle with center (*) and the 
 * half vertical and half horizontal vectors.
 */
/*
void ESPAreaEmitter::SetEmitVolume(const Point3D& center, const Vector3D& rectHalfVert, const Vector3D& rectHalfHoriz) {
	// Clean any previous area
	if (this->emitVolume != NULL) {
		delete this->emitVolume;
	}

	// TODO
}
*/

/**
 *		 -------------* max
 *		 |            |
 *		 |            |
 * min *-------------
 * Defines an emit area rectangle with a minimum and maximum point.
 */
void ESPVolumeEmitter::SetEmitVolume(const Point3D& min, const Point3D& max) {
	// Clean any previous area
	if (this->emitVolume != NULL) {
		delete this->emitVolume;
	}

	assert(min <= max);

	// Setup a new emit area rectangle with the given parameters
	ESPVolumeEmitter::EmitAABB* emitAABB = new ESPVolumeEmitter::EmitAABB();
	emitAABB->min = min;
	emitAABB->max = max;
	
	this->emitVolume = emitAABB;
}
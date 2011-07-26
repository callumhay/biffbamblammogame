/**
 * ESPVolumeEmitter.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
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
 * Set the emit direction for this area emitter.
 */
void ESPVolumeEmitter::SetEmitDirection(const Vector3D& dir) {
	this->emitDir = dir;
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
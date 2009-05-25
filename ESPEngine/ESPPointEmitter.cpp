#include "ESPPointEmitter.h"


ESPPointEmitter::ESPPointEmitter() : ESPEmitter(), emitPt(Point3D(0, 0, 0)),
emitDir(Vector3D(0, 1, 0)),  emitAngleInRads(0.0f) {
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

	// Convert to spherical coordinates to do this...
	Vector3D sphCoordEmitDir = Vector3D::ToSphericalFromCartesian(this->emitDir);
	
	// Now just manipulate the theta and phi values based on the range (cone) of the emitter...
	double thetaVariation = this->emitAngleInRads * Randomizer::GetInstance()->RandomNumNegOneToOne();
	double phiVariation		= this->emitAngleInRads * Randomizer::GetInstance()->RandomNumNegOneToOne();
	sphCoordEmitDir[1] += thetaVariation;
	sphCoordEmitDir[2] += phiVariation;

	// Make sure the values are still in range:
	// (0 <= theta <= pi)

	if (sphCoordEmitDir[1] > static_cast<float>(M_PI)) {
		sphCoordEmitDir[1] = sphCoordEmitDir[1] - static_cast<float>(M_PI);
	}
	else if (sphCoordEmitDir[1] < 0) {
		sphCoordEmitDir[1] = -sphCoordEmitDir[1];
	}
	// (0 <= phi < 2*pi)
	if (sphCoordEmitDir[2] >= static_cast<float>(2.0*M_PI)) {
		sphCoordEmitDir[1] = sphCoordEmitDir[1] - static_cast<float>(2.0*M_PI);
	}
	else if (sphCoordEmitDir[2] < 0) {
		sphCoordEmitDir[1] = -sphCoordEmitDir[1];// + static_cast<float>(2.0*M_PI);
	}

	// Convert back to cartesian Coordinates
	Vector3D resultVec = Vector3D::ToCartesianFromSpherical(sphCoordEmitDir);
	return Vector3D::Normalize(resultVec);
}

/** 
 * Private helper function that calculates an initial position for a
 * particle in this emitter to spawn at.
 * Returns: Initial position for particle spawn.
 */
Point3D ESPPointEmitter::CalculateRandomInitParticlePos() const {
	float randomPtX = this->radiusDeviationFromPt.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	float randomPtY = this->radiusDeviationFromPt.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	float randomPtZ = this->radiusDeviationFromPt.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	Point3D initialPt = this->emitPt + Vector3D(randomPtX, randomPtY, randomPtZ);

	return initialPt;
}

/**
 * Set the center emit point for this point emitter.
 */
void ESPPointEmitter::SetEmitPosition(const Point3D& pt) {
	this->emitPt = pt;
}

/**
 * Set the emit direction for this point emitter.
 */
void ESPPointEmitter::SetEmitDirection(const Vector3D& dir) {
	this->emitDir = dir;
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
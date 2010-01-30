#include "ESPLineEmitter.h"

#include "../BlammoEngine/Algebra.h"

ESPLineEmitter::ESPLineEmitter() : ESPEmitter() {
	this->SetEmitLine(Collision::LineSeg3D(Point3D(0, 0, 0), Point3D(0, 1, 0)));
	this->SetEmitAngle(180);
	this->SetEmitDirection(Vector3D(0, 0, 1));
}

ESPLineEmitter::~ESPLineEmitter() {
}

// Set the line along which particles are emitted
void ESPLineEmitter::SetEmitLine(const Collision::LineSeg3D& line) {
	assert(line.P1() != line.P2());
	this->emitLine = line;
}

// Set the direction (must be perpendicular to the current line) along which particles
// will emit from with variation of the emit angle.
void ESPLineEmitter::SetEmitDirection(const Vector3D& dir) {
	// The direction better be perpendicular to the line and non-zero!
	assert(dir != Vector3D(0, 0, 0));
	assert(fabs(Vector3D::Dot(dir, this->emitLine.GetUnitDirection())) < EPSILON);

	this->emitDir = dir;
}

// The varation from the given emit direction
void ESPLineEmitter::SetEmitAngle(float angleInDegrees) {
	assert(angleInDegrees >= 0 && angleInDegrees <= 180);
	this->emitAngle = angleInDegrees;
}

// Generate a random direction off the line based on the emit direction and
// emit angle...
Vector3D ESPLineEmitter::CalculateRandomInitParticleDir() const {
	// Rotate the current emit direction along the line some random number of degrees between
	// zero and the emitAngle
	float randomAngleInDegs = Randomizer::GetInstance()->RandomNumNegOneToOne() * this->emitAngle;
	float randomAngleInRads = Trig::degreesToRadians(randomAngleInDegs);
	Matrix4x4 rotMat = Matrix4x4::rotationMatrix(randomAngleInRads, this->emitLine.GetUnitDirection());

	return rotMat * this->emitDir;
}

// Generate a random initial particle position along the line, taking into account
// any completely random offsets...
Point3D  ESPLineEmitter::CalculateRandomInitParticlePos() const {
	// Find a random point along the line...
	Point3D  startPt = this->emitLine.P1();
	Point3D  endPt   = this->emitLine.P2();
	Vector3D endToEndVec = endPt - startPt;
	Point3D randomPointOnLine = startPt + Randomizer::GetInstance()->RandomNumZeroToOne() * endToEndVec;
	
	// Now vary that point by the random offset of the emitter...
	float randomPtX = this->radiusDeviationFromPt.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	float randomPtY = this->radiusDeviationFromPt.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	float randomPtZ = this->radiusDeviationFromPt.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	Vector3D randomDisplacement(randomPtX, randomPtY, randomPtZ);

	return randomPointOnLine + randomDisplacement;
}
/**
 * ESPLineEmitter.cpp
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
	float randomPtX = this->radiusDeviationFromPtX.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	float randomPtY = this->radiusDeviationFromPtY.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	float randomPtZ = this->radiusDeviationFromPtZ.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	Vector3D randomDisplacement(randomPtX, randomPtY, randomPtZ);

	return randomPointOnLine + randomDisplacement;
}
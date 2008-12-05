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

	// These MUST be true since the emitAngleInRads cannot be greater than pi
	assert(sphCoordEmitDir[1] <= 2*M_PI && sphCoordEmitDir[1] >= -M_PI);
	assert(sphCoordEmitDir[2] < 3*M_PI && sphCoordEmitDir[2] >= -M_PI);

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
 * Public function, called each frame to execute the emitter.
 */
void ESPPointEmitter::Tick(const double dT) {
	this->timeSinceLastSpawn += dT;

	// Figure out if we can spawn a particle by bring it back from the dead (zombie particle... of doom)
	float allowableTimeToSpawn = this->particleSpawnDelta.RandomValueInInterval();
	if (this->timeSinceLastSpawn >= allowableTimeToSpawn && this->deadParticles.size() > 0) {
		// Let's spawn a particle!
		// Figure out the properties we need to impart on the newly born particle...
		Vector3D initalParticleVel =  this->particleInitialSpd.RandomValueInInterval() * this->CalculateRandomInitParticleDir();
		float randomPtX = this->radiusDeviationFromPt.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
		float randomPtY = this->radiusDeviationFromPt.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
		float randomPtZ = this->radiusDeviationFromPt.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
		Point3D initalPt = this->emitPt + Vector3D(randomPtX, randomPtY, randomPtZ);
		
		// Remove a dead particle from the set of dead particles...
		ESPParticle* zombie = this->deadParticles.front();
		assert(zombie != NULL);
		this->deadParticles.pop_front();

		// Revive the particle and put it in the group of living particles
		zombie->Revive(initalPt, initalParticleVel, this->particleSize.RandomValueInInterval(), this->particleLifetime.RandomValueInInterval()); 
		this->aliveParticles.push_back(zombie);

		this->timeSinceLastSpawn = 0.0f;
	}

	// Go through the alive iterators and figure out which ones have died and tick those that are still alive
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); iter++) {
		// Give the particle time to do its thing...
		ESPParticle* currParticle = *iter;

		// Check to see if the particle has died, if so place it among the dead
		if (currParticle->IsDead()) {
			iter = this->aliveParticles.erase(iter);
			this->deadParticles.push_back(currParticle);
		}
		else {
			currParticle->Tick(dT);

			// Have each of the effectors in this emitter affect the particle...
			for (std::list<ESPParticleEffector*>::iterator effIter = this->effectors.begin(); effIter != this->effectors.end(); effIter++) {
				(*effIter)->AffectParticleOnTick(dT, currParticle);
			}
		}
	}
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

/**
 * Set the deviation from the center emit point where particles may spawn.
 */
void ESPPointEmitter::SetRadiusDeviationFromCenter(const ESPInterval& distFromCenter) {
	this->radiusDeviationFromPt = distFromCenter;
}


/**
 * Draw this emitter.
 */
void ESPPointEmitter::Draw(const Camera& camera) {
	// Setup OpenGL for drawing the particles in this emitter...
	glPushAttrib(GL_VIEWPORT_BIT | GL_TEXTURE_BIT | GL_LIGHTING_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT  | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 	
	
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	// Each particle needs to be transformed so that it adheres to
	// how it faces the viewer (view-plane-aligned or view-point-aligned) 
	glPushMatrix();

	Matrix4x4 allParticleAlignXF = ESPParticle::GetAlignmentTransform(camera, this->particleAlignment);
	glMultMatrixf(allParticleAlignXF.begin());
	
	// Bind the particle textures
	this->particleTexture->BindTexture();

	// Go through each of the particles, revive any dead ones (based on spawn rate), and draw them
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); iter++) {
		ESPParticle* currParticle = *iter;	
		currParticle->Draw(camera, this->particleAlignment);
	}
	
	glPopMatrix();
	glPopAttrib();
}
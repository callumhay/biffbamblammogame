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
 * Private helper function for reviving a particle.
 */
void ESPPointEmitter::ReviveParticle() {
	// Let's spawn a particle!
	// Figure out the properties we need to impart on the newly born particle...
	Vector3D initalParticleVel(0,0,0);
	float initialSpd = this->particleInitialSpd.RandomValueInInterval();
	if (initialSpd != 0.0f) {
		initalParticleVel = initialSpd * this->CalculateRandomInitParticleDir();
	}

	float randomPtX = this->radiusDeviationFromPt.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	float randomPtY = this->radiusDeviationFromPt.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	float randomPtZ = this->radiusDeviationFromPt.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	Point3D initalPt = this->emitPt + Vector3D(randomPtX, randomPtY, randomPtZ);
	
	// Remove a dead particle from the set of dead particles...
	ESPParticle* zombie = this->deadParticles.front();
	assert(zombie != NULL);
	this->deadParticles.pop_front();

	// Revive the particle and put it in the group of living particles
	Vector2D randomSizing(this->particleSize[0].RandomValueInInterval(), this->particleSize[1].RandomValueInInterval());
	if (this->makeSizeConstraintsEqual) {
		randomSizing[1] = randomSizing[0];
	}

	zombie->Revive(initalPt, initalParticleVel, randomSizing, this->particleRotation.RandomValueInInterval(), this->particleLifetime.RandomValueInInterval()); 
	zombie->SetColour(Colour(this->particleRed.RandomValueInInterval(), this->particleGreen.RandomValueInInterval(), 
		this->particleBlue.RandomValueInInterval()), this->particleAlpha.RandomValueInInterval());
	this->AssignRandomTextureToParticle(zombie);

	this->aliveParticles.push_back(zombie);
}

/**
 * Private helper function for ticking living particles and managing
 * the movement between alive and dead particles.
 */
void ESPPointEmitter::TickParticles(double dT) {
	// Go through the alive iterators and figure out which ones have died and tick those that are still alive
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); iter++) {
		// Give the particle time to do its thing...
		ESPParticle* currParticle = *iter;

		// Check to see if the particle has died, if so place it among the dead
		if (currParticle->IsDead()) {
			// Strange issue with erase when there's only 1 particle in the alive list...
			if (this->aliveParticles.size() == 1) {
				this->aliveParticles.clear();
				this->deadParticles.push_back(currParticle);
				break;
			}
			else {
				iter = this->aliveParticles.erase(iter);
				this->deadParticles.push_back(currParticle);
			}
			
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
 * Public function, called each frame to execute the emitter.
 */
void ESPPointEmitter::Tick(const double dT) {
	// Check for the special case of a single lifetime (interval values are -1)
	if (this->particleSpawnDelta.minValue == -1) {
		// Inline: Particles only have a single life time and are spawned immediately
		
		// We initialize all particles to living on the first run though
		if (timeSinceLastSpawn == 0.0f) {
			while(this->deadParticles.size() > 0) {
				this->ReviveParticle();
			}
		}

		this->TickParticles(dT);
		this->timeSinceLastSpawn += dT;
	}
	else {
		// Inline: there is a variable respawn time

		// Figure out if we can spawn a particle by bring it back from the dead (zombie particle... of doom)
		float allowableTimeToSpawn = this->particleSpawnDelta.RandomValueInInterval();
		if (this->timeSinceLastSpawn >= allowableTimeToSpawn && this->deadParticles.size() > 0) {
			// Let's spawn a particle!
			this->ReviveParticle();
			this->timeSinceLastSpawn = 0.0f;
		}
		else {
			this->timeSinceLastSpawn += dT;
		}

		this->TickParticles(dT);
	}
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
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	// Go through each of the particles, revive any dead ones (based on spawn rate), and draw them
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); iter++) {
		ESPParticle* currParticle = *iter;
		std::map<ESPParticle*, Texture2D*>::iterator findIter = this->textureAssignments.find(currParticle);
		if (findIter != this->textureAssignments.end()) {
			findIter->second->BindTexture();
		}
		currParticle->Draw(camera, this->particleAlignment);
	}
	
	glPopAttrib();
}
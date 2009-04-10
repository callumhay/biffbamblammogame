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
	// Remove a dead particle from the set of dead particles...
	ESPParticle* zombie = this->deadParticles.front();
	assert(zombie != NULL);
	
	// Check to see if the particle has any lives left
	if (this->particleLivesLeft[zombie] == 0) {
		// No lives left for the particle, therefore it gets no respawn.
		return;
	}
	
	this->deadParticles.pop_front();

	// Figure out the properties we need to impart on the newly born particle...
	Vector3D initialParticleVel(0,0,0);
	float initialSpd = this->particleInitialSpd.RandomValueInInterval();
	if (initialSpd != 0.0f) {
		initialParticleVel = initialSpd * this->CalculateRandomInitParticleDir();
	}

	float randomPtX = this->radiusDeviationFromPt.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	float randomPtY = this->radiusDeviationFromPt.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	float randomPtZ = this->radiusDeviationFromPt.RandomValueInInterval() * Randomizer::GetInstance()->RandomNegativeOrPositive();
	Point3D initialPt = this->emitPt + Vector3D(randomPtX, randomPtY, randomPtZ);

	// Revive the particle and put it in the group of living particles
	Vector2D randomSizing(this->particleSize[0].RandomValueInInterval(), this->particleSize[1].RandomValueInInterval());
	if (this->makeSizeConstraintsEqual) {
		randomSizing[1] = randomSizing[0];
	}

	// If the emitter is reversed then we must change the revive parameters for the particle
	float randomLifetime = this->particleLifetime.RandomValueInInterval();
	float randomRotation = this->particleRotation.RandomValueInInterval();
	if (this->isReversed) {
		// Figure out how far the particle will travel in its lifetime
		Vector3D distVecDuringLife = randomLifetime * initialParticleVel;
		initialPt = initialPt + distVecDuringLife;
		
		// Reverse its velocity
		initialParticleVel = -initialParticleVel;

		zombie->Revive(initialPt, initialParticleVel, randomSizing, randomRotation, randomLifetime); 
	}
	else {
		zombie->Revive(initialPt, initialParticleVel, randomSizing, randomRotation, randomLifetime); 
	}
	
	zombie->SetColour(Colour(this->particleRed.RandomValueInInterval(), this->particleGreen.RandomValueInInterval(), 
		this->particleBlue.RandomValueInInterval()), this->particleAlpha.RandomValueInInterval());

	this->aliveParticles.push_back(zombie);

	// Subtract a life from the particle if the lives are not infinite
	int& livesLeftForRevivedParticle = this->particleLivesLeft[zombie];
	if (livesLeftForRevivedParticle != ESPParticle::INFINITE_PARTICLE_LIVES) {
		assert(livesLeftForRevivedParticle != 0);
		livesLeftForRevivedParticle--;
	}

	this->timeSinceLastSpawn = 0.0f;
}

/**
 * Private helper function for ticking living particles and managing
 * the movement between alive and dead particles.
 */
void ESPPointEmitter::TickParticles(double dT) {
	std::list<std::list<ESPParticle*>::iterator> nowDead;

	// Go through the alive iterators and figure out which ones have died and tick those that are still alive
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); iter++) {
		// Give the particle time to do its thing...
		ESPParticle* currParticle = *iter;
		std::list<ESPParticle*>::iterator tempIter = iter;

		// Check to see if the particle has died, if so place it among the dead
		if (currParticle->IsDead()) {
			nowDead.push_back(iter);
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

	for (std::list<std::list<ESPParticle*>::iterator>::iterator iter = nowDead.begin(); iter != nowDead.end(); iter++) {
		this->aliveParticles.erase(*iter);	
	}
}

/**
 * Public function, called each frame to execute the emitter.
 */
void ESPPointEmitter::Tick(const double dT) {
	// Check for the special case of a single lifetime
	if (this->OnlySpawnsOnce()) {
		// Inline: Particles only have a single life time and are spawned immediately
		
		// We initialize all particles to living on the first run though
		if (timeSinceLastSpawn == 0.0f) {
			while(this->deadParticles.size() > 0 && this->numParticleLives != 0) {
				this->ReviveParticle();
			}
		}
		this->timeSinceLastSpawn += dT;
		this->TickParticles(dT);

	}
	else {
		// Inline: there is a variable respawn time

		// Figure out if we can spawn a particle by bring it back from the dead (zombie particle... of doom)
		float allowableTimeToSpawn = this->particleSpawnDelta.RandomValueInInterval();
		if (this->timeSinceLastSpawn >= allowableTimeToSpawn && this->deadParticles.size() > 0) {
			// Let's spawn a particle!
			this->ReviveParticle();
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
	glPushAttrib(GL_VIEWPORT_BIT | GL_TEXTURE_BIT | GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | 
		GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_MULTISAMPLE_BIT | GL_POINT_BIT); 	
	
	glDisable(GL_MULTISAMPLE);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	// Go through each of the particles, revive any dead ones (based on spawn rate), and draw them
	if (this->particleTexture != NULL) {
		this->particleTexture->BindTexture();
	}

	if (this->isPointSprite && GLEW_ARB_point_sprite) {
		
		// Draw things faster if we're doing point sprites
		glEnable(GL_POINT_SPRITE);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
		glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);

		for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); iter++) {
			ESPParticle* currParticle = *iter;
			currParticle->DrawAsPointSprite(camera);
		}
		glDisable(GL_POINT_SPRITE);
		debug_opengl_state();
	}
	else {
		for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); iter++) {
			ESPParticle* currParticle = *iter;
			currParticle->Draw(camera, this->particleAlignment);
		}
	}

	if (this->particleTexture != NULL) {
		this->particleTexture->UnbindTexture();
	}
	
	glPopAttrib();
}
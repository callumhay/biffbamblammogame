/**
 * ESPEmitter.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "ESPEmitter.h"
#include "ESPShaderParticle.h"
#include "ESPOnomataParticle.h"

#include "../BlammoEngine/TextLabel.h"

ESPEmitter::ESPEmitter() : timeSinceLastSpawn(0.0f), particleTexture(NULL),
particleAlignment(ESP::ViewPointAligned), particleRed(1), particleGreen(1), particleBlue(1), particleAlpha(1),
particleRotation(0), makeSizeConstraintsEqual(true), numParticleLives(ESPParticle::INFINITE_PARTICLE_LIVES),
isReversed(false), isPointSprite(false) {
}

ESPEmitter::~ESPEmitter() {
	this->Flush();
	
	// Clear the effectors
	this->effectors.clear();
}

/**
 * Private helper function for emptying this emitter of all particles and
 * other heap information.
 */
void ESPEmitter::Flush() {
	assert(this->aliveParticles.size() + this->deadParticles.size() == this->particleLivesLeft.size());
	// Delete particles
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); iter++) {
		ESPParticle* currParticle = *iter;
		delete currParticle;
		currParticle = NULL;
	}
	this->aliveParticles.clear();

	for (std::list<ESPParticle*>::iterator iter = this->deadParticles.begin(); iter != this->deadParticles.end(); iter++) {
		ESPParticle* currParticle = *iter;
		delete currParticle;
		currParticle = NULL;
	}
	this->deadParticles.clear();

	// Clear other relevant particle information
	this->particleLivesLeft.clear();

	// Reset appropriate variables
	this->timeSinceLastSpawn = 0.0f;
	this->particleTexture = NULL;
}

/**
 * Private helper function for reviving a particle.
 */
void ESPEmitter::ReviveParticle() {
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

	Point3D initialPt = this->CalculateRandomInitParticlePos();

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
void ESPEmitter::TickParticles(double dT) {
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
void ESPEmitter::Tick(const double dT) {
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
 * Draw this emitter.
 */
void ESPEmitter::Draw(const Camera& camera, bool enableDepth) {
	// Setup OpenGL for drawing the particles in this emitter...
	glPushAttrib(GL_VIEWPORT_BIT | GL_TEXTURE_BIT | GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | 
		GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_MULTISAMPLE_BIT | GL_POINT_BIT); 	
	
	glDisable(GL_MULTISAMPLE);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	enableDepth ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glPolygonMode(GL_FRONT, GL_FILL);

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

/**
 * Public function for setting the particles for this emitter.
 * Returns: true on success, false otherwise.
 */
bool ESPEmitter::SetParticles(unsigned int numParticles, Texture2D* texture) {
	assert(numParticles > 0);
	// Clean up all previous emitter data
	this->Flush();

	assert(texture != NULL);
	this->particleTexture = texture;

	// Create each of the new particles
	for (unsigned int i = 0; i < numParticles; i++) {
		ESPParticle* newParticle = new ESPParticle();
		this->deadParticles.push_back(newParticle);

		// Assign the number of lives...
		this->particleLivesLeft[newParticle] = this->numParticleLives;
	}

	return true;
}

/**
 * Public function for setting particles for this emitter such that they are
 * all shader particles of the given effect.
 * Returns: true on success, false otherwise.
 */
bool ESPEmitter::SetParticles(unsigned int numParticles, CgFxEffectBase* effect) {
	assert(numParticles > 0);
	// Clean up previous emitter data
	this->Flush();
	
	// Create each of the new particles
	for (unsigned int i = 0; i < numParticles; i++) {
		ESPShaderParticle* newParticle = new ESPShaderParticle(effect);
		this->deadParticles.push_back(newParticle);

		// Assign the number of lives...
		this->particleLivesLeft[newParticle] = this->numParticleLives;
	}

	return true;
}

/**
 * Public function for setting particles for this emitter such that they are
 * all of a certain text label.
 */
bool ESPEmitter::SetParticles(unsigned int numParticles, const TextLabel2D& text) {
	assert(numParticles > 0);
	// Clean up previous emitter data
	this->Flush();

	// Create each of the new particles
	for (unsigned int i = 0; i < numParticles; i++) {
		// Initialize the particle and its attributes
		ESPOnomataParticle* newParticle = new ESPOnomataParticle(text.GetFont(), text.GetText());
		newParticle->SetDropShadow(text.GetDropShadow());

		this->deadParticles.push_back(newParticle);

		// Assign the number of lives...
		this->particleLivesLeft[newParticle] = this->numParticleLives;
	}

	return true;
}

/**
 * Public function for setting particles for this emitter such that they are
 * all onomatopoeia particles of the given text label and qualifications.
 * Returns: true on success, false otherwise.
 */
bool ESPEmitter::SetParticles(unsigned int numParticles, const TextLabel2D& text, Onomatoplex::SoundType st, Onomatoplex::Extremeness e) {
	assert(numParticles > 0);
	// Clean up previous emitter data
	this->Flush();
	
	// Create each of the new particles
	for (unsigned int i = 0; i < numParticles; i++) {
		// Initialize the particle and its attributes
		ESPOnomataParticle* newParticle = new ESPOnomataParticle(text.GetFont());
		newParticle->SetDropShadow(text.GetDropShadow());
		newParticle->SetOnomatoplexSound(st, e);

		this->deadParticles.push_back(newParticle);

		// Assign the number of lives...
		this->particleLivesLeft[newParticle] = this->numParticleLives;
	}

	return true;
}

/**
 * Sets the particle alignments for this emitter.
 */
void  ESPEmitter::SetParticleAlignment(const ESP::ESPAlignment alignment) {
	this->particleAlignment = alignment;
}

/**
 * Sets the inclusive interval of random possible values that represent the 
 * delay (in seconds) between the spawning of new particles for this emitter.
 */
void ESPEmitter::SetSpawnDelta(const ESPInterval& spawnDelta) {
	this->particleSpawnDelta = spawnDelta;
}

/**
 * Sets the inclusive interval of random possible values that represent the
 * initial speed of particles as they are launched out of this emitter.
 */
void ESPEmitter::SetInitialSpd(const ESPInterval& initialSpd) {
	this->particleInitialSpd = initialSpd;
}

/**
 * Sets the inclusive interval of random possible values that represent the
 * lifetime (in seconds) of particles in this emitter.
 */
void ESPEmitter::SetParticleLife(const ESPInterval& particleLife) {
	this->particleLifetime = particleLife;
}

/**
 * Sets the inclusive interval of random possible values that represent the
 * size (in units) of particles in this emitter.
 */
void ESPEmitter::SetParticleSize(const ESPInterval& particleSizeX, const ESPInterval& particleSizeY) {
	this->particleSize[0] = particleSizeX;
	this->particleSize[1] = particleSizeY;
	this->makeSizeConstraintsEqual = false;
}
void ESPEmitter::SetParticleSize(const ESPInterval& particleSize) {
	this->particleSize[0] = particleSize;
	this->particleSize[1] = particleSize;
	this->makeSizeConstraintsEqual = true;
}

/**
 * Sets the inclusive interval of random possible values that represent the initial
 * rotation (in degrees) of particles in this emitter.
 */
void ESPEmitter::SetParticleRotation(const ESPInterval& particleRot) {
	this->particleRotation = particleRot;
}

/**
 * Sets the inclusive intervals of random possible values for the colour and alpha of particles.
 */
void ESPEmitter::SetParticleColour(const ESPInterval& red, const ESPInterval& green, const ESPInterval& blue, const ESPInterval& alpha) {
	this->particleRed = red;
	this->particleGreen = green;
	this->particleBlue = blue;
	this->particleAlpha = alpha;
}

void ESPEmitter::SetNumParticleLives(int lives) {
	assert(lives >= ESPParticle::INFINITE_PARTICLE_LIVES);
	
	this->numParticleLives = lives;

	// Go through any already assigned particles and set the lives...
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); iter++) {
		ESPParticle* currParticle = *iter;
		this->particleLivesLeft[currParticle] = lives;
	}
	for (std::list<ESPParticle*>::iterator iter = this->deadParticles.begin(); iter != this->deadParticles.end(); iter++) {
		ESPParticle* currParticle = *iter;
		this->particleLivesLeft[currParticle] = lives;
	}
}

/**
 * Sets whether this emitter is reveresed (i.e., particles will be birthed where they usually
 * would die and die where they usually would be birthed).
 */
void ESPEmitter::SetIsReversed(bool isReversed) {
	this->isReversed = isReversed;
}

/**
 * Sets whether this emitter emits point sprites or not.
 */
void ESPEmitter::SetAsPointSpriteEmitter(bool isPointSprite) {
	this->isPointSprite = isPointSprite;
}

/**
 * Set the deviation from the center emit point where particles may spawn.
 */
void ESPEmitter::SetRadiusDeviationFromCenter(const ESPInterval& distFromCenter) {
	this->radiusDeviationFromPt = distFromCenter;
}


/**
 * Adds a particle effector to this emitter.
 */
void ESPEmitter::AddEffector(ESPParticleEffector* effector) {
	assert(effector != NULL);
	this->effectors.push_back(effector);
}

// TODO: Get rid of this function, replace with set particle for onomatopiea effects!!!
void ESPEmitter::AddParticle(ESPParticle* particle) {
	assert(particle != NULL);
	this->deadParticles.push_back(particle);
	
	// Assign the number of lives...
	this->particleLivesLeft[particle] = this->numParticleLives;
}

/**
 * Removes a previously added particle effector from this emitter.
 */
void ESPEmitter::RemoveEffector(ESPParticleEffector* const effector) {
	assert(effector != NULL);
	this->effectors.remove(effector);
}

/**
 * Reset this emitter to before any ticks were executed on it.
 */
void ESPEmitter::Reset() {
	// Move all alive particles into the pool of dead particles (kill any ones still alive)
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); iter++) {
		ESPParticle* currParticle = *iter;
		currParticle->Kill();
		this->deadParticles.push_back(currParticle);
	}
	this->aliveParticles.clear();

	// Reset time...
	this->timeSinceLastSpawn = 0.0f;
}
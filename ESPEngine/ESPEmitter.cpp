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
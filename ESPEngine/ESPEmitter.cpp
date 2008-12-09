#include "ESPEmitter.h"

ESPEmitter::ESPEmitter() : particleTexture(NULL), timeSinceLastSpawn(0.0f), 
particleAlignment(ESP::ViewPointAligned) {
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

	// Delete particle textures
	delete this->particleTexture;
	this->particleTexture = NULL;

	// Reset appropriate variables
	this->timeSinceLastSpawn = 0.0f;
}


/**
 * Public function for setting the particles for this emitter.
 * Returns: true on success, false otherwise.
 */
bool ESPEmitter::SetParticles(unsigned int numParticles, const std::string& imgFilepath) {
	// Clean up all previous emitter data
	this->Flush();

	// TODO: fix texture filtering...
	this->particleTexture = Texture2D::CreateTexture2DFromImgFile(imgFilepath, Texture::Trilinear);
	if (this->particleTexture == NULL) {
		return false;
	}

	// Create each of the new particles
	for (unsigned int i = 0; i < numParticles; i++) {
		this->deadParticles.push_back(new ESPParticle());
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
void ESPEmitter::SetParticleSize(const ESPInterval& particleSize) {
	this->particleSize = particleSize;
}

/**
 * Adds a particle effector to this emitter.
 */
void ESPEmitter::AddEffector(ESPParticleEffector* effector) {
	assert(effector != NULL);
	this->effectors.push_back(effector);
}

void ESPEmitter::AddParticle(ESPParticle* particle) {
	assert(particle != NULL);
	this->deadParticles.push_back(particle);
}

/**
 * Removes a previously added particle effector from this emitter.
 */
void ESPEmitter::RemoveEffector(ESPParticleEffector* const effector) {
	assert(effector != NULL);
	this->effectors.remove(effector);
}
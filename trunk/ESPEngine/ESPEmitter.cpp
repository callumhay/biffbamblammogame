#include "ESPEmitter.h"

ESPEmitter::ESPEmitter() : timeSinceLastSpawn(0.0f), //particleTexture(NULL),
particleAlignment(ESP::ViewPointAligned), particleRed(1), particleGreen(1), particleBlue(1), particleAlpha(1),
makeSizeConstraintsEqual(true) {
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

	// Set texture to NULL
	//this->particleTexture = NULL;
	this->particleTextures.clear();
	this->textureAssignments.clear();

	// Reset appropriate variables
	this->timeSinceLastSpawn = 0.0f;
}

/**
 * Private helper function for assigning a random texture to a particle.
 */
void ESPEmitter::AssignRandomTextureToParticle(ESPParticle* particle) {
	assert(particle != NULL);

	// If there are no textures then just exit
	if (this->particleTextures.size() == 0) {
		return;
	}
	
	// Choose a random texture index...
	unsigned int randomTexIndex = Randomizer::GetInstance()->RandomUnsignedInt() % this->particleTextures.size();
	this->textureAssignments[particle] = this->particleTextures[randomTexIndex];
	assert(this->textureAssignments[particle] != NULL);
}

/**
 * Public function for setting the particles for this emitter.
 * Returns: true on success, false otherwise.
 */
bool ESPEmitter::SetParticles(unsigned int numParticles, Texture2D* texture) {
	// Clean up all previous emitter data
	this->Flush();

	assert(texture != NULL);
	this->particleTextures.push_back(texture);

	// Create each of the new particles
	for (unsigned int i = 0; i < numParticles; i++) {
		ESPParticle* newParticle = new ESPParticle();
		this->deadParticles.push_back(newParticle);
		this->textureAssignments[newParticle] = texture;
	}

	return true;
}

/**
 * Public function for setting the particles for this emitter, the textures given
 * are assigned to particles in this emitter at random.
 * Returns: true on success, false otherwise.
 */
bool ESPEmitter::SetParticles(unsigned int numParticles, std::vector<Texture2D*> textures) {
	// Clean up any previous emitter data
	this->Flush();

	this->particleTextures = textures;

	// Create each of the new particles
	for (unsigned int i = 0; i < numParticles; i++) {
		ESPParticle* newParticle = new ESPParticle();
		this->deadParticles.push_back(newParticle);

		// Pick a random texture and assign it...
		this->AssignRandomTextureToParticle(newParticle);
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
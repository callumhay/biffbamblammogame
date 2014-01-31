/**
 * ESPEmitter.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "ESPEmitter.h"
#include "ESPShaderParticle.h"
#include "ESPOnomataParticle.h"
#include "ESPOrthoOnomataParticle.h"
#include "ESPAnimatedSpriteParticle.h"
#include "ESPRandomTextureParticle.h"
#include "ESPAnimatedCurveParticle.h"
#include "ESPEmitterEventHandler.h"
#include "ESPTextureShaderParticle.h"

#include "../BlammoEngine/TextLabel.h"

ESPEmitter::ESPEmitter() : timeSinceLastSpawn(0.0f), particleTexture(NULL),
particleAlignment(ESP::ScreenAlignedGlobalUpVec), particleRed(1), particleGreen(1), particleBlue(1), particleAlpha(1),
particleRotation(0), makeSizeConstraintsEqual(true), numParticleLives(ESPParticle::INFINITE_PARTICLE_LIVES),
isReversed(false), particleDeathPlane(Vector3D(1, 0, 0), Point3D(-FLT_MAX, 0, 0)),
radiusDeviationFromPtX(0.0), radiusDeviationFromPtY(0.0), radiusDeviationFromPtZ(0.0) {
	// NOTE: The death plane has been setup so that it's impossible to be in the 'death-zone' of it
	this->particleSize[0] = ESPInterval(1,1);
	this->particleSize[1] = ESPInterval(1,1);
}

ESPEmitter::~ESPEmitter() {
	this->Flush();
	
	// Clear the effectors
	this->effectors.clear();
	// Clear the event handlers
	this->eventHandlers.clear();
}

/**
 * Private helper function for emptying this emitter of all particles and
 * other heap information.
 */
void ESPEmitter::Flush() {
	assert(this->aliveParticles.size() + this->deadParticles.size() == this->particleLivesLeft.size());
	// Delete particles
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); ++iter) {
		ESPParticle* currParticle = *iter;
        delete currParticle;
		currParticle = NULL;
	}
	this->aliveParticles.clear();

	for (std::list<ESPParticle*>::iterator iter = this->deadParticles.begin(); iter != this->deadParticles.end(); ++iter) {
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
	
    // Depending on whether there's a colour pallet or not, assign a random colour value to the particle that we're reviving
    if (this->particleColourPalette.empty()) {
	    zombie->SetColour(Colour(this->particleRed.RandomValueInInterval(), this->particleGreen.RandomValueInInterval(), 
		    this->particleBlue.RandomValueInInterval()), this->particleAlpha.RandomValueInInterval());
    }
    else {
        const Colour& randomColour = this->particleColourPalette[Randomizer::GetInstance()->RandomUnsignedInt() % this->particleColourPalette.size()];
        zombie->SetColour(randomColour, 1.0f);
    }
	this->aliveParticles.push_back(zombie);

	// Subtract a life from the particle if the lives are not infinite
	int& livesLeftForRevivedParticle = this->particleLivesLeft[zombie];
	if (livesLeftForRevivedParticle != ESPParticle::INFINITE_PARTICLE_LIVES) {
		assert(livesLeftForRevivedParticle != 0);
		livesLeftForRevivedParticle--;
	}

	this->timeSinceLastSpawn = 0.0f;

	// Signify that a particle spawn event has occurred
	for (std::list<ESPEmitterEventHandler*>::iterator iter = this->eventHandlers.begin(); iter != this->eventHandlers.end(); ++iter) {
		(*iter)->ParticleSpawnedEvent(zombie);
	}
}

/**
 * Determines whether the given particle is past the death plane (and thus should be killed).
 * Returns: true if past the death plane, false otherwise.
 */
bool ESPEmitter::IsParticlePastDeathPlane(const ESPParticle& p) {
	const Vector3D planeToParticle = p.GetPosition() - this->particleDeathPlane.GetPointOnPlane();
	return (Vector3D::Dot(planeToParticle, this->particleDeathPlane.GetUnitNormal()) <= 0);
}

/**
 * Private helper function for ticking living particles and managing
 * the movement between alive and dead particles.
 */
void ESPEmitter::TickParticles(double dT) {

    std::list<ESPParticle*>::iterator tempIter;
    ESPParticle* currParticle;

	// Go through the alive iterators and figure out which ones have died and tick those that are still alive
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); ) {
		
        // Give the particle time to do its thing...
		currParticle = *iter;
		tempIter = iter;

		// Check to see if the particle has died, if so place it among the dead
		if (currParticle->IsDead() || this->IsParticlePastDeathPlane(*currParticle)) {
			this->deadParticles.push_back(currParticle);
            for (std::list<ESPEmitterEventHandler*>::iterator iter2 = this->eventHandlers.begin(); iter2 != this->eventHandlers.end(); ++iter2) {
                (*iter2)->ParticleDiedEvent(currParticle);
            }

            iter = this->aliveParticles.erase(iter);
            continue;
		}
		else {
			currParticle->Tick(dT);

			// Have each of the effectors in this emitter affect the particle...
			for (std::list<ESPParticleEffector*>::iterator effIter = this->effectors.begin(); effIter != this->effectors.end(); ++effIter) {
				(*effIter)->AffectParticleOnTick(dT, currParticle);
			}
		}

        ++iter;
	}
}

/**
 * Call this function in order to simulate the functionality equivalent to having
 * called Tick at all the dT over the interval of the given time.
 */
void ESPEmitter::SimulateTicking(double time) {
	double spawnInterval = time;
	// Figure out how many particles should currently be spawned
	if (this->particleLifetime.MeanValueInInterval() != ESPParticle::INFINITE_PARTICLE_LIFETIME) {
		spawnInterval -= this->particleLifetime.maxValue;
	}

	// If the spawn interval is less than or equal to zero then the particle life
	// is longer than the given simulation - just tick away.
	if (spawnInterval <= 0) {
		this->Tick(spawnInterval);
		return;
	}

	// If there's no spawn delta and particles only spawn once - then just spawn everything and be done
	if (this->OnlySpawnsOnce()) {
		this->Tick(spawnInterval);
		return;
	}

	assert(this->particleSpawnDelta.minValue > EPSILON);
	unsigned int numberOfSpawns = spawnInterval / this->particleSpawnDelta.minValue;
	assert(numberOfSpawns >= 0);
	
	// If there were no spawns during the time interval then just tick...
	if (numberOfSpawns == 0) {
		this->Tick(spawnInterval);
		return;
	}

	// Spawn the number of particles given and set all their proper values...
	numberOfSpawns       = std::min<unsigned int>(numberOfSpawns, this->deadParticles.size());
	double ticksPerSpawn = spawnInterval / static_cast<double>(numberOfSpawns);
	double tickCounter   = numberOfSpawns * ticksPerSpawn;
	for (unsigned int i = 0; i < numberOfSpawns; i++) {
		assert(this->aliveParticles.size() == i);

		this->ReviveParticle();
		ESPParticle* newParticle = this->aliveParticles.back();
		newParticle->Tick(tickCounter);
		tickCounter -= ticksPerSpawn;
		
		assert(this->aliveParticles.size() == (i + 1));
	}

	double tickTimeLeft = spawnInterval - (numberOfSpawns * this->particleSpawnDelta.minValue);
	assert(tickTimeLeft >= 0);
	this->Tick(tickTimeLeft);
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
void ESPEmitter::Draw(const Camera& camera) {
	// Setup OpenGL for drawing the particles in this emitter...
	glPushAttrib(GL_TEXTURE_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 	

	glDisable(GL_DEPTH_TEST);
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

	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); ++iter) {
		ESPParticle* currParticle = *iter;
		currParticle->Draw(camera, this->particleAlignment);
	}

	glPopAttrib();
}

void ESPEmitter::DrawWithDepth(const Camera& camera) {
    // Setup OpenGL for drawing the particles in this emitter...
    glPushAttrib(GL_TEXTURE_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 	

    glEnable(GL_DEPTH_TEST);
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

    for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); ++iter) {
        ESPParticle* currParticle = *iter;
        currParticle->Draw(camera, this->particleAlignment);
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
        if (text.GetDropShadow().isSet) {
            newParticle->SetDropShadow(text.GetDropShadow());
        }

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
bool ESPEmitter::SetParticles(unsigned int numParticles, const TextLabel2D& text, Onomatoplex::SoundType st, 
                              Onomatoplex::Extremeness e, bool isOrtho) {
	assert(numParticles > 0);
	// Clean up previous emitter data
	this->Flush();
	
	// Create each of the new particles
	for (unsigned int i = 0; i < numParticles; i++) {
		// Initialize the particle and its attributes
        if (isOrtho) {
            ESPOrthoOnomataParticle* newParticle = new ESPOrthoOnomataParticle(text.GetFont());
            newParticle->SetDropShadow(text.GetDropShadow());
            newParticle->SetOnomatoplexSound(st, e);
            this->deadParticles.push_back(newParticle);
            // Assign the number of lives...
            this->particleLivesLeft[newParticle] = this->numParticleLives;
        }
        else {
		    ESPOnomataParticle* newParticle = new ESPOnomataParticle(text.GetFont());
		    newParticle->SetDropShadow(text.GetDropShadow());
		    newParticle->SetOnomatoplexSound(st, e);
		    this->deadParticles.push_back(newParticle);
		    // Assign the number of lives...
		    this->particleLivesLeft[newParticle] = this->numParticleLives;
        }
	}

	return true;
}

bool ESPEmitter::SetAnimatedParticles(unsigned int numParticles, Texture2D* texture, int spriteSizeX, int spriteSizeY, double animationFPS) {
	assert(numParticles > 0);
	// Clean up previous emitter data
	this->Flush();
	
    this->particleTexture = texture;

	// Create each of the new particles
	for (unsigned int i = 0; i < numParticles; i++) {
		// Initialize the particle and its attributes
		ESPAnimatedSpriteParticle* newParticle = new ESPAnimatedSpriteParticle(texture, spriteSizeX, spriteSizeY, animationFPS);
		this->deadParticles.push_back(newParticle);

		// Assign the number of lives...
		this->particleLivesLeft[newParticle] = this->numParticleLives;
	}

	return true;
}

bool ESPEmitter::SetRandomTextureParticles(unsigned int numParticles, std::vector<Texture2D*>& textures) {
    assert(numParticles > 0);
	// Clean up previous emitter data
	this->Flush();
	
	// Create each of the new particles
	for (unsigned int i = 0; i < numParticles; i++) {
		// Initialize the particle and its attributes
		ESPRandomTextureParticle* newParticle = new ESPRandomTextureParticle(textures);
		this->deadParticles.push_back(newParticle);

		// Assign the number of lives...
		this->particleLivesLeft[newParticle] = this->numParticleLives;
	}

	return true;
}

bool ESPEmitter::SetRandomTextureEffectParticles(unsigned int numParticles, CgFxTextureEffectBase* effect, 
                                                 std::vector<Texture2D*>& textures) {
    assert(numParticles > 0);
    // Clean up previous emitter data
    this->Flush();

    // Create each of the new particles
    for (unsigned int i = 0; i < numParticles; i++) {
        // Initialize the particle and its attributes
        ESPTextureShaderParticle* newParticle = new ESPTextureShaderParticle(effect, textures);
        this->deadParticles.push_back(newParticle);

        // Assign the number of lives...
        this->particleLivesLeft[newParticle] = this->numParticleLives;
    }

    return true;
}

bool ESPEmitter::SetRandomCurveParticles(unsigned int numParticles, const ESPInterval& lineThickness, 
                                         const std::vector<Bezier*>& curves, const ESPInterval& animateTimeInSecs) {
    assert(numParticles > 0);
    // Clean up previous emitter data
    this->Flush();

    // Create each of the new particles
    for (unsigned int i = 0; i < numParticles; i++) {
        // Initialize the particle and its attributes
        ESPAnimatedCurveParticle* newParticle = new ESPAnimatedCurveParticle(curves, lineThickness, animateTimeInSecs);
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
	this->particleSpawnDelta.CopyFromInterval(spawnDelta);
}

/**
 * Sets the inclusive interval of random possible values that represent the
 * initial speed of particles as they are launched out of this emitter.
 */
void ESPEmitter::SetInitialSpd(const ESPInterval& initialSpd) {
	this->particleInitialSpd.CopyFromInterval(initialSpd);
}

/**
 * Sets the inclusive interval of random possible values that represent the
 * lifetime (in seconds) of particles in this emitter.
 */
void ESPEmitter::SetParticleLife(const ESPInterval& particleLife, bool affectLiveParticles) {
	this->particleLifetime.CopyFromInterval(particleLife);
	
    // Go through any already assigned particles and set the lifespan...
    if (affectLiveParticles) {
	    for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); ++iter) {
		    ESPParticle* currParticle = *iter;
		    currParticle->ResetLifespanLength(particleLife.RandomValueInInterval());
	    }
    }
}

/**
 * Sets the inclusive interval of random possible values that represent the
 * size (in units) of particles in this emitter.
 */
void ESPEmitter::SetParticleSize(const ESPInterval& particleSizeX, const ESPInterval& particleSizeY) {
	this->particleSize[0] = particleSizeX;
	this->particleSize[1] = particleSizeY;
	this->makeSizeConstraintsEqual = false;

    float valueX = particleSizeX.MeanValueInInterval();
    float valueY = particleSizeY.MeanValueInInterval();
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); ++iter) {
		ESPParticle* currParticle = *iter;
        currParticle->SetScale(valueX, valueY);
	}
}
void ESPEmitter::SetParticleSize(const ESPInterval& particleSize) {
	this->SetParticleSpawnSize(particleSize);

    float value = particleSize.MeanValueInInterval();
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); ++iter) {
		ESPParticle* currParticle = *iter;
        currParticle->SetScale(value, value);
	}
}

void ESPEmitter::SetParticleSpawnSize(const ESPInterval& particleSize) {
	this->particleSize[0] = particleSize;
	this->particleSize[1] = particleSize;
	this->makeSizeConstraintsEqual = true;
}

/**
 * Sets the inclusive interval of random possible values that represent the initial
 * rotation (in degrees) of particles in this emitter.
 */
void ESPEmitter::SetParticleRotation(const ESPInterval& particleRot) {
	this->particleRotation.CopyFromInterval(particleRot);

	// Go through any already assigned particles and set the rotation...
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); ++iter) {
		ESPParticle* currParticle = *iter;
		currParticle->SetRotation(particleRot.RandomValueInInterval());
	}
}

/**
 * Sets the inclusive intervals of random possible values for the colour and alpha of particles.
 */
void ESPEmitter::SetParticleColour(const ESPInterval& red, const ESPInterval& green, const ESPInterval& blue, const ESPInterval& alpha) {
	this->particleRed.CopyFromInterval(red);
	this->particleGreen.CopyFromInterval(green);
	this->particleBlue.CopyFromInterval(blue);
	this->particleAlpha.CopyFromInterval(alpha);

	// Go through any already assigned particles and set the colour...
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); ++iter) {
		ESPParticle* currParticle = *iter;
		currParticle->SetColour(red.RandomValueInInterval(), green.RandomValueInInterval(), 
            blue.RandomValueInInterval(), alpha.RandomValueInInterval());
	}
}

void ESPEmitter::SetParticleColourPalette(const std::vector<Colour>& colours) {
    this->particleColourPalette = colours;

    if (this->particleColourPalette.empty()) {
        this->SetParticleColour(this->particleRed, this->particleGreen, this->particleBlue, this->particleAlpha);
    }
    else {
        // Go through any already assigned particles and set the colour...
        for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); ++iter) {
            ESPParticle* currParticle = *iter;
            const Colour& randomColour = this->particleColourPalette[Randomizer::GetInstance()->RandomUnsignedInt() % colours.size()];
            currParticle->SetColour(randomColour, 1.0f);
        }
    }
}

void ESPEmitter::SetParticleAlpha(const ESPInterval& alpha) {
	this->particleAlpha.CopyFromInterval(alpha);
	// Go through any already assigned particles and set the alpha...
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin();
         iter != this->aliveParticles.end(); ++iter) {

		ESPParticle* currParticle = *iter;
		currParticle->SetAlpha(alpha.RandomValueInInterval());
	}
}

void ESPEmitter::SetAliveParticleAlphaMax(float alpha) {
    for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin();
        iter != this->aliveParticles.end(); ++iter) {

        ESPParticle* currParticle = *iter;
        if (alpha < currParticle->GetAlpha()) {
            currParticle->SetAlpha(alpha);
        }
        else if (currParticle->GetLifespanLength() == ESPParticle::INFINITE_PARTICLE_LIFETIME) {
            currParticle->SetAlpha(std::min<float>(alpha, this->particleAlpha.RandomValueInInterval()));
        }
    }
}

void ESPEmitter::SetAliveParticlePosition(float x, float y, float z) {
    for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin();
         iter != this->aliveParticles.end(); ++iter) {

        ESPParticle* currParticle = *iter;
        currParticle->SetPosition(x, y, z);
    }
}

void ESPEmitter::TranslateAliveParticlePosition(const Vector3D& t) {
    for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin();
        iter != this->aliveParticles.end(); ++iter) {

        ESPParticle* currParticle = *iter;
        currParticle->SetPosition(currParticle->GetPosition() + t);
    }
}

void ESPEmitter::SetAliveParticleScale(const ESPInterval& sX, const ESPInterval& sY) {
    for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin();
        iter != this->aliveParticles.end(); ++iter) {

            ESPParticle* currParticle = *iter;
            currParticle->SetScale(sX.RandomValueInInterval(), sY.RandomValueInInterval());
    }
}

void ESPEmitter::SetAliveParticleScale(const ESPInterval& size) {
    for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin();
        iter != this->aliveParticles.end(); ++iter) {

            ESPParticle* currParticle = *iter;
            float scale = size.RandomValueInInterval();
            currParticle->SetScale(scale, scale);
    }
}

void ESPEmitter::SetNumParticleLives(int lives) {
	assert(lives >= ESPParticle::INFINITE_PARTICLE_LIVES);
    
	this->numParticleLives = lives;

	// Go through any already assigned particles and set the lives...
	for (std::list<ESPParticle*>::iterator iter = this->aliveParticles.begin(); iter != this->aliveParticles.end(); ++iter) {
		ESPParticle* currParticle = *iter;
		this->particleLivesLeft[currParticle] = lives;
	}
	for (std::list<ESPParticle*>::iterator iter = this->deadParticles.begin(); iter != this->deadParticles.end(); ++iter) {
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
 * Set the deviation from the center emit point where particles may spawn.
 */
void ESPEmitter::SetRadiusDeviationFromCenter(const ESPInterval& distFromCenter) {
	this->radiusDeviationFromPtX.CopyFromInterval(distFromCenter);
	this->radiusDeviationFromPtY.CopyFromInterval(distFromCenter);
	this->radiusDeviationFromPtZ.CopyFromInterval(distFromCenter);
}
void ESPEmitter::SetRadiusDeviationFromCenter(const ESPInterval& xDistFromCenter, const ESPInterval& yDistFromCenter, 
																							const ESPInterval& zDistFromCenter) {
	this->radiusDeviationFromPtX.CopyFromInterval(xDistFromCenter);
	this->radiusDeviationFromPtY.CopyFromInterval(yDistFromCenter);
	this->radiusDeviationFromPtZ.CopyFromInterval(zDistFromCenter);
}

/**
 * Sets the death plane for the particle, if the particle is ever on the negative
 * side of this plane then it will die (the negative side is defined as the one opposite
 * of the normal of the plane).
 */
void ESPEmitter::SetParticleDeathPlane(const Plane& plane) {
	this->particleDeathPlane = plane;
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

void ESPEmitter::ClearEffectors() {
    this->effectors.clear();
}

/**
 * Adds an event handler to this emitter.
 */
void ESPEmitter::AddEventHandler(ESPEmitterEventHandler* eventHandler) {
	assert(eventHandler != NULL);
	this->eventHandlers.push_back(eventHandler);
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

    for (std::map<const ESPParticle*, int>::iterator iter = this->particleLivesLeft.begin();
         iter != this->particleLivesLeft.end(); ++iter) {
        iter->second = this->numParticleLives;
    }

	// Reset time...
	this->timeSinceLastSpawn = 0.0f;
}
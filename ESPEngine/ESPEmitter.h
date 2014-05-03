/**
 * ESPEmitter.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#ifndef __ESPEMITTER_H__
#define __ESPEMITTER_H__

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Plane.h"

#include "../GameModel/Onomatoplex.h"

#include "ESPAbstractEmitter.h"
#include "ESPParticle.h"
#include "ESPUtil.h"
#include "ESPEffector.h"
#include "ESPParticleBatchMesh.h"

#include <vector>
#include <string>
#include <map>

class CgFxEffectBase;
class CgFxTextureEffectBase;
class TextLabel2D;
class ESPEmitterEventHandler;
class Bezier;

/**
 * ESPEmitter.h
 * Abstract base class representing an Emitter of particles.
 */
class ESPEmitter : public ESPAbstractEmitter {
public:
	ESPEmitter();
	virtual ~ESPEmitter();

	static const int ONLY_SPAWN_ONCE = -1;
    static const int NO_CUTOFF_LIFETIME = -1;

	bool ParticlesHaveNoLivesLeft() const {
		if (this->numParticleLives != ESPParticle::INFINITE_PARTICLE_LIVES) {
			for (std::map<const ESPParticle*, int>::const_iterator iter = this->particleLivesLeft.begin(); iter != this->particleLivesLeft.end(); ++iter) {
				if (iter->second != 0) {
					return false;
				}
			}
			return true;
		}
		return false;
	}

	bool IsDead() const {
		return (this->aliveParticles.size() == 0) && 
            (this->timeSinceLastSpawn > this->particleLifetime.maxValue || this->ParticlesHaveNoLivesLeft());
	}

	bool OnlySpawnsOnce() const {
		return this->particleSpawnDelta.minValue == ESPEmitter::ONLY_SPAWN_ONCE;
	}

	// Setter functions for typical attributes of emitters
	bool SetParticles(unsigned int numParticles, Texture2D* texture);
	bool SetParticles(unsigned int numParticles, CgFxEffectBase* effect);
	bool SetParticles(unsigned int numParticles, const TextLabel2D& text);
	bool SetParticles(unsigned int numParticles, const TextLabel2D& text, Onomatoplex::SoundType st, Onomatoplex::Extremeness e, bool isOrtho = false);
    
    bool SetAnimatedParticles(unsigned int numParticles, Texture2D* texture, int spriteSizeX, int spriteSizeY, double animationFPS = 24.0);
    bool SetRandomTextureParticles(unsigned int numParticles, std::vector<Texture2D*>& textures);
    bool SetRandomTextureEffectParticles(unsigned int numParticles, CgFxTextureEffectBase* effect, std::vector<Texture2D*>& textures);
    bool SetRandomCurveParticles(unsigned int numParticles, const ESPInterval& lineThickness, 
        const std::vector<Bezier*>& curves, const ESPInterval& animateTimeInSecs);

	void SetParticleAlignment(const ESP::ESPAlignment alignment);
	void SetSpawnDelta(const ESPInterval& spawnDelta, bool firstSpawnNoDelta = false);
	void SetInitialSpd(const ESPInterval& initialSpd);
	void SetParticleLife(const ESPInterval& particleLife, bool affectLiveParticles = false);

	void SetParticleSize(const ESPInterval& particleSizeX, const ESPInterval& particleSizeY);
	void SetParticleSize(const ESPInterval& particleSize);
    void SetParticleSpawnSize(const ESPInterval& particleSize);

	void SetParticleColour(const ESPInterval& red, const ESPInterval& green, const ESPInterval& blue, const ESPInterval& alpha);
    void SetParticleColour(const Colour& c);
    void SetParticleColourPalette(const std::vector<Colour>& colours);
	void SetParticleAlpha(const ESPInterval& alpha);
    void SetAliveParticleAlphaMax(float alpha);
    void SetAliveParticlePosition(float x, float y, float z);
    void TranslateAliveParticlePosition(const Vector3D& t);
    void SetAliveParticleScale(const ESPInterval& sX, const ESPInterval& sY);
    void SetAliveParticleScale(const ESPInterval& size);
	void SetParticleRotation(const ESPInterval& particleRot);
	void SetNumParticleLives(int lives);
	void SetIsReversed(bool isReversed);
	void SetRadiusDeviationFromCenter(const ESPInterval& distFromCenter);
	void SetRadiusDeviationFromCenter(const ESPInterval& xDistFromCenter, const ESPInterval& yDistFromCenter, const ESPInterval& zDistFromCenter);
	void SetParticleDeathPlane(const Plane& plane);

    void SetCutoffLifetime(double timeInSecs);

	void AddEffector(ESPEffector* effector);
    void AddCopiedEffector(const ESPEffector& effector);
    void ClearEffectors();
	void AddEventHandler(ESPEmitterEventHandler* eventHandler);

	void AddParticle(ESPParticle* particle);

    ESP::ESPAlignment GetParticleAlignment() const { return this->particleAlignment; }
	bool GetHasParticles() const;

    const ESPInterval& GetParticleAlpha() const { return this->particleAlpha; }
	const ESPInterval& GetParticleSizeX() const { return this->particleSize[0]; }
	const ESPInterval& GetParticleSizeY() const { return this->particleSize[1]; }
    const ESPInterval& GetParticleInitialSpd() const { return this->particleInitialSpd; }

	void SimulateTicking(double time);

	void Tick(double dT);
	void Draw(const Camera& camera);
    void DrawWithDepth(const Camera& camera);

	void Reset();

protected:			
	Texture2D* particleTexture;		// Texture of particles in this emitter

	std::map<const ESPParticle*, int> particleLivesLeft;  // Number of lives left for each particle
	std::list<ESPParticle*> aliveParticles;               // All the alive particles in this emitter
	std::list<ESPParticle*> deadParticles;                // All the dead particles in this emitter
	
    // If not NO_CUTOFF_LIFETIME, then this will kill the particle once it expires
    double cutoffLifetimeInSecs;
    double currCutoffLifetimeCountdown;

	float timeSinceLastSpawn;  // Time since the last particle was spawned	
	int numParticleLives;
	
	bool isReversed;		// Whether this emitter is reversed (i.e., particles start where they die and die where they start)

    std::list<std::pair<ESPEffector*, bool> > effectors; // All the particle effectors of this emitter, pairing boolean means that this object owns the emitter
	std::list<ESPEmitterEventHandler*> eventHandlers; // The event handlers attached to this emitter
	
	Plane particleDeathPlane;

	// The alignment of particles in this emitter w.r.t. the viewer
	ESP::ESPAlignment particleAlignment;
	// Inclusive interval of time between firing/spawning of particles in seconds
	ESPInterval particleSpawnDelta;
	// Inclusive interval for initial Speed of particles
	ESPInterval particleInitialSpd;
	// Inclusive interval for lifetime (in seconds) of particles
	ESPInterval particleLifetime;
	// Inclusive interval for size of particles (in units)
	ESPInterval particleSize[2];
	bool makeSizeConstraintsEqual;
	// Inclusive interval for the rotation of particles (in degrees)
	ESPInterval particleRotation;
	// Inclusive intervals for the colour of particles
	ESPInterval particleRed, particleGreen, particleBlue, particleAlpha;
    // Discrete particle colours may also be defined, if this is not empty then a random
    // colour from the collection of particleColours will be chosen
    std::vector<Colour> particleColourPalette;
	// Inclusive interval for how far from the emitPt this particle may spawn at
	ESPInterval radiusDeviationFromPtX;
	ESPInterval radiusDeviationFromPtY;
	ESPInterval radiusDeviationFromPtZ;

	void Flush();
    void Kill();

	virtual Vector3D CalculateRandomInitParticleDir() const = 0;
	virtual Point3D  CalculateRandomInitParticlePos() const = 0;
	void ReviveParticle();

	bool IsParticlePastDeathPlane(const ESPParticle& p);

private:
	void TickParticles(double dT);
    DISALLOW_COPY_AND_ASSIGN(ESPEmitter);
};

inline bool ESPEmitter::GetHasParticles() const {
	return !(this->aliveParticles.empty() && this->deadParticles.empty());
}

inline void ESPEmitter::SetParticleColour(const Colour& c) {
    this->SetParticleColour(ESPInterval(c.R()), ESPInterval(c.G()), ESPInterval(c.B()), ESPInterval(1.0f));
}

#endif
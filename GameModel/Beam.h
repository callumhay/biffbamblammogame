/**
 * Beam.h
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

#ifndef __BEAM_H__
#define __BEAM_H__

#include "../BlammoEngine/Collision.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/IPositionObject.h"
#include "../BlammoEngine/Colour.h"

class LevelPiece;
class Projectile;
class PlayerPaddle;
class GameModel;
class GameLevel;
class BeamColliderStrategy;

// Structure for defining a part of the overall beam - a complete
// linear segment of the beam
class BeamSegment {
public:
	BeamSegment(const Collision::Ray2D& beamRay, float beamRadius, int beamDmgPerSec, const void* ignoreThing);
	~BeamSegment();

	BeamColliderStrategy* FireBeamSegmentIntoLevel(const GameModel& gameModel, bool cannotCollideWithPaddle);
	const Collision::Ray2D& GetBeamSegmentRay() const { return this->ray; }
    void SetBeamSegmentRay(const Collision::Ray2D& r) { this->ray = r; }
	
	LevelPiece* GetCollidingPiece() const { return this->collidingPiece; }
    Projectile* GetCollidingProjectile() const { return this->collidingProjectile; }

	const Point2D& GetStartPoint() const { return this->ray.GetOrigin(); }
	Point2D GetEndPoint() const { return this->ray.GetPointAlongRayFromOrigin(this->endT); }
	
	float GetRadius() const { return this->radius; }
	void SetRadius(float radius);

	float GetLength() const { 
		assert(this->endT >= 0);
		return this->endT;
	}
    void SetLength(float length) {
        this->endT = length;
    }

	void Tick(double dT);

	const int& GetDamagePerSecond() const { return this->damagePerSecond; }
	void SetDamagePerSecond(int dmgPerSec) { this->damagePerSecond = dmgPerSec; }

    float GetCurrentRadiusFraction() const { return this->initialRadius == 0 ? 0.0f : (this->radius / this->initialRadius); }

	static bool Equals(const BeamSegment& beamSeg1, const BeamSegment& beamSeg2);

private:
	const void* ignoreThing;    // If a beam is originating inside a particular object then we want it to ignore all collisions with that object...

    Projectile* collidingProjectile;
	LevelPiece* collidingPiece;	// The level piece that the beam is colliding with, which marks the end of the beam
                                // In cases where this is NULL the beam technically is going out of bounds or is colliding with
                                // a non-block object

	int damagePerSecond; // Amount of damage this beam segment does per second to damageable blocks it hits
                         // NOTE: typical life of a block is 100

	double timeSinceFired;	// Time in seconds since this part of the beam was fired/shot
	Collision::Ray2D ray;   // The ray defining this part of the beam
	const float initialRadius;    // Initial half-width of the beam segment
    float radius;           // Half-width of the beam segment
	float endT;             // The value that when fed into the ray equation will give where this beam segment ends

	AnimationMultiLerp<float> radiusPulseAnim;

    DISALLOW_COPY_AND_ASSIGN(BeamSegment);
};

class Beam : public IPositionObject {
public:
	enum BeamType { PaddleBeam, BossBeam };

    static const float MIN_BEAM_RADIUS;
    static const int MIN_DMG_PER_SEC;

	virtual ~Beam();
    virtual Beam::BeamType GetType() const = 0;
    virtual bool CanDestroyLevelPieces() const = 0;

	virtual void UpdateCollisions(const GameModel* gameModel) = 0;
	virtual bool Tick(double dT, const GameModel* gameModel);
    virtual void TickAlpha();

    void SetZOffset(float zOffset) { this->zOffset = zOffset; }
    float GetZOffset() const { return this->zOffset; }

    float GetBeamAlpha() const { return this->beamAlpha; }

	void ResetTimeElapsed();

	std::list<BeamSegment*>& GetBeamParts() { return this->beamParts; }
	const std::list<BeamSegment*>& GetBeamParts() const { return this->beamParts; }

	virtual int GetNumBaseBeamSegments() const = 0;
    virtual const Colour& GetBeamColour() const = 0;

    virtual void UpdateOriginBeamSegment(const GameModel* gameModel, const Collision::Ray2D& newOriginBeamRay) = 0;
    //void UpdateOriginBeamSegments(const std::vector<Collision::Ray2D>& newOriginBeams);

	void DebugDraw() const;

    // Inherited from IPositionObject
    Point3D GetPosition3D() const {
        if (this->beamParts.empty()) { return Point3D(0,0,0); }
        return Point3D(this->beamParts.front()->GetStartPoint(), 0.0f);
    }

protected:
    static const double MIN_ALLOWED_LIFETIME_IN_SECS;

	std::list<BeamSegment*> beamParts;  // The beam's parts that make up this entire beam
	int baseDamagePerSecond;            // Amount of damage the beam does per second to damagable blocks it hits
                                        // NOTE: typical life of a block is 100

	double totalLifeTime;    // Total amount of time that the beam lives for in seconds
	double currTimeElapsed;  // The amount of time elapsed for the beam so far in seconds

    bool beamAlphaDirty;
    float beamAlpha; // As the beam nears the end of its life it's alpha diminishes and so does its power/damage
    float zOffset;

	void CleanUpBeam(std::list<BeamSegment*>& beamSegs);
	bool BeamHasChanged(const std::list<BeamSegment*>& oldBeamSegs, const std::list<BeamSegment*>& newBeamSegs);
    void BuildAndUpdateCollisionsForBeamParts(const std::list<BeamSegment*>& initialBeamSegs, const GameModel* gameModel);

	Beam(int dmgPerSec, double lifeTimeInSec);
};

inline void Beam::ResetTimeElapsed() {
	this->currTimeElapsed = 0.0f;
}

#endif
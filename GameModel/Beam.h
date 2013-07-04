/**
 * Beam.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011 - 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BEAM_H__
#define __BEAM_H__

#include "../BlammoEngine/Collision.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/IPositionObject.h"
#include "../BlammoEngine/Colour.h"

#include <list>

class LevelPiece;
class PlayerPaddle;
class GameModel;
class GameLevel;

// Structure for defining a part of the overall beam - a complete
// linear segment of the beam
class BeamSegment {
public:
	BeamSegment(const Collision::Ray2D& beamRay, float beamRadius, int beamDmgPerSec, LevelPiece* ignorePiece);
	~BeamSegment();

	LevelPiece* FireBeamSegmentIntoLevel(const GameLevel* level);
	const Collision::Ray2D& GetBeamSegmentRay() const { return this->ray; }
	
	LevelPiece* GetCollidingPiece() const { return this->collidingPiece; }
	void SetCollidingPiece(LevelPiece* piece) { this->collidingPiece = piece; }

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

	static bool Equals(const BeamSegment& beamSeg1, const BeamSegment& beamSeg2);

private:
	LevelPiece* ignorePiece;    // If a beam is originating inside a level piece then we want it to ignore all collisions with that piece...
	LevelPiece* collidingPiece;	// The level piece that the beam is colliding with, which marks the end of the beam
                                // In cases where this is NULL the beam technically is going out of bounds, in which case
                                // it is limited by a maximum length

	int damagePerSecond; // Amount of damage this beamsegment does per second to damagable blocks it hits
                         // NOTE: typical life of a block is 100

	double timeSinceFired;	// Time in seconds since this part of the beam was fired/shot
	Collision::Ray2D ray;   // The ray defining this part of the beam
	float radius;           // Half-width of the beam segment
	float endT;             // The value that when fed into the ray equation will give where this beam segment ends

	AnimationMultiLerp<float> radiusPulseAnim;

    DISALLOW_COPY_AND_ASSIGN(BeamSegment);
};

class Beam : public IPositionObject {
public:
	enum BeamType { PaddleBeam, BossBeam };

	virtual ~Beam();
    virtual Beam::BeamType GetType() const = 0;
    virtual bool CanDestroyLevelPieces() const = 0;

	virtual void UpdateCollisions(const GameModel* gameModel) = 0;
	bool Tick(double dT);

    float GetBeamAlpha() const { return this->beamAlpha; }

	void ResetTimeElapsed();

	std::list<BeamSegment*>& GetBeamParts() { return this->beamParts; }
	const std::list<BeamSegment*>& GetBeamParts() const { return this->beamParts; }

	virtual int GetNumBaseBeamSegments() const = 0;
    virtual const Colour& GetBeamColour() const = 0;

	void DebugDraw() const;

    // Inherited from IPositionObject
    Point3D GetPosition3D() const {
        if (this->beamParts.empty()) { return Point3D(0,0,0); }
        return Point3D(this->beamParts.front()->GetStartPoint(), 0.0f);
    }

protected:
    static const double MIN_ALLOWED_LIFETIME_IN_SECS;
	static const float MIN_BEAM_RADIUS;
	static const int MIN_DMG_PER_SEC;

	std::list<BeamSegment*> beamParts;  // The beam's parts that make up this entire beam
	int baseDamagePerSecond;            // Amount of damage the beam does per second to damagable blocks it hits
                                        // NOTE: typical life of a block is 100

	double totalLifeTime;    // Total amount of time that the beam lives for in seconds
	double currTimeElapsed;  // The amount of time elapsed for the beam so far in seconds

    float beamAlpha; // As the beam nears the end of its life it's alpha diminishes and so does its power/damage

	void CleanUpBeam(std::list<BeamSegment*>& beamSegs);
	bool BeamHasChanged(const std::list<BeamSegment*>& oldBeamSegs, const std::list<BeamSegment*>& newBeamSegs);
    void BuildAndUpdateCollisionsForBeamParts(const std::list<BeamSegment*>& initialBeamSegs, const GameModel* gameModel);

	Beam(int dmgPerSec, double lifeTimeInSec);
};

inline void Beam::ResetTimeElapsed() {
	this->currTimeElapsed = 0.0f;
}

#endif
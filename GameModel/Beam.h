/**
 * Beam.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009 - 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BEAM_H__
#define __BEAM_H__

#include "../BlammoEngine/Collision.h"
#include "../BlammoEngine/Animation.h"

#include <list>

class LevelPiece;
class PlayerPaddle;
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
	
	const float& GetRadius() const { return this->radius; }
	void SetRadius(float radius);

	const float& GetLength() const { 
		assert(this->endT >= 0);
		return this->endT;
	}

	void Tick(double dT);

	const int& GetDamagePerSecond() const { return this->damagePerSecond; }
	void SetDamagePerSecond(int dmgPerSec) { this->damagePerSecond = dmgPerSec; }

	static bool Equals(const BeamSegment& beamSeg1, const BeamSegment& beamSeg2);

private:
	LevelPiece* ignorePiece;		// If a beam is originating inside a level piece then we want it to ignore all collisions with that piece...
	LevelPiece* collidingPiece;	// The level piece that the beam is colliding with, which marks the end of the beam
															// In cases where this is NULL the beam technically is going out of bounds, in which case
															// it is limited by a maximum length

	int damagePerSecond;				// Amount of damage this beamsegment does per second to damagable blocks it hits
															// NOTE: typical life of a block is 100

	double timeSinceFired;	// Time in seconds since this part of the beam was fired/shot
	Collision::Ray2D ray;		// The ray defining this part of the beam
	float radius;						// Half-width of the beam segment
	float endT;							// The value that when fed into the ray equation will give where this beam segment ends

	AnimationMultiLerp<float> radiusPulseAnim;

};

class Beam {
public:
	enum BeamType { PaddleLaserBeam };

	virtual ~Beam();
	BeamType GetBeamType() const { return this->type; }

	virtual void UpdateCollisions(const GameLevel* level) = 0;
	bool Tick(double dT);

	void ResetTimeElapsed();

	std::list<BeamSegment*>& GetBeamParts() { return this->beamParts; }
	const std::list<BeamSegment*>& GetBeamParts() const { return this->beamParts; }

	virtual int GetNumBaseBeamSegments() const = 0;

	void DebugDraw() const;

protected:
	static const float MIN_BEAM_RADIUS;
	static const int   MIN_DMG_PER_SEC;

	BeamType type;														// Type of beam
	std::list<BeamSegment*> beamParts;				// The beam's parts that make up this entire beam
	int baseDamagePerSecond;									// Amount of damage the beam does per second to damagable blocks it hits
																						// NOTE: typical life of a block is 100

	double totalLifeTime;			// Total amount of time that the beam lives for in seconds
	double currTimeElapsed;		// The amount of time elapsed for the beam so far in seconds

	void CleanUpBeam(std::list<BeamSegment*>& beamSegs);
	bool BeamHasChanged(const std::list<BeamSegment*>& oldBeamSegs, const std::list<BeamSegment*>& newBeamSegs);

	Beam(BeamType type, int dmgPerSec, double lifeTimeInSec);
};

inline void Beam::ResetTimeElapsed() {
	this->currTimeElapsed = 0.0f;
}

#endif
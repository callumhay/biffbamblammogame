/**
 * Beam.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009 - 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "Beam.h"
#include "LevelPiece.h"
#include "PortalBlock.h"
#include "PlayerPaddle.h"
#include "GameLevel.h"
#include "GameEventManager.h"

const float Beam::MIN_BEAM_RADIUS = 0.05f;
const int   Beam::MIN_DMG_PER_SEC = 25;

Beam::Beam(BeamType type, int dmgPerSec, double lifeTimeInSec) : 
type(type), baseDamagePerSecond(dmgPerSec), currTimeElapsed(0.0), totalLifeTime(lifeTimeInSec) {
};

Beam::~Beam() {
	this->CleanUpBeam(this->beamParts);
}

// Deletes any beam segments currently associated with this beam
void Beam::CleanUpBeam(std::list<BeamSegment*>& beamSegs) {
	for (std::list<BeamSegment*>::iterator iter = beamSegs.begin(); iter != beamSegs.end(); ++iter) {
		BeamSegment* currSeg = *iter;
		delete currSeg;
		currSeg = NULL;
	}
	beamSegs.clear();
}

/**
 * Check to see if the given lists of beam segments are different...
 * Returns: true if they are different, false otherwise.
 */
bool Beam::BeamHasChanged(const std::list<BeamSegment*>& oldBeamSegs, const std::list<BeamSegment*>& newBeamSegs) {
	// First test is easy - just see if there are the same number of segments...
	if (oldBeamSegs.size() != newBeamSegs.size()) {
		return true;
	}

	// Now go through each segment in the old and new and compare...
	std::list<BeamSegment*>::const_iterator oldIter = oldBeamSegs.begin();
	std::list<BeamSegment*>::const_iterator newIter = newBeamSegs.begin();
	for (; oldIter != oldBeamSegs.end() && newIter != newBeamSegs.end(); ++oldIter, ++newIter) {
		if (!BeamSegment::Equals(**oldIter, **newIter)) {
			return true;
		}
	}

	return false;
}

/**
 * Used to draw the rays that this beam is using for collision detection
 * for debug purposes.
 */
void Beam::DebugDraw() const {
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
	glDisable(GL_DEPTH_TEST);

	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f);
	// Go through each beam and draw its 'ray' from origin to end
	for (std::list<BeamSegment*>::const_iterator iter = this->beamParts.begin(); iter != this->beamParts.end(); ++iter) {
		BeamSegment* currentSeg = *iter;
		Point2D start = currentSeg->GetStartPoint();
		Point2D end   = currentSeg->GetEndPoint();

		glVertex2f(start[0], start[1]);
		glVertex2f(end[0], end[1]);
	}
	glEnd();

	glPopAttrib();
}

/**
 * Tick away at the life of this beam.
 * Returns: true if the beam has expired, false otherwise.
 */
bool Beam::Tick(double dT) {
	if (this->currTimeElapsed >= this->totalLifeTime) {
		return true;
	}
	else {
		// Tick all the segments...
		for (std::list<BeamSegment*>::iterator iter = this->beamParts.begin(); iter != this->beamParts.end(); ++iter) {
			(*iter)->Tick(dT);
		}
		this->currTimeElapsed += dT;
	}
	return false;
}

BeamSegment::BeamSegment(const Collision::Ray2D& beamRay, float beamRadius, int beamDmgPerSec, LevelPiece* ignorePiece) :
timeSinceFired(0.0), ray(beamRay), collidingPiece(NULL), endT(0.0f),
ignorePiece(ignorePiece), damagePerSecond(beamDmgPerSec) {

	this->SetRadius(beamRadius);
}
void BeamSegment::SetRadius(float radius) { 
	this->radius = radius;  

	std::vector<double> animationTimes;
	animationTimes.push_back(0.0);
	animationTimes.push_back(0.3);
	animationTimes.push_back(0.6);
	std::vector<float> animationRadii;
	animationRadii.push_back(this->radius);
	animationRadii.push_back(0.9f * this->radius);
	animationRadii.push_back(this->radius);

	radiusPulseAnim = AnimationMultiLerp<float>(&this->radius);
	radiusPulseAnim.SetLerp(animationTimes, animationRadii);
	radiusPulseAnim.SetRepeat(true);
}

BeamSegment::~BeamSegment() {
	if (this->collidingPiece != NULL) {
		this->collidingPiece = NULL;
	}
}

void BeamSegment::Tick(double dT) {
	this->radiusPulseAnim.Tick(dT);
}

/**
 * Fire this beam segment into the given level.
 * Returns: The level piece that this beam segment collided with when shot - NULL if no collisions.
 */
LevelPiece* BeamSegment::FireBeamSegmentIntoLevel(const GameLevel* level) {
	assert(level != NULL);
	
	std::set<const LevelPiece*> ignorePieces;
	ignorePieces.insert(this->ignorePiece);

	// Collide the ray of this beam with the level - find out where the collision occurs
	// and what piece is being collided with - set those values for the members of this beam segment.
    this->collidingPiece = level->GetLevelPieceFirstCollider(this->ray, ignorePieces, this->endT);

	// Make sure that the endT isn't set if there was no collision - it flies out of the level...
	if (this->collidingPiece == NULL) {
		this->endT = std::max<float>(level->GetLevelUnitHeight(), level->GetLevelUnitWidth());
	}

	return this->collidingPiece;
}

/**
 * Check to see if two beam segments are 'equal'.
 * Returns: true if they are equal, false otherwise.
 */
bool BeamSegment::Equals(const BeamSegment& beamSeg1, const BeamSegment& beamSeg2) {
	if (beamSeg1.GetStartPoint() != beamSeg2.GetStartPoint()) {
		return false;
	}

	if (beamSeg1.GetEndPoint() != beamSeg2.GetEndPoint()) {
		return false;
	}

	if (fabs(beamSeg1.GetLength() - beamSeg2.GetLength()) > EPSILON) {
		return false;
	}

	// Make sure the base radii are the same
	if (beamSeg1.radiusPulseAnim.GetInterpolationValues()[0] != beamSeg2.radiusPulseAnim.GetInterpolationValues()[0]) {
		return false;
	}
		
	if (beamSeg1.GetCollidingPiece() != beamSeg2.GetCollidingPiece()) {
		return false;
	}

	return true;
}
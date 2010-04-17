/**
 * Beam.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
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

const float Beam::MIN_BEAM_RADIUS = 0.1f;

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

	glEnable(GL_DEPTH_TEST);
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

	// Collide the ray of this beam with the level - find out where the collision occurs
	// and what piece is being collided with - set those values for the members of this beam segment.
	this->collidingPiece = level->GetLevelPieceFirstCollider(this->ray, this->ignorePiece, this->endT);

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

const double PaddleLaserBeam::BEAM_EXPIRE_TIME_IN_SECONDS	= 10;		// Length of time for the beam to be firing
const int PaddleLaserBeam::BASE_DAMAGE_PER_SECOND				  = 150;	// Damage per second that the paddle laser does to blocks and stuff
																																	// NOTE: a typical block has about 100 life

PaddleLaserBeam::PaddleLaserBeam(PlayerPaddle* paddle, const GameLevel* level) : 
Beam(Beam::PaddleLaserBeam, PaddleLaserBeam::BASE_DAMAGE_PER_SECOND, PaddleLaserBeam::BEAM_EXPIRE_TIME_IN_SECONDS), 
paddle(paddle), reInitStickyPaddle(true) {
	assert((paddle->GetPaddleType() & PlayerPaddle::LaserBeamPaddle) == PlayerPaddle::LaserBeamPaddle);
	
	this->UpdateCollisions(level);
	paddle->SetIsLaserBeamFiring(true);
}

PaddleLaserBeam::~PaddleLaserBeam() {
	// Remove the paddle laser beam...
	this->paddle->RemovePaddleType(PlayerPaddle::LaserBeamPaddle);
	this->paddle->SetIsLaserBeamFiring(false);
}

/**
 * Update the collisions for this beam - this will fire the current beam
 */
void PaddleLaserBeam::UpdateCollisions(const GameLevel* level) {
	assert(this->paddle != NULL);

	// Keep a copy of the old beam segments (for comparison afterwards)
	std::list<BeamSegment*> oldBeamSegments = this->beamParts;
	this->beamParts.clear();

	// Create the very first beam part that shoots out of the paddle towards the level
	const float INITIAL_BEAM_RADIUS  = this->paddle->GetHalfFlatTopWidth()/2.0f;
	const Point2D BEAM_ORIGIN				 = this->paddle->GetCenterPosition() + Vector2D(0, this->paddle->GetHalfHeight());
	const Vector2D BEAM_UNIT_DIR		 = this->paddle->GetUpVector();

	// The current damage is determined by a ratio of the normal paddle width to its current width and
	// the typical base-damage that this beam does
	this->baseDamagePerSecond				 = ((2.0f * this->paddle->GetHalfFlatTopWidth()) / PlayerPaddle::PADDLE_WIDTH_FLAT_TOP) * PaddleLaserBeam::BASE_DAMAGE_PER_SECOND;
	
	// Create the first beam segment, emitting from the paddle
	BeamSegment* firstBeamSeg = NULL;

	// Now begin the possible recursion of adding more and more beams based on whether the first
	// beam hits a prism block (and if its children also do)
	LevelPiece* lastCollisionPiece = NULL;
	std::list<BeamSegment*> newBeamSegs;

	// In the case where there's a sticky paddle the beam refracts through the sticky paddle a bit
	if ((this->paddle->GetPaddleType() & PlayerPaddle::StickyPaddle) == PlayerPaddle::StickyPaddle) {
		static int rotateBeamCenterBeamAmt;
		static int rotateBeamRefract1Amt;
		static int rotateBeamRefract2Amt;
		
		static float centerBeamRadiusAmt;
		static float beamRefract1RadiusAmt;
		static float beamRefract2RadiusAmt;

		if (this->reInitStickyPaddle) {
			rotateBeamCenterBeamAmt = static_cast<int>(Randomizer::GetInstance()->RandomNumNegOneToOne() * 20);
			rotateBeamRefract1Amt   = rotateBeamCenterBeamAmt + 10 + static_cast<int>(Randomizer::GetInstance()->RandomNumZeroToOne() * 40);
			rotateBeamRefract2Amt   = rotateBeamCenterBeamAmt - 10 - static_cast<int>(Randomizer::GetInstance()->RandomNumZeroToOne() * 40);
			
			centerBeamRadiusAmt   = (0.5f + 0.25f * Randomizer::GetInstance()->RandomNumZeroToOne()) ;
			beamRefract1RadiusAmt = (0.2f + 0.4f * Randomizer::GetInstance()->RandomNumZeroToOne());
			beamRefract2RadiusAmt = (0.2f + 0.4f * Randomizer::GetInstance()->RandomNumZeroToOne());

			this->reInitStickyPaddle = false;
		}

		firstBeamSeg = new BeamSegment(Collision::Ray2D(BEAM_ORIGIN, Vector2D::Rotate(rotateBeamCenterBeamAmt, BEAM_UNIT_DIR)), 
																   centerBeamRadiusAmt * INITIAL_BEAM_RADIUS, 
																	 centerBeamRadiusAmt * this->baseDamagePerSecond, NULL);

		Vector2D adjustBeamOriginAmt(paddle->GetHalfFlatTopWidth()*0.5, 0.0f);
		BeamSegment* refractSeg1 = new BeamSegment(Collision::Ray2D(BEAM_ORIGIN - adjustBeamOriginAmt, Vector2D::Rotate(rotateBeamRefract1Amt, BEAM_UNIT_DIR)), 
																						   beamRefract1RadiusAmt * INITIAL_BEAM_RADIUS, beamRefract1RadiusAmt * this->baseDamagePerSecond, NULL);
		newBeamSegs.push_back(refractSeg1);
		BeamSegment* refractSeg2 = new BeamSegment(Collision::Ray2D(BEAM_ORIGIN + adjustBeamOriginAmt, Vector2D::Rotate(rotateBeamRefract2Amt, BEAM_UNIT_DIR)), 
																							 beamRefract2RadiusAmt * INITIAL_BEAM_RADIUS, beamRefract2RadiusAmt * this->baseDamagePerSecond, NULL);
		newBeamSegs.push_back(refractSeg2);
	}
	else {
		firstBeamSeg = new BeamSegment(Collision::Ray2D(BEAM_ORIGIN, BEAM_UNIT_DIR), INITIAL_BEAM_RADIUS, this->baseDamagePerSecond, NULL);
		this->reInitStickyPaddle = true;
	}

	newBeamSegs.push_front(firstBeamSeg);

	// Keep track of the pieces collided with to watch out for bad loops (e.g., infinite loops of beams through prisms)
	std::set<const LevelPiece*> piecesCollidedWith;

	// Loop as long as there are new beam segments to add to this beam
	while (newBeamSegs.size() > 0) {

		// Grab the current beam segment from the front of the list of new segments
		BeamSegment* currBeamSegment = newBeamSegs.front();
		newBeamSegs.pop_front();

		// Fire the current beam segment into the level in order to figure out what piece it hit
		// If new beam segments are spawned add them to the list of new beam segments to deal with
		LevelPiece* pieceCollidedWith = currBeamSegment->FireBeamSegmentIntoLevel(level);
		
		// In a case where there was no piece colliding it means that the beam shot out of the level
		if (pieceCollidedWith != NULL) {

			// Check to see if the piece was already hit by this beam...
			std::pair<std::set<const LevelPiece*>::iterator, bool> insertResult = piecesCollidedWith.insert(pieceCollidedWith);
			if (insertResult.second) {

				// First time the level piece was hit by this beam - we are allowed to spawn more beams...
				// The piece may generate a set of spawned beams based on whether or not it reflects/refracts light
				const Collision::Ray2D currBeamRay = currBeamSegment->GetBeamSegmentRay();
				std::list<Collision::Ray2D> spawnedRays = pieceCollidedWith->GetReflectionRefractionRays(currBeamSegment->GetEndPoint(), currBeamRay.GetUnitDirection());

				// In the case where a portal block is collided with then we need to account for its sibling
				// as a collider for the new beam we spawn
				if (pieceCollidedWith->GetType() == LevelPiece::Portal) {
					PortalBlock* portalBlock = dynamic_cast<PortalBlock*>(pieceCollidedWith);
					assert(portalBlock != NULL);
					pieceCollidedWith = portalBlock->GetSiblingPortal();
					insertResult = piecesCollidedWith.insert(pieceCollidedWith);
				}

				// We only allow a new ray to spawn if a) there is just one ray to spawn (in which case it will have the same radius as the current beam segment)
				// or b) There are multiple new rays but the current beam segment has a suitable radius to be divided.
				if (spawnedRays.size() == 1 || (spawnedRays.size() > 1 && currBeamSegment->GetRadius() > Beam::MIN_BEAM_RADIUS)) {
					
					// The radius of the spawned beams will be a fraction of the current radius based
					// on the number of reflection/refraction rays
					const float NEW_BEAM_SEGMENT_RADIUS = currBeamSegment->GetRadius() / static_cast<float>(spawnedRays.size());
					const int   NEW_BEAM_DMG_PER_SECOND = std::max<int>(Beam::MIN_DMG_PER_SEC, currBeamSegment->GetDamagePerSecond() / spawnedRays.size());
					
					// Now add the new beams to the list of beams we need to fire into the level and repeat this whole process with
					std::list<BeamSegment*> spawnedBeamSegs;
					for (std::list<Collision::Ray2D>::iterator iter = spawnedRays.begin(); iter != spawnedRays.end(); ++iter) {
						newBeamSegs.push_back(new BeamSegment(*iter, NEW_BEAM_SEGMENT_RADIUS, NEW_BEAM_DMG_PER_SECOND, pieceCollidedWith));
					}
				}
			}
		}

		// The current beam segment has had its collisions taken care of and is now properly
		// setup, insert it into the list of beam segments associated with this beam
		this->beamParts.push_back(currBeamSegment);
	}

	// If there were no old beam segments then this is the first spawn of this beam
	// and we shouldn't proceed any further (other wise we will update a beam that doesn't exist yet)
	if (oldBeamSegments.size() == 0) {
		return;
	}

	// Check to see if there was any change...
	if (this->BeamHasChanged(oldBeamSegments, this->beamParts)) {
		// EVENT: Beam updated/changed
		GameEventManager::Instance()->ActionBeamChanged(*this);

		// Clean up the old beam
		this->CleanUpBeam(oldBeamSegments);
	}
	else {
		this->CleanUpBeam(this->beamParts);
		this->beamParts = oldBeamSegments;
	}

	
}

/**
 * Get the number of base beam segments (i.e., initial beams without any parents) for this beam.
 */
int PaddleLaserBeam::GetNumBaseBeamSegments() const {
	if ((this->paddle->GetPaddleType() & PlayerPaddle::StickyPaddle) == PlayerPaddle::StickyPaddle) {
		return 3;
	}
	return 1;
}
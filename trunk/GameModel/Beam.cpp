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
#include "PlayerPaddle.h"
#include "GameLevel.h"

const float Beam::MIN_BEAM_RADIUS = 0.25f;

Beam::Beam(BeamType type, int dmgPerSec, double lifeTimeInSec) : 
type(type), damagePerSecond(dmgPerSec), currTimeElapsed(0.0), totalLifeTime(lifeTimeInSec) {
};

Beam::~Beam() {
	this->CleanUpBeam();
}

// Deletes any beam segments currently associated with this beam
void Beam::CleanUpBeam() {
	for (std::list<Beam::BeamSegment*>::iterator iter = this->beamParts.begin(); iter != this->beamParts.end(); ++iter) {
		Beam::BeamSegment* currSeg = *iter;
		delete currSeg;
		currSeg = NULL;
	}
	this->beamParts.clear();
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
	for (std::list<Beam::BeamSegment*>::const_iterator iter = this->beamParts.begin(); iter != this->beamParts.end(); ++iter) {
		Beam::BeamSegment* currentSeg = *iter;
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
		this->currTimeElapsed += dT;
	}
	return false;
}

Beam::BeamSegment::BeamSegment(const Collision::Ray2D& beamRay, float beamRadius, LevelPiece* ignorePiece) :
timeSinceFired(0.0), ray(beamRay), radius(beamRadius), collidingPiece(NULL), endT(0.0f),
ignorePiece(ignorePiece) {
}

Beam::BeamSegment::~BeamSegment() {
	if (this->collidingPiece != NULL) {
		this->collidingPiece = NULL;
	}
}

/**
 * Fire this beam segment into the given level.
 * Returns: The level piece that this beam segment collided with when shot - NULL if no collisions.
 */
LevelPiece* Beam::BeamSegment::FireBeamSegmentIntoLevel(const GameLevel* level) {
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

const double PaddleLaserBeam::BEAM_EXPIRE_TIME_IN_SECONDS	= 5.0;
const int PaddleLaserBeam::DAMAGE_PER_SECOND							= 100;	// Damage per second that the paddle laser does to blocks and stuff

PaddleLaserBeam::PaddleLaserBeam(PlayerPaddle* paddle, const GameLevel* level) : 
Beam(Beam::PaddleLaserBeam, PaddleLaserBeam::DAMAGE_PER_SECOND, PaddleLaserBeam::BEAM_EXPIRE_TIME_IN_SECONDS), paddle(paddle) {
	assert((paddle->GetPaddleType() & PlayerPaddle::LaserBeamPaddle) == PlayerPaddle::LaserBeamPaddle);
	this->UpdateCollisions(level);
}

PaddleLaserBeam::~PaddleLaserBeam() {
	// Remove the laser beam effect from the paddle
	paddle->RemovePaddleType(PlayerPaddle::LaserBeamPaddle);
}

/**
 * Update the collisions for this beam - this will fire the current beam
 */
void PaddleLaserBeam::UpdateCollisions(const GameLevel* level) {
	assert(this->paddle != NULL);

	// Redo all of the collisions: wipe out any previous beams
	this->CleanUpBeam();

	// Create the very first beam part that shoots out of the paddle towards the level
	const float INITIAL_BEAM_RADIUS  = this->paddle->GetHalfFlatTopWidth();
	const Point2D BEAM_ORIGIN				 = this->paddle->GetCenterPosition() + Vector2D(0, this->paddle->GetHalfHeight());
	const Vector2D BEAM_UNIT_DIR		 = this->paddle->GetUpVector();
	Beam::BeamSegment* firstBeamSeg = new Beam::BeamSegment(Collision::Ray2D(BEAM_ORIGIN, BEAM_UNIT_DIR), INITIAL_BEAM_RADIUS, NULL);

	// Now begin the possible recursion of adding more and more beams based on whether the first
	// beam hits a prism block (and if its children also do)
	LevelPiece* lastCollisionPiece = NULL;
	std::list<Beam::BeamSegment*> newBeamSegs;
	newBeamSegs.push_back(firstBeamSeg);

	// Keep track of the pieces collided with to watch out for bad loops (e.g., infinite loops of beams through prisms)
	std::set<const LevelPiece*> piecesCollidedWith;

	// Loop as long as there are new beam segments to add to this beam
	while (newBeamSegs.size() > 0) {

		// Grab the current beam segment from the front of the list of new segments
		Beam::BeamSegment* currBeamSegment = newBeamSegs.front();
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
				
				// We only allow a new ray to spawn if a) there is just one ray to spawn (in which case it will have the same radius as the current beam segment)
				// or b) There are multiple new rays but the current beam segment has a suitable radius to be divided.
				if (spawnedRays.size() == 1 || (spawnedRays.size() > 1 && currBeamSegment->GetRadius() > Beam::MIN_BEAM_RADIUS)) {
					
					// The radius of the spawned beams will be a fraction of the current radius based
					// on the number of reflection/refraction rays
					const float NEW_BEAM_SEGMENT_RADIUS = currBeamSegment->GetRadius() / static_cast<float>(spawnedRays.size());
					
					// Now add the new beams to the list of beams we need to fire into the level and repeat this whole process with
					std::list<Beam::BeamSegment*> spawnedBeamSegs;
					for (std::list<Collision::Ray2D>::iterator iter = spawnedRays.begin(); iter != spawnedRays.end(); ++iter) {
						newBeamSegs.push_back(new Beam::BeamSegment(*iter, NEW_BEAM_SEGMENT_RADIUS, pieceCollidedWith));
					}
				}
			}
		}

		// The current beam segment has had its collisions taken care of and is now properly
		// setup, insert it into the list of beam segments associated with this beam
		this->beamParts.push_back(currBeamSegment);
	}
}
/**
 * Beam.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011 - 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "Beam.h"
#include "LevelPiece.h"
#include "PortalBlock.h"
#include "PlayerPaddle.h"
#include "GameModel.h"
#include "GameLevel.h"
#include "GameEventManager.h"

const double Beam::MIN_ALLOWED_LIFETIME_IN_SECS = 0.75;
const float Beam::MIN_BEAM_RADIUS = 0.05f;
const int   Beam::MIN_DMG_PER_SEC = 25;

Beam::Beam(int dmgPerSec, double lifeTimeInSec) : 
baseDamagePerSecond(dmgPerSec), currTimeElapsed(0.0), totalLifeTime(lifeTimeInSec), beamAlpha(1), beamAlphaDirty(true) {
    assert(lifeTimeInSec >= MIN_ALLOWED_LIFETIME_IN_SECS);
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
	// See if there are the same number of segments...
    if (oldBeamSegs.size() != newBeamSegs.size()) {
		return true;
	}
    // See if the alpha has changed
    if (this->beamAlphaDirty) {
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

void Beam::BuildAndUpdateCollisionsForBeamParts(const std::list<BeamSegment*>& initialBeamSegs, const GameModel* gameModel) {
    assert(gameModel != NULL);
   
    const GameLevel* level = gameModel->GetCurrentLevel();
    assert(level != NULL);

    // Keep a temporary list of all the beam segments that we are dealing with in the current
    // iteration of building all of the beam parts, we start with the provided initial beams segments
    std::list<BeamSegment*> newBeamSegs;
    newBeamSegs.insert(newBeamSegs.begin(), initialBeamSegs.begin(), initialBeamSegs.end());

    // Keep a copy of the old beam segments (for comparison afterwards)
    std::list<BeamSegment*> oldBeamSegments = this->beamParts;
    this->beamParts.clear();

    // Keep track of the pieces collided with to watch out for bad loops (e.g., infinite loops of beams through prisms)
    std::set<const LevelPiece*> piecesCollidedWith;

    // Loop as long as there are new beam segments to add to this beam
    while (newBeamSegs.size() > 0) {

        // Grab the current beam segment from the front of the list of new segments
        BeamSegment* currBeamSegment = newBeamSegs.front();
        newBeamSegs.pop_front();

        // Fire the current beam segment into the level in order to figure out what piece it hit
        // If new beam segments are spawned add them to the list of new beam segments to deal with
        // IMPORTANT NOTE / TODO: For now this MUST be done before checking for collisions with the paddle... combine all collisions
        // in a single function in the future!
        LevelPiece* pieceCollidedWith = currBeamSegment->FireBeamSegmentIntoLevel(level);

        // Check for collisions with the paddle too
        PlayerPaddle* playerPaddle = gameModel->GetPlayerPaddle();
        assert(playerPaddle != NULL);
        float minRayT = std::numeric_limits<float>::max();

        // Note: If the beam is a paddle laser beam and the beam segment is an origin
        // segment then we don't do collisions with the paddle! 
        // Also, if the beam has faded-out past a certain point then paddle collisions no longer happen
        if (this->GetType() == Beam::PaddleBeam && this->beamParts.size() < initialBeamSegs.size() || this->beamAlpha < 0.33f) {
            // No paddle collisions for the current beam segment!
        }
        else {
            // Fire multiple rays of the current beam segment...
            const Collision::Ray2D& beamSegRayMiddle = currBeamSegment->GetBeamSegmentRay();
            
            // Create other parallel rays to test for paddle collision...
            Vector2D perpendicularDir(-beamSegRayMiddle.GetUnitDirection()[1], beamSegRayMiddle.GetUnitDirection()[0]);
            Collision::Ray2D beamSegRayOuter1(beamSegRayMiddle.GetOrigin() + currBeamSegment->GetRadius() * perpendicularDir, beamSegRayMiddle.GetUnitDirection());
            Collision::Ray2D beamSegRayOuter2(beamSegRayMiddle.GetOrigin() - currBeamSegment->GetRadius() * perpendicularDir, beamSegRayMiddle.GetUnitDirection());

            float tempRayT;
            playerPaddle->CollisionCheck(beamSegRayMiddle, minRayT);
            playerPaddle->CollisionCheck(beamSegRayOuter1, tempRayT);
            minRayT = std::min<float>(minRayT, tempRayT);
            playerPaddle->CollisionCheck(beamSegRayOuter2, tempRayT);
            minRayT = std::min<float>(minRayT, tempRayT);
        }

        minRayT = std::max<float>(0.0f, minRayT);
        if (minRayT <= currBeamSegment->GetLength()) {
            // Set the new beam ending point - This MUST be done before telling the paddle it was hit!!
            currBeamSegment->SetLength(minRayT);
            // There's a collision with the paddle, stop all collisions here
            playerPaddle->HitByBeam(*this, *currBeamSegment);
        }
        else if (pieceCollidedWith != NULL) {
            // There was a piece that the beam collided with...

            // Check to see if the piece was already hit by this beam...
            std::pair<std::set<const LevelPiece*>::iterator, bool> insertResult = piecesCollidedWith.insert(pieceCollidedWith);
            if (insertResult.second) {

                // First time the level piece was hit by this beam - we are allowed to spawn more beams...
                // The piece may generate a set of spawned beams based on whether or not it reflects/refracts light
                const Collision::Ray2D& currBeamRay = currBeamSegment->GetBeamSegmentRay();
                std::list<Collision::Ray2D> spawnedRays;
                pieceCollidedWith->GetReflectionRefractionRays(currBeamSegment->GetEndPoint(), currBeamRay.GetUnitDirection(), spawnedRays);

                // In the case where a portal block is collided with then we need to account for its sibling
                // as a collider for the new beam we spawn
                if (pieceCollidedWith->GetType() == LevelPiece::Portal) {
                    PortalBlock* portalBlock = static_cast<PortalBlock*>(pieceCollidedWith);
                    assert(portalBlock != NULL);
                    pieceCollidedWith = portalBlock->GetSiblingPortal();
                    insertResult = piecesCollidedWith.insert(pieceCollidedWith);
                }

                if (spawnedRays.size() >= 1) {
                    // The radius of the spawned beams will be a fraction of the current radius based
                    // on the number of reflection/refraction rays
                    const float NEW_BEAM_SEGMENT_RADIUS = this->beamAlpha * std::max<float>(currBeamSegment->GetRadius() / 
                        static_cast<float>(spawnedRays.size()), Beam::MIN_BEAM_RADIUS);

                    if (NEW_BEAM_SEGMENT_RADIUS > 0.0f) {
                        const int NEW_BEAM_DMG_PER_SECOND = this->beamAlpha * std::max<int>(Beam::MIN_DMG_PER_SEC, 
                            currBeamSegment->GetDamagePerSecond() / spawnedRays.size());

                        // Now add the new beams to the list of beams we need to fire into the level and repeat this whole process with
                        std::list<BeamSegment*> spawnedBeamSegs;
                        for (std::list<Collision::Ray2D>::iterator iter = spawnedRays.begin(); iter != spawnedRays.end(); ++iter) {

                            newBeamSegs.push_back(new BeamSegment(*iter, NEW_BEAM_SEGMENT_RADIUS, NEW_BEAM_DMG_PER_SECOND, pieceCollidedWith));
                        }
                    }
                }
            }
        }
        else {
            // In a case where there was no piece colliding it means that the beam shot out of the level
        }

        // TODO: Bosses... ?

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

    this->beamAlphaDirty = false;
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
bool Beam::Tick(double dT, const GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);

	if (this->currTimeElapsed >= this->totalLifeTime) {
		return true;
	}
	else {
		// Tick all the segments...
		for (std::list<BeamSegment*>::iterator iter = this->beamParts.begin(); iter != this->beamParts.end(); ++iter) {
			(*iter)->Tick(dT);
		}
		this->currTimeElapsed += dT;
        this->currTimeElapsed = std::min<float>(this->currTimeElapsed, this->totalLifeTime);
	}

    this->TickAlpha();

	return false;
}

void Beam::TickAlpha() {
    float prevAlpha = this->beamAlpha;
    if (this->currTimeElapsed < this->totalLifeTime - MIN_ALLOWED_LIFETIME_IN_SECS) {
        this->beamAlpha = 1.0f;
    }
    else {
        this->beamAlpha = NumberFuncs::LerpOverTime(this->totalLifeTime - MIN_ALLOWED_LIFETIME_IN_SECS, 
            this->totalLifeTime, 1.0f, 0.0f, this->currTimeElapsed);
    }
    assert(this->beamAlpha >= 0.0f && this->beamAlpha <= 1.0f);

    if (prevAlpha != this->beamAlpha) {
        this->beamAlphaDirty = true;
    }
}

BeamSegment::BeamSegment(const Collision::Ray2D& beamRay, float beamRadius, int beamDmgPerSec, LevelPiece* ignorePiece) :
timeSinceFired(0.0), ray(beamRay), collidingPiece(NULL), endT(0.0f),
ignorePiece(ignorePiece), damagePerSecond(beamDmgPerSec) {

	this->SetRadius(beamRadius);
}
void BeamSegment::SetRadius(float radius) { 
	this->radius = radius;  

    static const int NUM_ANIMATION_CHANGES = 5;

	std::vector<double> animationTimes;
	animationTimes.reserve(1 + 2 * NUM_ANIMATION_CHANGES);
    animationTimes.push_back(0.0);
    for (int i = 0; i < NUM_ANIMATION_CHANGES; i++) {
        animationTimes.push_back(animationTimes.back() + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.1f + 0.05f);
	    animationTimes.push_back(animationTimes.back() + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.125f + 0.1f);
    }

	std::vector<float> animationRadii;
    animationRadii.reserve(animationTimes.size());
	animationRadii.push_back(this->radius);
    for (int i = 0; i < NUM_ANIMATION_CHANGES; i++) {
	    animationRadii.push_back((Randomizer::GetInstance()->RandomNumZeroToOne() * 0.2f + 0.75f) * this->radius);
	    animationRadii.push_back(this->radius);
    }

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
    else {
        if (this->endT < 0.0) {
            this->endT = 0;
        }
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

	if (fabs(beamSeg1.GetLength() - beamSeg2.GetLength()) > 0.001f) {
		return false;
	}

	if (beamSeg1.GetCollidingPiece() != beamSeg2.GetCollidingPiece()) {
		return false;
	}


	return true;
}
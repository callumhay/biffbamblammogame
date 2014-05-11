/**
 * Beam.cpp
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

#include "Beam.h"
#include "LevelPiece.h"
#include "PortalBlock.h"
#include "PlayerPaddle.h"
#include "GameModel.h"
#include "GameLevel.h"
#include "GameEventManager.h"
#include "BeamColliderStrategy.h"


const double Beam::MIN_ALLOWED_LIFETIME_IN_SECS = 0.75;
const float Beam::MIN_BEAM_RADIUS = 0.05f;
const int   Beam::MIN_DMG_PER_SEC = 25;

Beam::Beam(int dmgPerSec, double lifeTimeInSec) : 
baseDamagePerSecond(dmgPerSec), currTimeElapsed(0.0), totalLifeTime(lifeTimeInSec), 
beamAlpha(1), zOffset(0), beamAlphaDirty(true) {
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
    std::set<const void*> thingsCollidedWith;

    // Loop as long as there are new beam segments to add to this beam
    while (newBeamSegs.size() > 0) {

        // Grab the current beam segment from the front of the list of new segments
        BeamSegment* currBeamSegment = newBeamSegs.front();
        newBeamSegs.pop_front();

        // Note: If the beam is a paddle laser beam and the beam segment is an origin
        // segment then we don't do collisions with the paddle! 
        // Also, if the beam has faded-out past a certain point then paddle collisions no longer happen
        bool cannotCollideWithPaddle = (this->GetType() == Beam::PaddleBeam && this->beamParts.size() < initialBeamSegs.size() || this->beamAlpha < 0.33f);

        // Fire the current beam segment into the level in order to figure out what piece it hit
        
        BeamColliderStrategy* collisionStrategy = currBeamSegment->FireBeamSegmentIntoLevel(*gameModel, cannotCollideWithPaddle);
        if (collisionStrategy != NULL) {
            
            // The beam collided with something:
            // If new beam segments are spawned or any other special stuff needs to happen, it will be dealt with by
            // the particular collision strategy...
            collisionStrategy->UpdateBeam(*this, currBeamSegment, thingsCollidedWith, newBeamSegs);
            delete collisionStrategy;
            collisionStrategy = NULL;
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
        this->currTimeElapsed = std::min<double>(this->currTimeElapsed, this->totalLifeTime);
	}

    this->TickAlpha();

	return false;
}

void Beam::TickAlpha() {
    float prevAlpha = this->beamAlpha;
    if (this->currTimeElapsed < this->totalLifeTime - MIN_ALLOWED_LIFETIME_IN_SECS) {
        this->beamAlpha = 1.0f;
    }
    else if (this->currTimeElapsed > this->totalLifeTime) {
        this->currTimeElapsed = this->totalLifeTime;
        this->beamAlpha = 0.0f;
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

void Beam::UpdateOriginBeamSegment(const GameModel* gameModel, const Collision::Ray2D& newOriginBeamRay) {
    if (this->beamParts.empty()) {
        assert(false);
        return;
    }

    BeamSegment* originSeg = this->beamParts.front();
    assert(originSeg != NULL);
    originSeg->SetBeamSegmentRay(newOriginBeamRay);

    this->UpdateCollisions(gameModel);
}

BeamSegment::BeamSegment(const Collision::Ray2D& beamRay, float beamRadius, 
                         int beamDmgPerSec, const void* ignoreThing) :
timeSinceFired(0.0), ray(beamRay), collidingPiece(NULL), collidingProjectile(NULL), endT(0.0f),
ignoreThing(ignoreThing), damagePerSecond(beamDmgPerSec), initialRadius(beamRadius) {

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
    this->collidingPiece = NULL;
    this->collidingProjectile = NULL;
}

void BeamSegment::Tick(double dT) {
	this->radiusPulseAnim.Tick(dT);
}

bool SmallestToLargestRayTStratSorter(const std::pair<float, BeamColliderStrategy*>& a, const std::pair<float, BeamColliderStrategy*>& b) {
    return (a.first < b.first);
}

/**
 * Fire this beam segment into the given level.
 * Returns: The level piece that this beam segment collided with when shot - NULL if no collisions.
 */
BeamColliderStrategy* BeamSegment::FireBeamSegmentIntoLevel(const GameModel& gameModel, 
                                                            bool cannotCollideWithPaddle) {

	const GameLevel* level = gameModel.GetCurrentLevel();
    assert(level != NULL);

    const float maxRayT = std::max<float>(level->GetLevelUnitHeight(), level->GetLevelUnitWidth());

	std::set<const void*> ignoreThings;
	ignoreThings.insert(this->ignoreThing);

    std::vector<std::pair<float, BeamColliderStrategy*> > strategies;
    strategies.reserve(3);

    // Check for colliding pieces...
    float tempT = std::numeric_limits<float>::max();
    this->collidingPiece = level->GetLevelPieceFirstCollider(this->ray, ignoreThings, tempT);
    if (this->collidingPiece != NULL) {
        strategies.push_back(std::make_pair(std::max<float>(0,tempT), new LevelPieceBeamColliderStrategy(tempT, this->collidingPiece)));
    }

    // Check for colliding paddle...
    if (!cannotCollideWithPaddle) {
        // Check for collision with the paddle as well...
        PlayerPaddle* playerPaddle = gameModel.GetPlayerPaddle();
        assert(playerPaddle != NULL);
        
        // Fire multiple rays of the current beam segment...
        // Create other parallel rays to test for paddle collision...
        Vector2D perpendicularDir(-this->ray.GetUnitDirection()[1], this->ray.GetUnitDirection()[0]);
        Collision::Ray2D beamSegRayOuter1(this->ray.GetOrigin() + this->GetRadius() * perpendicularDir, this->ray.GetUnitDirection());
        Collision::Ray2D beamSegRayOuter2(this->ray.GetOrigin() - this->GetRadius() * perpendicularDir, this->ray.GetUnitDirection());

        float minRayT = std::numeric_limits<float>::max();
        tempT = std::numeric_limits<float>::max();
        bool isCollision = false;
        isCollision |= playerPaddle->CollisionCheck(this->ray, minRayT);
        isCollision |= playerPaddle->CollisionCheck(beamSegRayOuter1, tempT);
        minRayT = std::min<float>(minRayT, tempT);
        isCollision |= playerPaddle->CollisionCheck(beamSegRayOuter2, tempT);
        minRayT = std::min<float>(minRayT, tempT);
        
        if (isCollision) {
            strategies.push_back(std::make_pair(std::max<float>(0,minRayT), new PaddleBeamColliderStrategy(maxRayT, playerPaddle)));
        }
    }
    
    // Check for colliding projectiles...
    tempT = std::numeric_limits<float>::max();
    this->collidingProjectile = gameModel.GetFirstBeamProjectileCollider(this->ray, ignoreThings, tempT);
    if (this->collidingProjectile != NULL) {
        strategies.push_back(std::make_pair(std::max<float>(0,tempT), new ProjectileBeamColliderStrategy(tempT, this->collidingProjectile)));
    }

    if (strategies.empty()) {
        this->endT = maxRayT;
        return NULL;
    }

    std::sort(strategies.begin(), strategies.end(), SmallestToLargestRayTStratSorter);

    // Delete all the failures...
    for (int i = 1; i < static_cast<int>(strategies.size()); i++) {
        delete strategies[i].second;
    }

    BeamColliderStrategy* strategy = strategies.front().second;
    assert(strategy != NULL);
    this->endT = strategies.front().first;
    this->collidingPiece = strategy->GetCollidingPiece();
    this->collidingProjectile = strategy->GetCollidingProjectile();

    return strategy;
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
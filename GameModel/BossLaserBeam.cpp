/**
 * BossLaserBeam.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "BossLaserBeam.h"
#include "GameEventManager.h"

const double BossLaserBeam::BEAM_EXPIRE_TIME_IN_SECONDS = 3.25; // Length of time for the beam to be firing
const int BossLaserBeam::BASE_DAMAGE_PER_SECOND         = 0;    // Damage per second that the beam does to blocks and stuff																															// NOTE: a typical block has about 100 life

BossLaserBeam::BossLaserBeam(const Collision::Ray2D& initialBeamRay, float initialBeamRadius, const GameModel* gameModel) : 
Beam(BossLaserBeam::BASE_DAMAGE_PER_SECOND, BossLaserBeam::BEAM_EXPIRE_TIME_IN_SECONDS), 
initialBeamRay(initialBeamRay), initialBeamRadius(initialBeamRadius) {
	this->UpdateCollisions(gameModel);
}

BossLaserBeam::~BossLaserBeam() {
}

void BossLaserBeam::UpdateCollisions(const GameModel* gameModel) {
    assert(gameModel != NULL);

    std::list<BeamSegment*> initialBeamSegs;
    initialBeamSegs.push_back(new BeamSegment(this->initialBeamRay, this->beamAlpha * this->initialBeamRadius, this->beamAlpha * BASE_DAMAGE_PER_SECOND, NULL));

    this->BuildAndUpdateCollisionsForBeamParts(initialBeamSegs, gameModel);
}

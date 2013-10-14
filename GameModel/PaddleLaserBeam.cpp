/**
 * PaddleLaserBeam.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "PaddleLaserBeam.h"
#include "PlayerPaddle.h"
#include "LevelPiece.h"
#include "PortalBlock.h"
#include "GameEventManager.h"
#include "GameModel.h"

const double PaddleLaserBeam::BEAM_EXPIRE_TIME_IN_SECONDS = 12;  // Length of time for the beam to be firing
const int PaddleLaserBeam::BASE_DAMAGE_PER_SECOND         = 100; // Damage per second that the paddle laser does to blocks and stuff																															// NOTE: a typical block has about 100 life

PaddleLaserBeam::PaddleLaserBeam(PlayerPaddle* paddle, const GameModel* gameModel) : 
Beam(PaddleLaserBeam::BASE_DAMAGE_PER_SECOND, PaddleLaserBeam::BEAM_EXPIRE_TIME_IN_SECONDS), 
paddle(paddle), reInitStickyPaddle(true) {
    
    assert(paddle != NULL);
    assert(gameModel != NULL);

	this->UpdateCollisions(gameModel);
	paddle->SetIsLaserBeamFiring(true);
}

PaddleLaserBeam::~PaddleLaserBeam() {
	// Remove the paddle laser beam...
	this->paddle->RemovePaddleType(PlayerPaddle::LaserBeamPaddle);
	this->paddle->SetIsLaserBeamFiring(false);
}

/**
 * Get the number of base beam segments (i.e., initial beams without any parents) for this beam.
 */
int PaddleLaserBeam::GetNumBaseBeamSegments() const {
	if (this->paddle->HasPaddleType(PlayerPaddle::StickyPaddle)) {
		return 3;
	}
	return 1;
}

bool PaddleLaserBeam::Tick(double dT, const GameModel* gameModel) {

    // Don't tick the beam if the paddle isn't active in the game...
    if (this->paddle->HasBeenPausedAndRemovedFromGame(gameModel->GetPauseState())) {
        this->paddle->SetIsLaserBeamFiring(false);
        this->TickAlpha();
        return false;
    }
    else {
        this->paddle->SetIsLaserBeamFiring(true);
    }

    return Beam::Tick(dT, gameModel);
}

void PaddleLaserBeam::TickAlpha() {
    Beam::TickAlpha();
    if (!this->paddle->GetIsPaddleCameraOn()) {
        this->beamAlpha = std::min<float>(this->paddle->GetAlpha(), this->beamAlpha);
    }
}

/**
 * Update the collisions for this beam - this will fire the current beam
 */
void PaddleLaserBeam::UpdateCollisions(const GameModel* gameModel) {
	assert(this->paddle != NULL);
    assert(gameModel != NULL);

    // If the paddle is removed from the game then we don't do any updates...
    if (this->paddle->HasBeenPausedAndRemovedFromGame(gameModel->GetPauseState())) {
        
        // Remove all of the beam parts, the beam is not active right now!
        std::list<BeamSegment*> oldBeamSegments = this->beamParts;
        this->CleanUpBeam(this->beamParts);
        
        if (this->BeamHasChanged(oldBeamSegments, this->beamParts)) {
            // EVENT: Beam updated/changed
            GameEventManager::Instance()->ActionBeamChanged(*this);
        }

        return;
    }

	// Create the very first beam part that shoots out of the paddle towards the level
	const float INITIAL_BEAM_RADIUS  = this->beamAlpha * this->paddle->GetHalfFlatTopWidth()/2.0f;
	const Point2D BEAM_ORIGIN        = this->paddle->GetCenterPosition() + Vector2D(0, this->paddle->GetHalfHeight());
	const Vector2D BEAM_UNIT_DIR     = this->paddle->GetUpVector();

	// The current damage is determined by a ratio of the normal paddle width to its current width and
	// the typical base-damage that this beam does
	this->baseDamagePerSecond = this->beamAlpha * ((2.0f * this->paddle->GetHalfFlatTopWidth()) / PlayerPaddle::PADDLE_WIDTH_FLAT_TOP) * PaddleLaserBeam::BASE_DAMAGE_PER_SECOND;
	
	BeamSegment* firstBeamSeg = NULL;
	std::list<BeamSegment*> initialBeamSegs;

	// In the case where there's a sticky paddle the beam refracts through the sticky paddle a bit
	if (this->paddle->HasPaddleType(PlayerPaddle::StickyPaddle)) {

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
		    centerBeamRadiusAmt * INITIAL_BEAM_RADIUS, centerBeamRadiusAmt * this->baseDamagePerSecond, NULL);

		Vector2D adjustBeamOriginAmt(paddle->GetHalfFlatTopWidth()*0.5, 0.0f);
		BeamSegment* refractSeg1 = new BeamSegment(Collision::Ray2D(BEAM_ORIGIN - adjustBeamOriginAmt, Vector2D::Rotate(rotateBeamRefract1Amt, BEAM_UNIT_DIR)), 
		    beamRefract1RadiusAmt * INITIAL_BEAM_RADIUS, beamRefract1RadiusAmt * this->baseDamagePerSecond, NULL);
		initialBeamSegs.push_back(refractSeg1);
		BeamSegment* refractSeg2 = new BeamSegment(Collision::Ray2D(BEAM_ORIGIN + adjustBeamOriginAmt, Vector2D::Rotate(rotateBeamRefract2Amt, BEAM_UNIT_DIR)), 
		    beamRefract2RadiusAmt * INITIAL_BEAM_RADIUS, beamRefract2RadiusAmt * this->baseDamagePerSecond, NULL);
		initialBeamSegs.push_back(refractSeg2);
	}
	else {
		firstBeamSeg = new BeamSegment(Collision::Ray2D(BEAM_ORIGIN, BEAM_UNIT_DIR), INITIAL_BEAM_RADIUS, this->baseDamagePerSecond, NULL);
		this->reInitStickyPaddle = true;
	}

	initialBeamSegs.push_front(firstBeamSeg);

    // Build the rest of the beam using the initial beam segments
    this->BuildAndUpdateCollisionsForBeamParts(initialBeamSegs, gameModel);
}

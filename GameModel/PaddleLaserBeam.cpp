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
const int PaddleLaserBeam::BASE_DAMAGE_PER_SECOND         = 115; // Damage per second that the paddle laser does to blocks and stuff																															// NOTE: a typical block has about 100 life

PaddleLaserBeam::PaddleLaserBeam(PlayerPaddle* paddle, const GameModel* gameModel) : 
Beam(PaddleLaserBeam::BASE_DAMAGE_PER_SECOND, PaddleLaserBeam::BEAM_EXPIRE_TIME_IN_SECONDS), paddle(paddle) {
    
    assert(paddle != NULL);
    assert(gameModel != NULL);

	this->UpdateCollisions(gameModel);
	paddle->SetIsLaserBeamFiring(true);
}

PaddleLaserBeam::~PaddleLaserBeam() {
    StickyPaddleBeamDirGenerator::ReinitializeBeams();

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
    float prevAlpha = this->beamAlpha;

    if (this->currTimeElapsed < this->totalLifeTime - MIN_ALLOWED_LIFETIME_IN_SECS) {
        this->beamAlpha = 1.0f;
    }
    else {
        this->beamAlpha = NumberFuncs::LerpOverTime(this->totalLifeTime - MIN_ALLOWED_LIFETIME_IN_SECS, 
            this->totalLifeTime, 1.0f, 0.0f, this->currTimeElapsed);
    }
    assert(this->beamAlpha >= 0.0f && this->beamAlpha <= 1.0f);

    if (!this->paddle->GetIsPaddleCameraOn()) {
        if (this->paddle->GetAlpha() < 1.0f) {
            this->beamAlpha = this->paddle->GetAlpha();
        }
    }

    if (prevAlpha != this->beamAlpha) {
        this->beamAlphaDirty = true;
    }
}

/**
 * Update the collisions for this beam - this will fire the current beam
 */
void PaddleLaserBeam::UpdateCollisions(const GameModel* gameModel) {
	assert(this->paddle != NULL);
    assert(gameModel != NULL);

    std::list<BeamSegment*> initialBeamSegs;

	// Create the very first beam part that shoots out of the paddle towards the level
	const float INITIAL_BEAM_RADIUS  = this->beamAlpha * PaddleLaserBeam::GetInitialBeamRadius(*this->paddle);
    const Point2D BEAM_ORIGIN        = this->paddle->GetCenterPosition() + Vector2D(0, this->paddle->GetHalfHeight());
    const Vector2D BEAM_UNIT_DIR     = this->paddle->GetUpVector();

    // The current damage is determined by a ratio of the normal paddle width to its current width and
    // the typical base-damage that this beam does
    this->baseDamagePerSecond = this->beamAlpha * ((2.0f * this->paddle->GetHalfFlatTopWidth()) / 
        PlayerPaddle::PADDLE_WIDTH_FLAT_TOP) * PaddleLaserBeam::BASE_DAMAGE_PER_SECOND;
	
    BeamSegment* firstBeamSeg = NULL;

    // In the case where there's a sticky paddle the beam refracts through the sticky paddle a bit
    if (this->paddle->HasPaddleType(PlayerPaddle::StickyPaddle)) {

        Vector2D centerVec, leftVec, rightVec;
        float centerSize, leftSize, rightSize;
	    StickyPaddleBeamDirGenerator::GetBeamValues(BEAM_UNIT_DIR, centerVec, leftVec, rightVec, centerSize, leftSize, rightSize);

	    firstBeamSeg = new BeamSegment(Collision::Ray2D(BEAM_ORIGIN, centerVec), 
	        centerSize * INITIAL_BEAM_RADIUS, centerSize * this->baseDamagePerSecond, NULL);

	    Vector2D adjustBeamOriginAmt(PaddleLaserBeam::GetStickyPaddleOriginBeamSpacing(*this->paddle), 0.0f);
	    
        BeamSegment* refractSeg1 = new BeamSegment(Collision::Ray2D(BEAM_ORIGIN - adjustBeamOriginAmt, leftVec), 
	        leftSize * INITIAL_BEAM_RADIUS, leftSize * this->baseDamagePerSecond, NULL);
	    initialBeamSegs.push_back(refractSeg1);
	    
        BeamSegment* refractSeg2 = new BeamSegment(Collision::Ray2D(BEAM_ORIGIN + adjustBeamOriginAmt, rightVec), 
	        rightSize * INITIAL_BEAM_RADIUS, rightSize * this->baseDamagePerSecond, NULL);
	    initialBeamSegs.push_back(refractSeg2);
    }
    else {
	    firstBeamSeg = new BeamSegment(Collision::Ray2D(BEAM_ORIGIN, BEAM_UNIT_DIR), INITIAL_BEAM_RADIUS, this->baseDamagePerSecond, NULL);
        StickyPaddleBeamDirGenerator::ReinitializeBeams();
    }

    initialBeamSegs.push_front(firstBeamSeg);
    
    // Build the rest of the beam using the initial beam segments
    this->BuildAndUpdateCollisionsForBeamParts(initialBeamSegs, gameModel);
}

float PaddleLaserBeam::GetInitialBeamRadius(const PlayerPaddle& paddle) {
    return paddle.GetHalfFlatTopWidth() / 2.0f;
}

float PaddleLaserBeam::GetStickyPaddleOriginBeamSpacing(const PlayerPaddle& paddle) {
    return paddle.GetHalfFlatTopWidth() * 0.5;
}


bool StickyPaddleBeamDirGenerator::isInit = false;

int StickyPaddleBeamDirGenerator::rotateCenterBeamAmt = 0;
int StickyPaddleBeamDirGenerator::rotateLeftBeamAmt   = 0;
int StickyPaddleBeamDirGenerator::rotateRightBeamAmt  = 0;

float StickyPaddleBeamDirGenerator::stickyPaddleOriginBeamSizeCenter = 0.0f;
float StickyPaddleBeamDirGenerator::stickyPaddleOriginBeamSizeLeft   = 0.0f;
float StickyPaddleBeamDirGenerator::stickyPaddleOriginBeamSizeRight  = 0.0f;

void StickyPaddleBeamDirGenerator::ReinitializeBeams() {

    rotateCenterBeamAmt = static_cast<int>(Randomizer::GetInstance()->RandomNumNegOneToOne() * 20);
    rotateLeftBeamAmt   = rotateCenterBeamAmt + 10 + static_cast<int>(Randomizer::GetInstance()->RandomNumZeroToOne() * 40);
    rotateRightBeamAmt  = rotateCenterBeamAmt - 10 - static_cast<int>(Randomizer::GetInstance()->RandomNumZeroToOne() * 40);

    stickyPaddleOriginBeamSizeCenter = (0.5f + 0.25f * Randomizer::GetInstance()->RandomNumZeroToOne());
    stickyPaddleOriginBeamSizeLeft   = (0.2f + 0.4f * Randomizer::GetInstance()->RandomNumZeroToOne());
    stickyPaddleOriginBeamSizeRight  = (0.2f + 0.4f * Randomizer::GetInstance()->RandomNumZeroToOne());
}

void StickyPaddleBeamDirGenerator::GetBeamValues(const Vector2D& upVec, Vector2D& centerVec, Vector2D& leftVec, Vector2D& rightVec,
                                                 float& centerSize, float& leftSize, float& rightSize) {

    if (!isInit) {
        ReinitializeBeams();
        isInit = true;
    }

    centerVec = Vector2D::Rotate(rotateCenterBeamAmt, upVec);
    leftVec   = Vector2D::Rotate(rotateLeftBeamAmt,   upVec);
    rightVec  = Vector2D::Rotate(rotateRightBeamAmt,  upVec);

    centerSize = stickyPaddleOriginBeamSizeCenter;
    leftSize   = stickyPaddleOriginBeamSizeLeft;
    rightSize  = stickyPaddleOriginBeamSizeRight;
}
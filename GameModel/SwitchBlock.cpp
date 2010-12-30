/**
 * SwitchBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "SwitchBlock.h"
#include "GameModel.h"
#include "GameLevel.h"
#include "Beam.h"

// Constant for the amount of time it takes for the switch to become activatable again, after
// being turned on
const float SwitchBlock::RESET_TIME = 1.0f;

const float SwitchBlock::SWITCH_WIDTH   = 1.5f;
const float SwitchBlock::SWITCH_HEIGHT  = 0.3f;

const int SwitchBlock::TOGGLE_ON_OFF_LIFE_POINTS = 150;

SwitchBlock::SwitchBlock(unsigned int wLoc, unsigned int hLoc) : 
LevelPiece(wLoc, hLoc), timeOfLastSwitchPress(0), lifePointsUntilNextToggle(SwitchBlock::TOGGLE_ON_OFF_LIFE_POINTS) {
}

SwitchBlock::~SwitchBlock() {
}

bool SwitchBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	return Collision::IsCollision(ray, this->GetAABB(), rayT);
}

void SwitchBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                               const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                               const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                               const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {
    
	// If the tesla block is in ice then its bounds are a basic rectangle...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		LevelPiece::UpdateBounds(leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
		return;
	}

	// Set the bounding lines for a bevelled block...
	std::vector<Collision::LineSeg2D> boundingLines;
	std::vector<Vector2D>  boundingNorms;

    static const float HALF_SWITCH_WIDTH  = SWITCH_WIDTH / 2.0f;
    static const float HALF_SWITCH_HEIGHT = SWITCH_HEIGHT / 2.0f;

    // Important points for the Left and right switches
    Point2D leftTopSwitchPt     = this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, HALF_SWITCH_HEIGHT);
    Point2D leftBottomSwitchPt  = leftTopSwitchPt + Vector2D(0, -SWITCH_HEIGHT);
    Point2D rightTopSwitchPt    = this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, HALF_SWITCH_HEIGHT);
    Point2D rightBottomSwitchPt = rightTopSwitchPt + Vector2D(0, -SWITCH_HEIGHT);

    // Important points for the top and bottom switches
    Point2D topleftSwitchPt     = this->center + Vector2D(-HALF_SWITCH_WIDTH, LevelPiece::HALF_PIECE_HEIGHT);
    Point2D topRightSwitchPt    = topleftSwitchPt + Vector2D(SWITCH_WIDTH, 0);
    Point2D bottomLeftSwitchPt  = this->center + Vector2D(-HALF_SWITCH_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT);
    Point2D bottomRightSwitchPt = bottomLeftSwitchPt + Vector2D(SWITCH_WIDTH, 0);

	// Left boundry of the piece
	if (leftNeighbor != NULL) {
        Collision::LineSeg2D l1(leftTopSwitchPt, leftBottomSwitchPt);
		Vector2D n1(-1, 0);
		boundingLines.push_back(l1);
		boundingNorms.push_back(n1);
	}

	// Bottom boundry of the piece
	if (bottomNeighbor != NULL) {
		Collision::LineSeg2D l2(bottomLeftSwitchPt, bottomRightSwitchPt);
		Vector2D n2(0, -1);
		boundingLines.push_back(l2);
		boundingNorms.push_back(n2);
	}

	// Right boundry of the piece
	if (rightNeighbor != NULL) {
		Collision::LineSeg2D l3(rightTopSwitchPt, rightBottomSwitchPt);
		Vector2D n3(1, 0);
		boundingLines.push_back(l3);
		boundingNorms.push_back(n3);
	}

	// Top boundry of the piece
	if (topNeighbor != NULL) {
		Collision::LineSeg2D l4(topleftSwitchPt, topRightSwitchPt);
		Vector2D n4(0, 1);
		boundingLines.push_back(l4);
		boundingNorms.push_back(n4);
	}

    // Finally add all of the bevels at the edges (4 of them)
    Collision::LineSeg2D topLeftBevelLine(leftTopSwitchPt, topleftSwitchPt);
    Collision::LineSeg2D topRightBevelLine(topRightSwitchPt, rightTopSwitchPt);
    Collision::LineSeg2D bottomLeftBevelLine(leftBottomSwitchPt, bottomLeftSwitchPt);
    Collision::LineSeg2D bottomRightBevelLine(rightBottomSwitchPt, bottomRightSwitchPt);
    
    boundingLines.push_back(topLeftBevelLine);
    boundingNorms.push_back(Vector2D(-1.0f, 1.0f) / SQRT_2);

    boundingLines.push_back(topRightBevelLine);
    boundingNorms.push_back(Vector2D(1.0f, 1.0f) / SQRT_2);

    boundingLines.push_back(bottomLeftBevelLine);
    boundingNorms.push_back(Vector2D(-1.0f, -1.0f) / SQRT_2);

    boundingLines.push_back(bottomRightBevelLine);
    boundingNorms.push_back(Vector2D(1.0f, -1.0f) / SQRT_2);
    
	this->SetBounds(BoundingLines(boundingLines, boundingNorms), leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
		 			topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}


LevelPiece* SwitchBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
    this->SwitchPressed(gameModel);
    ball.SetLastPieceCollidedWith(NULL);
    return this;
}

LevelPiece* SwitchBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
    // Figure out which switch(es) is/are being affected by the projectile
    LevelPiece* resultingPiece = this;
	switch (projectile->GetType()) {
		case Projectile::PaddleLaserBulletProjectile:
			this->SwitchPressed(gameModel);
			break;
		
		case Projectile::CollateralBlockProjectile:
			this->SwitchPressed(gameModel);
			break;

		case Projectile::PaddleRocketBulletProjectile: 
			// The rocket should not destroy this block, however it certainly
			// is allowed to destroy blocks around it!
			resultingPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, projectile, this);
            assert(resultingPiece == this);
            this->SwitchPressed(gameModel);
			break;

		case Projectile::FireGlobProjectile:
			// Fire glob will just extinguish
			break;

		default:
			assert(false);
			break;
	}

	return resultingPiece;
}

LevelPiece* SwitchBlock::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle) {
    UNUSED_PARAMETER(paddle);
    this->SwitchPressed(gameModel);
    return this;
}

LevelPiece* SwitchBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	assert(gameModel != NULL);

	this->lifePointsUntilNextToggle -= static_cast<float>(dT * static_cast<double>(beamSegment->GetDamagePerSecond()));
	if (this->lifePointsUntilNextToggle <= 0) {
		// Reset the hit points for the next toggle
		this->lifePointsUntilNextToggle = SwitchBlock::TOGGLE_ON_OFF_LIFE_POINTS;
		this->SwitchPressed(gameModel);
	}

	return this;
}

void SwitchBlock::SwitchPressed(GameModel* gameModel) {
    unsigned long currSysTime = BlammoTime::GetSystemTimeInMillisecs();
    if (currSysTime - this->timeOfLastSwitchPress < SwitchBlock::RESET_TIME) {
        // Do nothing, need to wait for the switch to reset
        return;
    }
    
    // Activate the switch - which in turn will activate some other block/event in the level...
    // TODO

    this->timeOfLastSwitchPress = currSysTime;
}
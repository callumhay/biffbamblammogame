/**
 * SwitchBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "SwitchBlock.h"
#include "GameModel.h"
#include "GameLevel.h"
#include "Beam.h"
#include "GameEventManager.h"

// Constant for the amount of time it takes for the switch to become activatable again, after
// being turned on
const float SwitchBlock::RESET_TIME_IN_MS = 4000;

const float SwitchBlock::SWITCH_WIDTH   = 1.5f;
const float SwitchBlock::SWITCH_HEIGHT  = 0.3f;

const int SwitchBlock::TOGGLE_ON_OFF_LIFE_POINTS = 150;

SwitchBlock::SwitchBlock(const LevelPiece::TriggerID& idToTriggerOnSwitch, 
                         unsigned int wLoc, unsigned int hLoc) :
idToTriggerOnSwitch(idToTriggerOnSwitch),
LevelPiece(wLoc, hLoc), timeOfLastSwitchPress(0), 
lifePointsUntilNextToggle(SwitchBlock::TOGGLE_ON_OFF_LIFE_POINTS) {
    assert(idToTriggerOnSwitch != LevelPiece::NO_TRIGGER_ID);
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

	static const float HALF_SWITCH_HEIGHT_BOUND = LevelPiece::HALF_PIECE_HEIGHT;
	static const float HALF_SWITCH_WIDTH_BOUND  = LevelPiece::HALF_PIECE_WIDTH;

	// Left boundry of the piece
	if (leftNeighbor != NULL) {
        if (leftNeighbor->GetType() != LevelPiece::Solid) {
		    Collision::LineSeg2D l1(this->center + Vector2D(-HALF_SWITCH_WIDTH_BOUND, HALF_SWITCH_HEIGHT_BOUND), 
								     this->center + Vector2D(-HALF_SWITCH_WIDTH_BOUND, -HALF_SWITCH_HEIGHT_BOUND));
		    Vector2D n1(-1, 0);
		    boundingLines.push_back(l1);
		    boundingNorms.push_back(n1);
        }
	}

	// Bottom boundry of the piece
	if (bottomNeighbor != NULL) {
        if (bottomNeighbor->GetType() != LevelPiece::Solid) {
		    Collision::LineSeg2D l2(this->center + Vector2D(-HALF_SWITCH_WIDTH_BOUND, -HALF_SWITCH_HEIGHT_BOUND),
								     this->center + Vector2D(HALF_SWITCH_WIDTH_BOUND, -HALF_SWITCH_HEIGHT_BOUND));
		    Vector2D n2(0, -1);
		    boundingLines.push_back(l2);
		    boundingNorms.push_back(n2);
        }
	}

	// Right boundry of the piece
	if (rightNeighbor != NULL) {
        if (rightNeighbor->GetType() != LevelPiece::Solid) {
		    Collision::LineSeg2D l3(this->center + Vector2D(HALF_SWITCH_WIDTH_BOUND, -HALF_SWITCH_HEIGHT_BOUND),
								     this->center + Vector2D(HALF_SWITCH_WIDTH_BOUND, HALF_SWITCH_HEIGHT_BOUND));
		    Vector2D n3(1, 0);
		    boundingLines.push_back(l3);
		    boundingNorms.push_back(n3);
        }
	}

	// Top boundry of the piece
	if (topNeighbor != NULL) {
        if (topNeighbor->GetType() != LevelPiece::Solid) {
		    Collision::LineSeg2D l4(this->center + Vector2D(HALF_SWITCH_WIDTH_BOUND, HALF_SWITCH_HEIGHT_BOUND),
								     this->center + Vector2D(-HALF_SWITCH_WIDTH_BOUND, HALF_SWITCH_HEIGHT_BOUND));
		    Vector2D n4(0, 1);
		    boundingLines.push_back(l4);
		    boundingNorms.push_back(n4);
        }
	}

	this->SetBounds(BoundingLines(boundingLines, boundingNorms), leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
    /*
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
    */
}

bool SwitchBlock::ProjectilePassesThrough(const Projectile* projectile) const {
    switch (projectile->GetType()) {	
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			// When frozen, projectiles can pass through
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				return true;
			}
			break;

		case Projectile::CollateralBlockProjectile:
			return false;
		case Projectile::FireGlobProjectile:
            return false;

		default:
			break;
	}
	return false;
}

LevelPiece* SwitchBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
    if (ball.IsLastPieceCollidedWith(this)) {
        return this;
    }

	bool isFireBall = ((ball.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
	bool isIceBall  = ((ball.GetBallType() & GameBall::IceBall) == GameBall::IceBall);
	if (isIceBall) {
		this->FreezePieceInIce(gameModel);
	}
	else {
        if (this->HasStatus(LevelPiece::IceCubeStatus)) {
            if (!isFireBall) {
				// EVENT: Ice was shattered
				GameEventManager::Instance()->ActionBlockIceShattered(*this);
            }
            // Unfreeze a frozen block
			bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
            UNUSED_VARIABLE(success);
			assert(success);
        }
        else {
            this->SwitchPressed(gameModel);
        }
	}

    ball.SetLastPieceCollidedWith(this);
    return this;
}

LevelPiece* SwitchBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
    // Figure out which switch(es) is/are being affected by the projectile
    LevelPiece* resultingPiece = this;
	switch (projectile->GetType()) {
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				this->DoIceCubeReflectRefractLaserBullets(projectile, gameModel);
			}
            else {
			    this->SwitchPressed(gameModel);
            }
			break;
		
		case Projectile::CollateralBlockProjectile:
            if (this->HasStatus(LevelPiece::IceCubeStatus)) {
                GameEventManager::Instance()->ActionBlockIceShattered(*this);
                gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
            }
	        this->SwitchPressed(gameModel);
			break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:

			// The rocket should not destroy this block, however it certainly
			// is allowed to destroy blocks around it!
            assert(dynamic_cast<RocketProjectile*>(projectile) != NULL);
			resultingPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, static_cast<RocketProjectile*>(projectile), this);
            assert(resultingPiece == this);

            if (this->HasStatus(LevelPiece::IceCubeStatus)) {
                GameEventManager::Instance()->ActionBlockIceShattered(*this);
                bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                UNUSED_VARIABLE(success);
                assert(success);
            }

            this->SwitchPressed(gameModel);
			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            // A mine will just come to rest on the block.
            break;

		case Projectile::FireGlobProjectile:
			// Fire glob just extinguishes on a switch block, unless it's frozen in an ice cube;
			// in that case, unfreeze a frozen switch block
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                UNUSED_VARIABLE(success);
				assert(success);
			}
            else {
                this->SwitchPressed(gameModel);
            }
			break;

		default:
			assert(false);
			break;
	}

	return resultingPiece;
}

LevelPiece* SwitchBlock::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle) {
    UNUSED_PARAMETER(paddle);

    if (this->HasStatus(LevelPiece::IceCubeStatus)) {
	    GameEventManager::Instance()->ActionBlockIceShattered(*this);
        // Unfreeze a frozen block
		bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
        UNUSED_VARIABLE(success);
		assert(success);
    }
    else {
        this->SwitchPressed(gameModel);
    }
    return this;
}

LevelPiece* SwitchBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	assert(gameModel != NULL);

	// If the piece is frozen in ice it will just refract the laser beams...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		return this;
	}

	this->lifePointsUntilNextToggle -= static_cast<float>(dT * static_cast<double>(beamSegment->GetDamagePerSecond()));
	if (this->lifePointsUntilNextToggle <= 0) {
		// Reset the hit points for the next toggle
		this->lifePointsUntilNextToggle = SwitchBlock::TOGGLE_ON_OFF_LIFE_POINTS;
		this->SwitchPressed(gameModel);
	}

	return this;
}

/**
 * Private helper function called whenever the switch block is 'pressed'.
 */
void SwitchBlock::SwitchPressed(GameModel* gameModel) {
    // The timer makes sure that the player can't repeatedly trigger this block, it also prevents
    // infinite recursion when two switches are hooked up in a loop
    unsigned long currSysTime = BlammoTime::GetSystemTimeInMillisecs();
    if (currSysTime - this->timeOfLastSwitchPress < SwitchBlock::RESET_TIME_IN_MS) {
        // Do nothing, need to wait for the switch to reset
        return;
    }
    
    // Activate the switch - which in turn will activate some other block/event in the level...
    GameLevel* currLevel = gameModel->GetCurrentLevel();
    currLevel->ActivateTriggerableLevelPiece(this->idToTriggerOnSwitch, gameModel);

    // EVENT: This switch block has been activated
    GameEventManager::Instance()->ActionSwitchBlockActivated(*this);

    // Switch has now officially been activated, reset the timer.
    this->timeOfLastSwitchPress = currSysTime;
}
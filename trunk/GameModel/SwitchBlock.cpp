/**
 * SwitchBlock.cpp
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
LevelPiece(wLoc, hLoc), idToTriggerOnSwitch(idToTriggerOnSwitch), timeOfLastSwitchPress(0), 
lifePointsUntilNextToggle(SwitchBlock::TOGGLE_ON_OFF_LIFE_POINTS) {

    assert(idToTriggerOnSwitch != LevelPiece::NO_TRIGGER_ID);
}

SwitchBlock::~SwitchBlock() {
}

void SwitchBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                               const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                               const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                               const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {
    
	// If the block is in ice then its bounds are a basic rectangle...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		LevelPiece::UpdateBounds(leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
            topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
		return;
	}

	// Set the bounding lines for a beveled block...
    static const int MAX_NUM_LINES = 4;
    Collision::LineSeg2D boundingLines[MAX_NUM_LINES];
    Vector2D  boundingNorms[MAX_NUM_LINES];
    bool onInside[MAX_NUM_LINES];

	static const float HALF_SWITCH_HEIGHT_BOUND = LevelPiece::HALF_PIECE_HEIGHT;
	static const float HALF_SWITCH_WIDTH_BOUND  = LevelPiece::HALF_PIECE_WIDTH;

    bool shouldGenBounds = false;
    int lineCount = 0;

	// Left boundary of the piece
    shouldGenBounds = (leftNeighbor == NULL || leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        leftNeighbor->GetType() != LevelPiece::Solid);
    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(-HALF_SWITCH_WIDTH_BOUND, HALF_SWITCH_HEIGHT_BOUND), 
            this->center + Vector2D(-HALF_SWITCH_WIDTH_BOUND, -HALF_SWITCH_HEIGHT_BOUND));
	    boundingNorms[lineCount] = Vector2D(-1, 0);
        onInside[lineCount] = (leftNeighbor == NULL || leftNeighbor->HasStatus(LevelPiece::IceCubeStatus));
        lineCount++;
    }

	// Bottom boundary of the piece
    shouldGenBounds = (bottomNeighbor == NULL || bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
        bottomNeighbor->GetType() != LevelPiece::Solid);
    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(-HALF_SWITCH_WIDTH_BOUND, -HALF_SWITCH_HEIGHT_BOUND),
            this->center + Vector2D(HALF_SWITCH_WIDTH_BOUND, -HALF_SWITCH_HEIGHT_BOUND));
	    boundingNorms[lineCount] = Vector2D(0, -1);
        onInside[lineCount] = (bottomNeighbor == NULL || bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus));
        lineCount++;
	}

	// Right boundary of the piece
    shouldGenBounds = (rightNeighbor == NULL || rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        rightNeighbor->GetType() != LevelPiece::Solid);
    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(HALF_SWITCH_WIDTH_BOUND, -HALF_SWITCH_HEIGHT_BOUND),
            this->center + Vector2D(HALF_SWITCH_WIDTH_BOUND, HALF_SWITCH_HEIGHT_BOUND));
	    boundingNorms[lineCount] = Vector2D(1, 0);
        onInside[lineCount] = (rightNeighbor == NULL || rightNeighbor->HasStatus(LevelPiece::IceCubeStatus));
        lineCount++;
	}

	// Top boundary of the piece
    shouldGenBounds = (topNeighbor == NULL || topNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
        topNeighbor->GetType() != LevelPiece::Solid);
    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(HALF_SWITCH_WIDTH_BOUND, HALF_SWITCH_HEIGHT_BOUND),
            this->center + Vector2D(-HALF_SWITCH_WIDTH_BOUND, HALF_SWITCH_HEIGHT_BOUND));
	    boundingNorms[lineCount] = Vector2D(0, 1);
        onInside[lineCount] = (topNeighbor == NULL || topNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus));
        lineCount++;
	}

    this->SetBounds(BoundingLines(lineCount, boundingLines, boundingNorms, onInside), 
        leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

bool SwitchBlock::ProjectilePassesThrough(const Projectile* projectile) const {
    switch (projectile->GetType()) {

        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
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
        case Projectile::PaddleFlameBlastProjectile:
        case Projectile::PaddleIceBlastProjectile:
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

            if (isFireBall) {
                // EVENT: Frozen block canceled-out by fire
                GameEventManager::Instance()->ActionBlockIceCancelledWithFire(*this);
            }

        }
        else {
            this->SwitchPressed(gameModel);
        }
	}

    ball.SetLastPieceCollidedWith(this);
    return this;
}

LevelPiece* SwitchBlock::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle) {
    if (paddle.IsLastPieceCollidedWith(this)) {
        return this;
    }

    if (this->HasStatus(LevelPiece::IceCubeStatus)) {
        GameEventManager::Instance()->ActionBlockIceShattered(*this);
        bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
        UNUSED_VARIABLE(success);
        assert(success);
    }
    else {
        this->SwitchPressed(gameModel);
    }

    paddle.SetLastPieceCollidedWith(this);

    return this;
}

LevelPiece* SwitchBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
    // Figure out which switch(es) is/are being affected by the projectile
    LevelPiece* resultingPiece = this;
	switch (projectile->GetType()) {

        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
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

        case Projectile::BossLightningBoltBulletProjectile:
        case Projectile::BossShockOrbBulletProjectile:
            if (!this->HasStatus(LevelPiece::IceCubeStatus)) {
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
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:

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
        case Projectile::PaddleFlameBlastProjectile:
			// Fire glob activates a switch block, unless it's frozen in an ice cube;
			// in that case, unfreeze it
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				this->LightPieceOnFire(gameModel, false);
			}
            else {
                this->SwitchPressed(gameModel);
            }
			break;

        case Projectile::PaddleIceBlastProjectile:
            this->FreezePieceInIce(gameModel);
            break;

        case Projectile::PortalBlobProjectile:
            break;

		default:
			assert(false);
			break;
	}

	return resultingPiece;
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
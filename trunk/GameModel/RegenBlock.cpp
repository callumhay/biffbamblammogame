/**
 * RegenBlock.cpp
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

#include "RegenBlock.h"
#include "GameEventManager.h"
#include "GameModel.h"
#include "EmptySpaceBlock.h"
#include "GameLevel.h"
#include "GameBall.h"
#include "FireGlobProjectile.h"

const float RegenBlock::REGEN_LIFE_POINTS_PER_SECOND = 6.0f;

RegenBlock::RegenBlock(bool hasInfiniteLife, unsigned int wLoc, unsigned int hLoc) :
LevelPiece(wLoc, hLoc), currLifePoints(0.0f), numFireGlobCountdownsWithoutDrop(0),
fireGlobDropCountdown(GameModelConstants::GetInstance()->GenerateFireGlobDropTime()) {
    if (hasInfiniteLife) {
        this->currLifePoints = RegenBlock::INFINITE_LIFE_POINTS;
    }
    else {
        this->currLifePoints = RegenBlock::MAX_LIFE_POINTS;
    }
}

RegenBlock::~RegenBlock() {
}

bool RegenBlock::ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const {
    // Fire and ice balls always bounce off
    if (((b.GetBallType() & GameBall::IceBall) != GameBall::IceBall) || 
        ((b.GetBallType() & GameBall::FireBall) != GameBall::FireBall)) {
        return true;
    }

    // Deal with the infinite life version of the regen block...
    if (this->HasInfiniteLife()) {
        // Always bounces off unless it is frozen and gets hit by a non-iceball...
        if (this->HasStatus(LevelPiece::IceCubeStatus)) {
            return false;
        }
        return true;
    }

    // If the ball destroys the block then there's no bounce effect
    return b.GetCollisionDamage() < this->currLifePoints;
}

// Whether or not the ball can just blast right through this block.
// Returns: true if it can, false otherwise.
bool RegenBlock::BallBlastsThrough(const GameBall& b) const {
    // The may blast through this if it's uber and not fiery/icy
	return ((b.GetBallType() & GameBall::UberBall) == GameBall::UberBall) &&
           ((b.GetBallType() & GameBall::IceBall) != GameBall::IceBall) && 
           ((b.GetBallType() & GameBall::FireBall) != GameBall::FireBall) &&
           ((!this->HasInfiniteLife() && this->currLifePoints <= b.GetCollisionDamage()) || this->HasStatus(LevelPiece::IceCubeStatus));
}

// Determine whether the given projectile will pass through this block...
bool RegenBlock::ProjectilePassesThrough(const Projectile* projectile) const {
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
			return true;

		case Projectile::FireGlobProjectile:
			// Let fire globs pass through if they spawned on this block
			if (projectile->IsLastThingCollidedWith(this)) {
				return true;
			}
			break;

		default:
			break;
	}
	return false;
}

/**
 * Get the number of points when this piece changes to the given piece.
 */
int RegenBlock::GetPointsOnChange(const LevelPiece& changeToPiece) const {
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        return RegenBlock::POINTS_ON_BLOCK_DESTROYED;
    }
    return 0;
}

LevelPiece* RegenBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
    
    // If this block has infinite life then it can only be destroyed in very particular ways...
    if (this->HasInfiniteLife()) {
        // Infinite life regen blocks may only be destroyed by being shattered or via a collateral block clobbering them
        if (method != LevelPiece::IceShatterDestruction && method != LevelPiece::CollateralDestruction &&
            method != LevelPiece::TeslaDestruction && method != LevelPiece::RocketDestruction && 
            method != LevelPiece::MineDestruction && method != LevelPiece::BombDestruction) {

            // EVENT: The block has been 'perturbed'
            GameEventManager::Instance()->ActionRegenBlockPreturbed(*this);
            return this;
        }
    }

	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this, method);

	// When destroying a non-infinite regen block there is the possiblity of dropping an item...
    if (!this->HasInfiniteLife()) {
	    gameModel->AddPossibleItemDrop(*this);
    }

	// Tell the level that this piece has changed to empty...
	GameLevel* level = gameModel->GetCurrentLevel();
	LevelPiece* emptyPiece = new EmptySpaceBlock(this->wIndex, this->hIndex);
	level->PieceChanged(gameModel, this, emptyPiece, method);

	// Obliterate all that is left of this block...
	LevelPiece* tempThis = this;
	delete tempThis;
	tempThis = NULL;

    // Add to the ball boost meter...
    if (this->HasInfiniteLife()) {
        // Infinite-life regen blocks tend to have a very particular way of being destroyed that doesn't
        // rely heavily on boosting, don't give back quite as much boost juice as finite
        gameModel->AddPercentageToBoostMeter(0.2);
    }
    else {
        // Finite-life regen blocks need lots of boosts to bring down, give the player back some of that boost juice
        gameModel->AddPercentageToBoostMeter(0.3);
    }

	return emptyPiece;
}

LevelPiece* RegenBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	LevelPiece* newPiece = this;
	
	// If the ball is an uber ball with fire or a ball without fire, then we dimish the piece,
	// otherwise we apply the "on fire" status to the piece and leave it at that
	bool isFireBall = ((ball.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
	bool isIceBall  = ((ball.GetBallType() & GameBall::IceBall) == GameBall::IceBall);

	if (!isFireBall && !isIceBall) {
		if (this->HasStatus(LevelPiece::IceCubeStatus)) {
			// EVENT: Ice was shattered
			GameEventManager::Instance()->ActionBlockIceShattered(*this);
			// If the piece is frozen it shatters and is immediately destroyed on ball impact
            newPiece = this->Destroy(gameModel, LevelPiece::IceShatterDestruction);
		}
		else {
            newPiece = this->HurtPiece(ball.GetCollisionDamage(), gameModel, LevelPiece::RegularDestruction);
		}
	}
	else if (isFireBall) {
		this->LightPieceOnFire(gameModel);
	}
	else if (isIceBall) {
		this->FreezePieceInIce(gameModel);
	}

	ball.SetLastPieceCollidedWith(newPiece);
	return newPiece;
}

LevelPiece* RegenBlock::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle) {
    if (paddle.IsLastPieceCollidedWith(this)) {
        return this;
    }

    LevelPiece* newPiece = this;

    if (this->HasStatus(LevelPiece::IceCubeStatus)) {
        // EVENT: Ice was shattered
        GameEventManager::Instance()->ActionBlockIceShattered(*this);
        // If the piece is frozen it shatters and is immediately destroyed on ball impact
        newPiece = this->Destroy(gameModel, LevelPiece::IceShatterDestruction);
    }
    else {
        newPiece = this->HurtPiece(paddle.GetCollisionDamage(), gameModel, LevelPiece::RegularDestruction);

        if (newPiece->GetType() != LevelPiece::Empty) {
            paddle.SetLastPieceCollidedWith(newPiece);
        }
    }

    return newPiece;
}

/**
 * Call this when a collision has actually occurred with a projectile and this block.
 * Returns: The resulting level piece that this has become.
 */
LevelPiece* RegenBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	assert(gameModel != NULL);
	assert(projectile != NULL);
	LevelPiece* newPiece = this;

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
				// Laser bullets dimish the piece, but don't necessarily obliterate/destroy it
                newPiece = this->HurtPiece(projectile->GetDamage(), gameModel, LevelPiece::LaserProjectileDestruction);
			}
			break;

        case Projectile::BossLightningBoltBulletProjectile:
        case Projectile::BossShockOrbBulletProjectile:
            newPiece = this->HurtPiece(projectile->GetDamage(), gameModel, LevelPiece::BasicProjectileDestruction);
            break;

		case Projectile::CollateralBlockProjectile:
			// Completely destroy the block...
            newPiece = this->Destroy(gameModel, LevelPiece::CollateralDestruction);
			break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
            assert(dynamic_cast<RocketProjectile*>(projectile) != NULL);
			newPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, static_cast<RocketProjectile*>(projectile), this);
			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            // A mine will just come to rest on the block.
            break;

		case Projectile::FireGlobProjectile:
			if (!projectile->IsLastThingCollidedWith(this)) {
                this->LightPieceOnFire(gameModel);
			}
			break;

        case Projectile::PaddleFlameBlastProjectile:
            // Do a touch of damage if the piece is on fire...
            if (this->HasStatus(LevelPiece::OnFireStatus)) {
                newPiece = this->HurtPiece(projectile->GetDamage(), gameModel, LevelPiece::BasicProjectileDestruction);
            }
            else {
                // This piece may catch on fire...
                this->LightPieceOnFire(gameModel);
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

	return newPiece;
}

/**
 * When the piece has a per-frame effect on it (e.g., it's on fire) then this will be called
 * by the game model. See LevelPiece for more information.
 */
bool RegenBlock::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
	assert(gameModel != NULL);
	
	LevelPiece* resultingPiece = this;
	int32_t currPieceStatus = this->pieceStatus;

	// If this piece is on fire then we slowly eat away at it with fire...
	if (this->HasStatus(LevelPiece::OnFireStatus)) {
		assert(!this->HasStatus(LevelPiece::IceCubeStatus));

		// Blocks on fire have a chance of dropping a glob of flame over some time...
		if (this->fireGlobDropCountdown <= 0.0) {
            bool didDrop = this->DoPossibleFireGlobDrop(gameModel, this->numFireGlobCountdownsWithoutDrop > MAX_FIREGLOB_COUNTDOWNS_WITHOUT_DROP);
            if (didDrop) {
                this->numFireGlobCountdownsWithoutDrop = 0;
            }
            else {
                this->numFireGlobCountdownsWithoutDrop++;
            }
            this->fireGlobDropCountdown = GameModelConstants::GetInstance()->GenerateFireGlobDropTime();
		}
        else {
            this->fireGlobDropCountdown -= dT;
        }

		// Fire will continue to diminish the piece... 
		// NOTE: This can destroy this piece resulting in a new level piece to replace it (i.e., an empty piece)
        // NOTE: We do 25% of the normal fire damage because we want the block to stay on fire for as long as a red block would
        resultingPiece = this->HurtPiece(dT * (REGEN_LIFE_POINTS_PER_SECOND + 0.25 * GameModelConstants::GetInstance()->FIRE_DAMAGE_PER_SECOND), gameModel,
            LevelPiece::FireDestruction);

		// Technically if the above destroys the block then the block automatically loses all of its status
		// this is done in the destructor of LevelPiece
	}

	// If this piece has been destroyed during this tick then we need to return all the
	// status effects that were previously making it tick
	if (resultingPiece != this) {
		removedStatuses = currPieceStatus;
		return true;
	}

	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return false;
}

/**
 * Regenerates the life of this piece over small increments of time.
 */
void RegenBlock::Regen(double dT) {
    // If the piece has infinite life then this doesn't matter
    if (this->HasInfiniteLife()) {
        return;
    }

    // No regeneration if the piece is frozen
    if (this->HasStatus(LevelPiece::IceCubeStatus)) {
        return;
    }
    // NOTE: if the piece is on fire it will be losing life to the fire faster then it
    // will be regenerating it
    
    // Regenerate...
    if (this->currLifePoints < MAX_LIFE_POINTS && this->currLifePoints > 0.0) {
        float lifePointsBefore = this->currLifePoints;
        this->currLifePoints = std::min<float>(MAX_LIFE_POINTS, this->currLifePoints + dT * REGEN_LIFE_POINTS_PER_SECOND);

        if (lifePointsBefore != this->currLifePoints) {
            // EVENT: Life points on a regen block have changed
            GameEventManager::Instance()->ActionRegenBlockLifeChanged(*this, lifePointsBefore);
        }
    }
}

/**
 * Does damage to this piece if this piece doesn't have infinite life.
 */
LevelPiece* RegenBlock::HurtPiece(float damage, GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
    // EVENT: The block has been 'preturbed'
    GameEventManager::Instance()->ActionRegenBlockPreturbed(*this);

    // You can't hurt a block with infinite life, silly
    if (this->HasInfiniteLife()) {
        return this;
    }

    LevelPiece* resultingPiece = this;

    float lifePointsBefore = this->currLifePoints;
    this->currLifePoints = std::max<float>(0.0, this->currLifePoints - damage);
    
    // EVENT: Life points on a regen block have changed
    GameEventManager::Instance()->ActionRegenBlockLifeChanged(*this, lifePointsBefore);
    
    if (this->currLifePoints <= 0.0f) {
        resultingPiece = this->Destroy(gameModel, method);
    }

    return resultingPiece;
}
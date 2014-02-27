/**
 * AlwaysDropBlock.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#include "AlwaysDropBlock.h"
#include "EmptySpaceBlock.h"
#include "GameModel.h"
#include "GameLevel.h"
#include "GameBall.h"
#include "GameEventManager.h"

AlwaysDropBlock::AlwaysDropBlock(const std::vector<GameItem::ItemType>& droppableItemTypes,
                                 unsigned int wLoc, unsigned int hLoc) : 
LevelPiece(wLoc, hLoc), currLifePoints(PIECE_STARTING_LIFE_POINTS) {
    
    assert(!droppableItemTypes.empty());

    this->nextDropItemType    = droppableItemTypes[Randomizer::GetInstance()->RandomUnsignedInt() % droppableItemTypes.size()];
    this->nextDropDisposition = GameItemFactory::GetInstance()->GetItemTypeDisposition(this->nextDropItemType);
}

AlwaysDropBlock::~AlwaysDropBlock() {
}

LevelPiece* AlwaysDropBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this, method);

	// When destroying an always drop block it will always drop an item...
    gameModel->AddItemDrop(this->GetCenter(), this->nextDropItemType);

	// Tell the level that this piece has changed to empty...
	GameLevel* level = gameModel->GetCurrentLevel();
	LevelPiece* emptyPiece = new EmptySpaceBlock(this->wIndex, this->hIndex);
	level->PieceChanged(gameModel, this, emptyPiece, method);

	// Obliterate all that is left of this block...
	LevelPiece* tempThis = this;
	delete tempThis;
	tempThis = NULL;

    // Add to the ball boost meter...
    gameModel->AddPercentageToBoostMeter(0.03);

	return emptyPiece;
}

// Eat away at this block over the given dT time at the given damage per second...
// Returns: The resulting level piece that this becomes after exposed to the damage
LevelPiece* AlwaysDropBlock::EatAwayAtPiece(double dT, int dmgPerSec, GameModel* gameModel,
                                            const LevelPiece::DestructionMethod& method) {

	this->currLifePoints -= static_cast<float>(dT * dmgPerSec);
	
	LevelPiece* newPiece = this;
	if (currLifePoints <= 0) {
		// The piece is dead...
		newPiece = this->Destroy(gameModel, method);
	}

	return newPiece;
}

/**
 * Call this when a collision has actually occurred with the ball and this block.
 * Returns: The resulting level piece that this has become.
 */
LevelPiece* AlwaysDropBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	assert(gameModel != NULL);
    
    // Make sure we don't do a double collision
    if (ball.IsLastPieceCollidedWith(this)) {
        return this;
    }
    
	LevelPiece* newPiece = this;
	
	// If the ball is an uber ball with fire or a ball without fire, then we dimish the piece,
	// otherwise we apply the "on fire" status to the piece and leave it at that
	bool isFireBall = ((ball.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
	bool isIceBall  = ((ball.GetBallType() & GameBall::IceBall) == GameBall::IceBall);

	if (!isFireBall && !isIceBall) {

		if (this->HasStatus(LevelPiece::IceCubeStatus)) {
			// EVENT: Ice was shattered
			GameEventManager::Instance()->ActionBlockIceShattered(*this);
            newPiece = this->Destroy(gameModel, LevelPiece::IceShatterDestruction);
		}
        else {
            newPiece = this->Destroy(gameModel, LevelPiece::RegularDestruction);
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

LevelPiece* AlwaysDropBlock::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle) {
    UNUSED_PARAMETER(paddle);

    LevelPiece* newPiece = this;

    if (this->HasStatus(LevelPiece::IceCubeStatus)) {
        // EVENT: Ice was shattered
        GameEventManager::Instance()->ActionBlockIceShattered(*this);
        newPiece = this->Destroy(gameModel, LevelPiece::IceShatterDestruction);
    }
    else {
        newPiece = this->Destroy(gameModel, LevelPiece::RegularDestruction);
    }

    return newPiece;
}

/**
 * Call this when a collision has actually occurred with a projectile and this block.
 * Returns: The resulting level piece that this has become.
 */
LevelPiece* AlwaysDropBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
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
                newPiece = this->Destroy(gameModel, LevelPiece::LaserProjectileDestruction);
			}
			break;
        
        case Projectile::BossLightningBoltBulletProjectile:
            newPiece = this->Destroy(gameModel, LevelPiece::BasicProjectileDestruction);
            break;

		case Projectile::CollateralBlockProjectile:
			// Completely destroy the block...
            newPiece = this->Destroy(gameModel, LevelPiece::CollateralDestruction);
			break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
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
				// This piece may catch on fire...
				this->LightPieceOnFire(gameModel);
			}
			break;

        case Projectile::PaddleFlameBlastProjectile:
            this->LightPieceOnFire(gameModel);
            break;

        case Projectile::PaddleIceBlastProjectile:
            this->FreezePieceInIce(gameModel);
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
bool AlwaysDropBlock::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
	assert(gameModel != NULL);
	
	LevelPiece* resultingPiece = this;
	int32_t currPieceStatus = this->pieceStatus;

	// If this piece is on fire then we slowly eat away at it with fire...
	if (this->HasStatus(LevelPiece::OnFireStatus)) {
		assert(!this->HasStatus(LevelPiece::IceCubeStatus));

		// Fire will continue to diminish the piece... 
		// NOTE: This can destroy this piece resulting in a new level piece to replace it (i.e., an empty piece)
		resultingPiece = this->EatAwayAtPiece(dT, GameModelConstants::GetInstance()->FIRE_DAMAGE_PER_SECOND, gameModel,
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

    // No statues were removed
	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return false;
}

// Determine whether the given projectile will pass through this block...
bool AlwaysDropBlock::ProjectilePassesThrough(const Projectile* projectile) const {
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


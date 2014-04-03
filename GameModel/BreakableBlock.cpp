/**
 * BreakableBlock.cpp
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

#include "BreakableBlock.h"
#include "EmptySpaceBlock.h"
#include "TriangleBlocks.h"
#include "GameModel.h"
#include "GameLevel.h"
#include "GameBall.h"
#include "GameEventManager.h"
#include "FireGlobProjectile.h"

const double BreakableBlock::ALLOWABLE_TIME_BETWEEN_BALL_COLLISIONS_IN_MS = 17;

BreakableBlock::BreakableBlock(char type, unsigned int wLoc, unsigned int hLoc) : 
LevelPiece(wLoc, hLoc), pieceType(static_cast<BreakablePieceType>(type)), currLifePoints(PIECE_STARTING_LIFE_POINTS),
fireGlobDropCountdown(GameModelConstants::GetInstance()->GenerateFireGlobDropTime()), numFireGlobCountdownsWithoutDrop(0),
timeOfLastBallCollision() {
	assert(IsValidBreakablePieceType(type));

	this->colour = ColourRGBA(BreakableBlock::GetColourOfBreakableType(this->pieceType), 1.0f);
}


BreakableBlock::~BreakableBlock() {
}

bool BreakableBlock::ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const {
    // If the ball is a fire or ice ball then it will always bounce off
    if (((b.GetBallType() & GameBall::IceBall) == GameBall::IceBall) ||
        ((b.GetBallType() & GameBall::FireBall) == GameBall::FireBall)) {
        return true;
    }

    // If this piece is green then it will break no matter what, since the ball has no special status
    if (this->GetBreakablePieceType() == GreenBreakable) {
        return false;
    }

    // Only other special case is the uber ball...
    if (((b.GetBallType() & GameBall::UberBall) == GameBall::UberBall) && 
        (this->GetBreakablePieceType() == YellowBreakable)) {
        return false;
    }

    return true;
}

LevelPiece* BreakableBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this, method);

	// When destroying a breakable there is the possiblity of dropping an item...
	gameModel->AddPossibleItemDrop(*this);

	// Tell the level that this piece has changed to empty...
	GameLevel* level = gameModel->GetCurrentLevel();
	LevelPiece* emptyPiece = new EmptySpaceBlock(this->wIndex, this->hIndex);
	level->PieceChanged(gameModel, this, emptyPiece, method);

	// Obliterate all that is left of this block...
	LevelPiece* tempThis = this;
	delete tempThis;
	tempThis = NULL;

    // Add to the ball boost meter...
    gameModel->AddPercentageToBoostMeter(0.025);

	return emptyPiece;
}

/**
 * Private static helper function used to move the colour of this breakable
 * down to the next level (thus decrementing it appropriately). For example
 * when a red block is decremented it goes to orange, then yellow, and then to green.
 */
BreakableBlock::BreakablePieceType BreakableBlock::GetDecrementedPieceType(BreakablePieceType breakableType) {
	switch(breakableType) {
		case YellowBreakable:
			return GreenBreakable;
		case OrangeBreakable:
			return YellowBreakable;
		case RedBreakable:
			return OrangeBreakable;
		default:
			break;
	}
	assert(false);
	return GreenBreakable;
}

Colour BreakableBlock::GetColourOfBreakableType(BreakableBlock::BreakablePieceType breakableType) {
	switch(breakableType) {
		case GreenBreakable:
			return Colour(0.0f, 1.0f, 0.0f);
		case YellowBreakable:
			return Colour(1.0f, 1.0f, 0.0f);
		case OrangeBreakable:
			return Colour(1.0f, 0.5f, 0.0f);
		case RedBreakable:
			return Colour(1.0f, 0.0f, 0.0f);
		default:
			assert(false);
			break;
	}
	
	return Colour(1,1,1);
}

/** 
 * This piece is officially dead in its current form, diminish it down
 * to its next form (or officially destroy it if it's in its final form i.e., green).
 * Returns: The resulting piece in its next form after being diminished from the
 * current one.
 */
LevelPiece* BreakableBlock::DiminishPiece(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
	assert(gameModel != NULL);
	LevelPiece* newPiece = this;

	switch(this->pieceType) {
		case GreenBreakable:
			newPiece = this->Destroy(gameModel, method);
			break;

		default:
			// By default any other type of block is simply decremented
			this->DecrementPieceType();
			{
				GameLevel* level = gameModel->GetCurrentLevel();
				level->PieceChanged(gameModel, this, this, method);
			}
			newPiece = this;

            gameModel->AddPercentageToBoostMeter(0.01);
			
            break;

	}
	assert(newPiece != NULL);
	return newPiece;
}

// Eat away at this block over the given dT time at the given damage per second...
// Returns: The resulting level piece that this becomes after exposed to the damage
LevelPiece* BreakableBlock::EatAwayAtPiece(double dT, int dmgPerSec, GameModel* gameModel,
                                           const LevelPiece::DestructionMethod& method) {
	this->currLifePoints -= static_cast<float>(dT * dmgPerSec);
	
	LevelPiece* newPiece = this;
	if (currLifePoints <= 0) {
		// The piece is dead... spawn the next one in sequence
		this->currLifePoints = BreakableBlock::PIECE_STARTING_LIFE_POINTS;
		newPiece = this->DiminishPiece(gameModel, method);
	}

	return newPiece;
}

/**
 * Call this when a collision has actually occured with the ball and this block.
 * Returns: The resulting level piece that this has become.
 */
LevelPiece* BreakableBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	assert(gameModel != NULL);
    
    long currSystemTime = BlammoTime::GetSystemTimeInMillisecs();

    // Make sure we don't do a double collision - check to make sure the ball hasn't already
    // collided with this block and also that we're not in the same game time tick since the last
    // collision of the ball
    if (ball.IsLastPieceCollidedWith(this) && ball.GetTimeSinceLastCollision() < 
        (BreakableBlock::ALLOWABLE_TIME_BETWEEN_BALL_COLLISIONS_IN_MS/1000.0)) {

        this->timeOfLastBallCollision = currSystemTime;
        return this;
    }
    
    if ((currSystemTime - this->timeOfLastBallCollision) < 
         BreakableBlock::ALLOWABLE_TIME_BETWEEN_BALL_COLLISIONS_IN_MS) {

        this->timeOfLastBallCollision = currSystemTime;
        return this;
    }

    this->timeOfLastBallCollision = currSystemTime;

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
			bool isUberBall = ((ball.GetBallType() & GameBall::UberBall) == GameBall::UberBall);

			// If the ball is an 'uber' ball then we decrement the piece type twice when it is
			// not the lowest kind of breakable block (i.e., green)
			if (isUberBall && this->pieceType != GreenBreakable) {
				this->DecrementPieceType();
			}

            newPiece = this->DiminishPiece(gameModel, LevelPiece::RegularDestruction);
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

LevelPiece* BreakableBlock::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle) {

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
        newPiece = this->DiminishPiece(gameModel, LevelPiece::RegularDestruction);

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
LevelPiece* BreakableBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
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
				// Laser bullets diminish the piece, but don't necessarily obliterate/destroy it
                newPiece = this->DiminishPiece(gameModel, LevelPiece::LaserProjectileDestruction);
			}
			break;

        case Projectile::BossLightningBoltBulletProjectile:
            newPiece = this->DiminishPiece(gameModel, LevelPiece::BasicProjectileDestruction);
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
bool BreakableBlock::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
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
		resultingPiece = this->EatAwayAtPiece(dT, GameModelConstants::GetInstance()->FIRE_DAMAGE_PER_SECOND, gameModel,
            LevelPiece::FireDestruction);

		// Technically if the above destroys the block then the block automatically loses all of its status
		// this is done in the destructor of LevelPiece
	}
	//else if (this->HasStatus(LevelPiece::IceCubeStatus)) {
	//	assert(!this->HasStatus(LevelPiece::OnFireStatus));
	//  // ... There's no tick activity for ice cubes
	//}

	// If this piece has been destroyed during this tick then we need to return all the
	// status effects that were previously making it tick
	if (resultingPiece != this) {
		removedStatuses = currPieceStatus;
		return true;
	}

	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return false;
}

// Determine whether the given projectile will pass through this block...
bool BreakableBlock::ProjectilePassesThrough(const Projectile* projectile) const {
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
int BreakableBlock::GetPointsOnChange(const LevelPiece& changeToPiece) const {
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        return BreakableBlock::POINTS_ON_BLOCK_DESTROYED;
    }
    return 0;
}
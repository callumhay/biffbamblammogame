/**
 * LevelPiece.cpp
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

#include "LevelPiece.h"
#include "BreakableBlock.h"
#include "TriangleBlocks.h"
#include "GameEventManager.h"
#include "GameBall.h"
#include "GameModel.h"
#include "GameItem.h"
#include "GameItemFactory.h"
#include "GameModelConstants.h"
#include "Beam.h"
#include "PaddleLaserProjectile.h"
#include "PaddleMineProjectile.h"
#include "FireGlobProjectile.h"

const LevelPiece::TriggerID LevelPiece::NO_TRIGGER_ID = -1;

const float LevelPiece::PIECE_WIDTH = 2.5f;
const float LevelPiece::PIECE_HEIGHT = 1.0f;
const float LevelPiece::PIECE_DEPTH  = 1.30f;
const float LevelPiece::HALF_PIECE_WIDTH  = PIECE_WIDTH / 2.0f;
const float LevelPiece::HALF_PIECE_HEIGHT = PIECE_HEIGHT / 2.0f;
const float LevelPiece::HALF_PIECE_DEPTH  = PIECE_DEPTH / 2.0f;

LevelPiece::LevelPiece(unsigned int wLoc, unsigned int hLoc) : 
colour(1,1,1,1), pieceStatus(LevelPiece::NormalStatus), leftNeighbor(NULL),
bottomNeighbor(NULL), rightNeighbor(NULL), topNeighbor(NULL), topRightNeighbor(NULL),
topLeftNeighbor(NULL), bottomRightNeighbor(NULL), bottomLeftNeighbor(NULL), 
triggerID(LevelPiece::NO_TRIGGER_ID) {
	this->SetWidthAndHeightIndex(wLoc, hLoc);
}

LevelPiece::~LevelPiece() {
	// We need to be sure to obliterate any status effects that might be lingering on the level piece -
	// this emits an important event to any model listeners to remove those effects as well!
	this->RemoveAllStatus(NULL);
    this->DetachAllProjectiles();
}

void LevelPiece::DetachAllProjectiles() {
    // Make sure we remove all attached projectiles as well
    // NOTE: DO NOT USE ITERATORS HERE SINCE THE MINE PROJECTILE DETACHES ITSELF IN THE SetAsFalling call
    while (!this->attachedProjectiles.empty()) {

        MineProjectile* p = *this->attachedProjectiles.begin();
        p->SetLastThingCollidedWith(NULL);
        p->SetAsFalling();
        this->DetachProjectile(p); // This is redundant and will be ignored, just here for robustness
    }
    this->attachedProjectiles.clear();
}

// Static function to determine if the piece type given is valid
bool LevelPiece::IsValidLevelPieceType(int pieceType) {
    switch (pieceType) {
        case LevelPiece::Breakable:
        case LevelPiece::Solid:
        case LevelPiece::Empty:
        case LevelPiece::Bomb:
        case LevelPiece::SolidTriangle:
        case LevelPiece::BreakableTriangle:
        case LevelPiece::Ink:
        case LevelPiece::Prism:
        case LevelPiece::Portal:
        case LevelPiece::PrismTriangle:
        case LevelPiece::Cannon:
        case LevelPiece::FragileCannon:
        case LevelPiece::Collateral:
        case LevelPiece::Tesla:
        case LevelPiece::ItemDrop:
        case LevelPiece::Switch:
        case LevelPiece::OneWay:
        case LevelPiece::NoEntry:
        case LevelPiece::LaserTurret:
        case LevelPiece::RocketTurret:
        case LevelPiece::MineTurret:
        case LevelPiece::AlwaysDrop:
        case LevelPiece::Regen:
            return true;
        default:
            assert(false);
            break;
    }
    return false;
}

void LevelPiece::SetWidthAndHeightIndex(unsigned int wLoc, unsigned int hLoc) {
	this->wIndex = wLoc;
	this->hIndex = hLoc;
	this->center = Point2D(wLoc * PIECE_WIDTH + HALF_PIECE_WIDTH, hLoc * PIECE_HEIGHT + HALF_PIECE_HEIGHT);
}

/** 
 * The default update bounds method used by subclasses - this will update the boundaries
 * based on a generalized idea that if there is a boundary piece type on a certain side
 * of this piece then there is no need for another boundary on that side. In all other cases
 * the boundary will be placed there.
 */
void LevelPiece::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                              const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                              const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                              const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	UNUSED_PARAMETER(bottomLeftNeighbor);
	UNUSED_PARAMETER(bottomRightNeighbor);
	UNUSED_PARAMETER(topRightNeighbor);
	UNUSED_PARAMETER(topLeftNeighbor);

	// Set the bounding lines for a rectangular block - these are also used when any block is frozen in an ice cube
    static const int MAX_NUM_LINES = 4;
    Collision::LineSeg2D boundingLines[MAX_NUM_LINES];
    Vector2D  boundingNorms[MAX_NUM_LINES];

    bool shouldGenBounds = false;
    int lineCount = 0;

	// Left boundary of the piece
    shouldGenBounds = (leftNeighbor == NULL || leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        leftNeighbor->GetType() != LevelPiece::Solid);
    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
            this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
		boundingNorms[lineCount] = Vector2D(-1, 0);
        lineCount++;
    }

	// Bottom boundary of the piece
    shouldGenBounds = (bottomNeighbor == NULL || bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
        bottomNeighbor->GetType() != LevelPiece::Solid);
	if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
            this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
		boundingNorms[lineCount] = Vector2D(0, -1);
        lineCount++;
	}

	// Right boundary of the piece
    shouldGenBounds = (rightNeighbor == NULL || rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        rightNeighbor->GetType() != LevelPiece::Solid);
	if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
            this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));;
		boundingNorms[lineCount] = Vector2D(1, 0);
        lineCount++;
	}

	// Top boundary of the piece
    shouldGenBounds = (topNeighbor == NULL || topNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
        topNeighbor->GetType() != LevelPiece::Solid);
	if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
            this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
		boundingNorms[lineCount] = Vector2D(0, 1);
        lineCount++;
	}

	this->SetBounds(BoundingLines(lineCount, boundingLines, boundingNorms), leftNeighbor, bottomNeighbor,
        rightNeighbor, topNeighbor, topRightNeighbor, topLeftNeighbor, 
        bottomRightNeighbor, bottomLeftNeighbor);
}


bool LevelPiece::ProjectileIsDestroyedOnCollision(const Projectile* projectile) const {
    switch (projectile->GetType()) {

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            // Mines are typically not destroyed by collisions with blocks.
            return false;

        default:
            break;
    }

    return !this->ProjectilePassesThrough(projectile);
}

void LevelPiece::AddStatus(GameLevel* level, const PieceStatus& status) {
	// If the piece already has the status then just exit, this saves us the trouble
	// of emitting an event that could potentially be a more expensive operation
	if ((this->pieceStatus & status) == status) {
		return;
	}

	this->pieceStatus = (this->pieceStatus | status);
	
	// We update the bounds since the status can change the nature of the boundaries generated for this block and
    // its surrounding blocks
    level->UpdateBoundsOnPieceAndSurroundingPieces(this);

	// EVENT: A status has been added to this piece...
	GameEventManager::Instance()->ActionLevelPieceStatusAdded(*this, status);
}

void LevelPiece::RemoveStatus(GameLevel* level, const PieceStatus& status) {
	// If the piece doesn't have the status then just exit, this saves us the trouble
	// of emitting an event that could potentially be a more expensive operation
	if ((this->pieceStatus & status) != status) {
		return;
	}

	this->pieceStatus = (this->pieceStatus & ~status);
	
	// We update the bounds since the status can change the nature of the boundaries generated for this block and
    // its surrounding blocks
    level->UpdateBoundsOnPieceAndSurroundingPieces(this);

	// EVENT: A status has been removed from this piece...
	GameEventManager::Instance()->ActionLevelPieceStatusRemoved(*this, status);
}

void LevelPiece::RemoveStatuses(GameLevel* level, int32_t statusMask) {
	bool statusFound = false;
	// Go through each status and remove it...
	if ((statusMask & LevelPiece::OnFireStatus) == LevelPiece::OnFireStatus) {
		this->RemoveStatus(level, LevelPiece::OnFireStatus);
		statusFound = true;
	}
	if ((statusMask & LevelPiece::IceCubeStatus) == LevelPiece::IceCubeStatus) {
		this->RemoveStatus(level, LevelPiece::IceCubeStatus);
		statusFound = true;
	}
	//... TODO for each other status

	assert(statusFound);
}

void LevelPiece::UpdateBreakableBlockBounds(LevelPiece* thePiece,
                                            const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                            const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                                            const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                                            const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

    UNUSED_PARAMETER(bottomLeftNeighbor);
    UNUSED_PARAMETER(bottomRightNeighbor);
    UNUSED_PARAMETER(topRightNeighbor);
    UNUSED_PARAMETER(topLeftNeighbor);

    // Set the bounding lines for a rectangular block - these are also used when any block is frozen in an ice cube
    static const int MAX_NUM_LINES = 4;
    Collision::LineSeg2D boundingLines[MAX_NUM_LINES];
    Vector2D  boundingNorms[MAX_NUM_LINES];
    bool onInside[MAX_NUM_LINES];
    TriangleBlock::Orientation triOrientation;

    bool shouldGenBounds = false;
    int lineCount = 0;

    // Left boundary of the piece
    if (leftNeighbor != NULL) {
        if ((leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            (leftNeighbor->GetType() != LevelPiece::Solid && 
             leftNeighbor->GetType() != LevelPiece::Breakable && 
             leftNeighbor->GetType() != LevelPiece::LaserTurret && 
             leftNeighbor->GetType() != LevelPiece::RocketTurret &&
             leftNeighbor->GetType() != LevelPiece::MineTurret && 
             leftNeighbor->GetType() != LevelPiece::Bomb && 
             leftNeighbor->GetType() != LevelPiece::Tesla &&
             leftNeighbor->GetType() != LevelPiece::AlwaysDrop && 
             leftNeighbor->GetType() != LevelPiece::Switch &&
             leftNeighbor->GetType() != LevelPiece::ItemDrop &&
             leftNeighbor->GetType() != LevelPiece::Regen))) {

            shouldGenBounds = true;
            if (TriangleBlock::GetOrientation(leftNeighbor, triOrientation)) {
                shouldGenBounds = !(triOrientation == TriangleBlock::UpperRight || triOrientation == TriangleBlock::LowerRight) ||
                    leftNeighbor->GetType() == LevelPiece::PrismTriangle;
            }
        }
    }
    else {
        shouldGenBounds = true;
    }

    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(thePiece->GetCenter() + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
            thePiece->GetCenter() + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
        boundingNorms[lineCount] = Vector2D(-1, 0);
        onInside[lineCount] = (leftNeighbor == NULL || leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            leftNeighbor->GetType() == LevelPiece::OneWay || leftNeighbor->GetType() == LevelPiece::NoEntry ||
            leftNeighbor->GetType() == LevelPiece::PrismTriangle);
        lineCount++;
    }

    // Bottom boundary of the piece
    shouldGenBounds = false;
    if (bottomNeighbor != NULL) {
        if ((bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
            (bottomNeighbor->GetType() != LevelPiece::Solid && 
             bottomNeighbor->GetType() != LevelPiece::Breakable &&
             bottomNeighbor->GetType() != LevelPiece::LaserTurret && 
             bottomNeighbor->GetType() != LevelPiece::RocketTurret &&
             bottomNeighbor->GetType() != LevelPiece::MineTurret && 
             bottomNeighbor->GetType() != LevelPiece::Bomb && 
             bottomNeighbor->GetType() != LevelPiece::Tesla &&
             bottomNeighbor->GetType() != LevelPiece::AlwaysDrop &&
             bottomNeighbor->GetType() != LevelPiece::Switch &&
             bottomNeighbor->GetType() != LevelPiece::ItemDrop &&
             bottomNeighbor->GetType() != LevelPiece::Regen))) {

            shouldGenBounds = true;
            if (TriangleBlock::GetOrientation(bottomNeighbor, triOrientation)) {
                shouldGenBounds = !(triOrientation == TriangleBlock::UpperRight || triOrientation == TriangleBlock::UpperLeft) ||
                    bottomNeighbor->GetType() == LevelPiece::PrismTriangle;
            }
        }
    }
    else {
        shouldGenBounds = true;
    }

    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(thePiece->GetCenter() + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
            thePiece->GetCenter() + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
        boundingNorms[lineCount] = Vector2D(0, -1);
        onInside[lineCount] = (bottomNeighbor == NULL || bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
            bottomNeighbor->GetType() == LevelPiece::OneWay || bottomNeighbor->GetType() == LevelPiece::NoEntry ||
            bottomNeighbor->GetType() == LevelPiece::PrismTriangle);
        lineCount++;
    }

    // Right boundary of the piece
    shouldGenBounds = false;
    if (rightNeighbor != NULL) {
        if ((rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            (rightNeighbor->GetType() != LevelPiece::Solid && 
             rightNeighbor->GetType() != LevelPiece::Breakable &&
             rightNeighbor->GetType() != LevelPiece::LaserTurret && 
             rightNeighbor->GetType() != LevelPiece::RocketTurret &&
             rightNeighbor->GetType() != LevelPiece::MineTurret && 
             rightNeighbor->GetType() != LevelPiece::Bomb && 
             rightNeighbor->GetType() != LevelPiece::Tesla &&
             rightNeighbor->GetType() != LevelPiece::AlwaysDrop && 
             rightNeighbor->GetType() != LevelPiece::Switch &&
             rightNeighbor->GetType() != LevelPiece::ItemDrop &&
             rightNeighbor->GetType() != LevelPiece::Regen))) {

            shouldGenBounds = true;
            if (TriangleBlock::GetOrientation(rightNeighbor, triOrientation)) {
                shouldGenBounds = !(triOrientation == TriangleBlock::UpperLeft || triOrientation == TriangleBlock::LowerLeft) ||
                    rightNeighbor->GetType() == LevelPiece::PrismTriangle;
            }
        }
    }
    else {
        shouldGenBounds = true;
    }

    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(thePiece->GetCenter() + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
            thePiece->GetCenter() + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
        boundingNorms[lineCount] = Vector2D(1, 0);
        onInside[lineCount] = (rightNeighbor == NULL || rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            rightNeighbor->GetType() == LevelPiece::OneWay || rightNeighbor->GetType() == LevelPiece::NoEntry ||
            rightNeighbor->GetType() == LevelPiece::PrismTriangle);
        lineCount++;
    }

    // Top boundary of the piece
    shouldGenBounds = false;
    if (topNeighbor != NULL) {
        if (topNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
            (topNeighbor->GetType() != LevelPiece::Solid && 
             topNeighbor->GetType() != LevelPiece::Breakable &&
             topNeighbor->GetType() != LevelPiece::LaserTurret && 
             topNeighbor->GetType() != LevelPiece::RocketTurret &&
             topNeighbor->GetType() != LevelPiece::MineTurret && 
             topNeighbor->GetType() != LevelPiece::Bomb && 
             topNeighbor->GetType() != LevelPiece::Tesla &&
             topNeighbor->GetType() != LevelPiece::AlwaysDrop && 
             topNeighbor->GetType() != LevelPiece::Switch &&
             topNeighbor->GetType() != LevelPiece::ItemDrop &&
             topNeighbor->GetType() != LevelPiece::Regen)) {

            shouldGenBounds = true;
            if (TriangleBlock::GetOrientation(topNeighbor, triOrientation)) {
                shouldGenBounds = !(triOrientation == TriangleBlock::LowerRight || triOrientation == TriangleBlock::LowerLeft) ||
                    topNeighbor->GetType() == LevelPiece::PrismTriangle;
            }
        }
    }
    else {
        shouldGenBounds = true;
    }

    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(thePiece->GetCenter() + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
            thePiece->GetCenter() + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
        boundingNorms[lineCount] = Vector2D(0, 1);
        onInside[lineCount] = (topNeighbor == NULL || topNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
            topNeighbor->GetType() == LevelPiece::OneWay || topNeighbor->GetType() == LevelPiece::NoEntry ||
            topNeighbor->GetType() == LevelPiece::PrismTriangle);
        lineCount++;
    }

    thePiece->SetBounds(BoundingLines(lineCount, boundingLines, boundingNorms, onInside),
        leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor,
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

void LevelPiece::UpdateSolidRectBlockBounds(LevelPiece* thePiece,
                                            const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                            const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                                            const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                                            const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

    UNUSED_PARAMETER(bottomLeftNeighbor);
    UNUSED_PARAMETER(bottomRightNeighbor);
    UNUSED_PARAMETER(topRightNeighbor);
    UNUSED_PARAMETER(topLeftNeighbor);

    // Set the bounding lines for a rectangular block
    static const int MAX_NUM_LINES = 4;
    Collision::LineSeg2D boundingLines[MAX_NUM_LINES];
    Vector2D  boundingNorms[MAX_NUM_LINES];
    bool onInside[MAX_NUM_LINES];
    TriangleBlock::Orientation triOrientation;

    bool shouldGenBounds = false;
    int lineCount = 0;

    // Left boundary of the piece
    if (leftNeighbor != NULL) {
        if (leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            leftNeighbor->GetType() != LevelPiece::Solid &&
            leftNeighbor->GetType() != LevelPiece::LaserTurret && 
            leftNeighbor->GetType() != LevelPiece::RocketTurret &&
            leftNeighbor->GetType() != LevelPiece::MineTurret &&
            leftNeighbor->GetType() != LevelPiece::Tesla && 
            leftNeighbor->GetType() != LevelPiece::Switch &&
            leftNeighbor->GetType() != LevelPiece::ItemDrop) {

                shouldGenBounds = true;
                if (TriangleBlock::GetOrientation(leftNeighbor, triOrientation)) {
                    shouldGenBounds = !(triOrientation == TriangleBlock::UpperRight || triOrientation == TriangleBlock::LowerRight) ||
                        leftNeighbor->GetType() == LevelPiece::PrismTriangle;
                }
        }
    }
    else {
        shouldGenBounds = true;
    }
    if (shouldGenBounds) {

        boundingLines[lineCount] = Collision::LineSeg2D(thePiece->GetCenter() + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
            thePiece->GetCenter() + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
        boundingNorms[lineCount] = Vector2D(-1, 0);
        onInside[lineCount] = (leftNeighbor == NULL || leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            leftNeighbor->GetType() == LevelPiece::OneWay || leftNeighbor->GetType() == LevelPiece::NoEntry ||
            leftNeighbor->GetType() == LevelPiece::Breakable || leftNeighbor->GetType() == LevelPiece::BreakableTriangle ||
            leftNeighbor->GetType() == LevelPiece::Bomb || leftNeighbor->GetType() == LevelPiece::PrismTriangle ||
            leftNeighbor->GetType() == LevelPiece::Regen);
        lineCount++;
    }
    shouldGenBounds = false;

    // Bottom boundary of the piece
    if (bottomNeighbor != NULL) {
        if (bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
            bottomNeighbor->GetType() != LevelPiece::Solid && 
            bottomNeighbor->GetType() != LevelPiece::LaserTurret && 
            bottomNeighbor->GetType() != LevelPiece::RocketTurret &&
            bottomNeighbor->GetType() != LevelPiece::MineTurret &&
            bottomNeighbor->GetType() != LevelPiece::Tesla && 
            bottomNeighbor->GetType() != LevelPiece::Switch &&
            bottomNeighbor->GetType() != LevelPiece::ItemDrop) {

                shouldGenBounds = true;
                if (TriangleBlock::GetOrientation(bottomNeighbor, triOrientation)) {
                    shouldGenBounds = !(triOrientation == TriangleBlock::UpperRight || triOrientation == TriangleBlock::UpperLeft) ||
                        bottomNeighbor->GetType() == LevelPiece::PrismTriangle;
                }
        }
    }
    else {
        shouldGenBounds = true;
    }
    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(thePiece->GetCenter() + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
            thePiece->GetCenter() + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
        boundingNorms[lineCount] = Vector2D(0, -1);
        onInside[lineCount] = (bottomNeighbor == NULL || bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
            bottomNeighbor->GetType() == LevelPiece::OneWay || bottomNeighbor->GetType() == LevelPiece::NoEntry ||
            bottomNeighbor->GetType() == LevelPiece::Breakable || bottomNeighbor->GetType() == LevelPiece::BreakableTriangle ||
            bottomNeighbor->GetType() == LevelPiece::Bomb || bottomNeighbor->GetType() == LevelPiece::PrismTriangle ||
            bottomNeighbor->GetType() == LevelPiece::Regen);
        lineCount++;
    }
    shouldGenBounds = false;

    // Right boundary of the piece
    if (rightNeighbor != NULL) {
        if (rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            rightNeighbor->GetType() != LevelPiece::Solid && 
            rightNeighbor->GetType() != LevelPiece::LaserTurret && 
            rightNeighbor->GetType() != LevelPiece::RocketTurret &&
            rightNeighbor->GetType() != LevelPiece::MineTurret && 
            rightNeighbor->GetType() != LevelPiece::Tesla && 
            rightNeighbor->GetType() != LevelPiece::Switch &&
            rightNeighbor->GetType() != LevelPiece::ItemDrop) {

                shouldGenBounds = true;
                if (TriangleBlock::GetOrientation(rightNeighbor, triOrientation)) {
                    shouldGenBounds = !(triOrientation == TriangleBlock::UpperLeft || triOrientation == TriangleBlock::LowerLeft) ||
                        rightNeighbor->GetType() == LevelPiece::PrismTriangle;
                }
        }
    }
    else {
        shouldGenBounds = true;
    }
    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(thePiece->GetCenter() + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
            thePiece->GetCenter() + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
        boundingNorms[lineCount] = Vector2D(1, 0);
        onInside[lineCount] = (rightNeighbor == NULL || rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            rightNeighbor->GetType() == LevelPiece::OneWay || rightNeighbor->GetType() == LevelPiece::NoEntry ||
            rightNeighbor->GetType() == LevelPiece::Breakable || rightNeighbor->GetType() == LevelPiece::BreakableTriangle ||
            rightNeighbor->GetType() == LevelPiece::Bomb || rightNeighbor->GetType() == LevelPiece::PrismTriangle ||
            rightNeighbor->GetType() == LevelPiece::Regen);
        lineCount++;
    }
    shouldGenBounds = false;

    // Top boundary of the piece
    if (topNeighbor != NULL) {
        if (topNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
            topNeighbor->GetType() != LevelPiece::Solid &&
            topNeighbor->GetType() != LevelPiece::LaserTurret && 
            topNeighbor->GetType() != LevelPiece::RocketTurret &&
            topNeighbor->GetType() != LevelPiece::MineTurret && 
            topNeighbor->GetType() != LevelPiece::Tesla && 
            topNeighbor->GetType() != LevelPiece::Switch &&
            topNeighbor->GetType() != LevelPiece::ItemDrop) {

                shouldGenBounds = true;
                if (TriangleBlock::GetOrientation(topNeighbor, triOrientation)) {
                    shouldGenBounds = !(triOrientation == TriangleBlock::LowerRight || triOrientation == TriangleBlock::LowerLeft) ||
                        topNeighbor->GetType() == LevelPiece::PrismTriangle;
                }
        }
    }
    else {
        shouldGenBounds = true;
    }
    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(thePiece->GetCenter() + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
            thePiece->GetCenter() + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
        boundingNorms[lineCount] = Vector2D(0, 1);
        onInside[lineCount] = (topNeighbor == NULL || topNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
            topNeighbor->GetType() == LevelPiece::OneWay ||
            topNeighbor->GetType() == LevelPiece::NoEntry || topNeighbor->GetType() == LevelPiece::Breakable ||
            topNeighbor->GetType() == LevelPiece::BreakableTriangle || topNeighbor->GetType() == LevelPiece::Bomb ||
            topNeighbor->GetType() == LevelPiece::PrismTriangle || topNeighbor->GetType() == LevelPiece::Regen);
        lineCount++;
    }

    thePiece->SetBounds(BoundingLines(lineCount, boundingLines, boundingNorms, onInside),
        leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

void LevelPiece::RemoveAllStatus(GameLevel* level) {

	// If the piece has no status effects then just exit, this saves us the trouble
	// of emitting an event that could potentially be a more expensive operation
	if (this->pieceStatus == LevelPiece::NormalStatus) {
		return;
	}

	bool hadIceCubeStatus = this->HasStatus(LevelPiece::IceCubeStatus);
	this->pieceStatus = LevelPiece::NormalStatus;
	if (hadIceCubeStatus) {
		// We update the bounds since ice cubes change the bounds of a block - this must be done
		// AFTER the change to the status
        if (level != NULL) {
		    level->UpdateBoundsOnPieceAndSurroundingPieces(this);
        }
	}

	// EVENT: All status effects removed from this piece...
	GameEventManager::Instance()->ActionLevelPieceAllStatusRemoved(*this);
}

// Helper function used to light this piece on fire (if it is allowed to catch on fire).
void LevelPiece::LightPieceOnFire(GameModel* gameModel, bool canCatchOnFire) {
	// If this piece is currently in an ice cube then just melt the ice cube but don't
	// make the block catch on fire...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
        UNUSED_VARIABLE(success);
		assert(success);

        // EVENT: Frozen block canceled-out by fire
        GameEventManager::Instance()->ActionBlockIceCancelledWithFire(*this);
	}
	else if (canCatchOnFire) {
        this->AddStatus(gameModel->GetCurrentLevel(), LevelPiece::OnFireStatus);
		gameModel->AddStatusUpdateLevelPiece(this, LevelPiece::OnFireStatus);
	}
}

// Helper function used to freeze this piece in an ice block (if it is allowed to be frozen).
void LevelPiece::FreezePieceInIce(GameModel* gameModel, bool canBeFrozen) {

	// If this piece is currently on fire then the ice will cancel with the fire and
	// the piece will no longer be on fire
	if (this->HasStatus(LevelPiece::OnFireStatus)) {
		bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::OnFireStatus);
        UNUSED_VARIABLE(success);
		assert(success);

        // EVENT: On-fire block canceled-out by ice
        GameEventManager::Instance()->ActionBlockFireCancelledWithIce(*this);
	}
	else if (canBeFrozen) {
		// The ball is icy, encapsulate this piece in an ice cube, this will make the ball take
		// have to hit this piece once more in order to destroy it...
        this->AddStatus(gameModel->GetCurrentLevel(), LevelPiece::IceCubeStatus);
		gameModel->AddStatusUpdateLevelPiece(this, LevelPiece::IceCubeStatus);
	}
}

/**
 * Call this in order to properly set the projectile (and possibly spawn any extra projectiles) when
 * this piece is frozen in ice.
 */
void LevelPiece::DoIceCubeReflectRefractLaserBullets(Projectile* projectile, GameModel* gameModel) const {

	assert(this->HasStatus(LevelPiece::IceCubeStatus));
	assert(projectile != NULL);
	assert(gameModel != NULL);

	// Need to figure out if this laser bullet already collided with this block... 
	// If it has then we just ignore it
	if (projectile->IsLastThingCollidedWith(this)) {
		return;
	}
	
	std::list<Collision::Ray2D> rays;
	this->GetIceCubeReflectionRefractionRays(projectile->GetPosition(), projectile->GetVelocityDirection(), rays);
	assert(rays.size() >= 1);
					
	std::list<Collision::Ray2D>::iterator rayIter = rays.begin();

    // Make refracted rays smaller based on how many there are
    float scaleFactor = Projectile::GetProjectileSplitScaleFactor(rays.size());

    projectile->SetWidth(scaleFactor * projectile->GetWidth());
    projectile->SetHeight(scaleFactor * projectile->GetHeight());

	// The first ray is how the current projectile gets transmitted through this block...
	projectile->SetPosition(rayIter->GetOrigin());
	projectile->SetVelocity(rayIter->GetUnitDirection(), projectile->GetVelocityMagnitude());
	projectile->SetLastThingCollidedWith(this);

	// All the other rays were created via refraction or some such thing, so spawn new particles for them
	++rayIter;
	for (; rayIter != rays.end(); ++rayIter) {
        Projectile* newProjectile = Projectile::CreateProjectileFromCopy(projectile, true);
		
        newProjectile->SetWidth(scaleFactor * newProjectile->GetWidth());
        newProjectile->SetHeight(scaleFactor * newProjectile->GetHeight());
        newProjectile->SetPosition(rayIter->GetOrigin());
		newProjectile->SetVelocity(rayIter->GetUnitDirection(), projectile->GetVelocityMagnitude());
		newProjectile->SetLastThingCollidedWith(this); // If we don't do this then it will cause recursive doom

		gameModel->AddProjectile(newProjectile);
	}
}

/**
 * Gets the expected reflection/refraction rays for a levelpiece that's been frozen in an ice cube status.
 * The resulting rays are based on the position of the projectile at impact relative to this piece.
 */
void LevelPiece::GetIceCubeReflectionRefractionRays(const Point2D& currCenter, 
                                                    const Vector2D& currDir, 
                                                    std::list<Collision::Ray2D>& rays) const {

	Collision::Ray2D defaultRay(currCenter, currDir);	// This is what happens to the original ray
	
	// Determine how the ray will move through the ice cube based on where it hit...
	const Vector2D delta										= currCenter - this->GetCenter();
	static const float HEIGHT_FUZZINESS     = LevelPiece::PIECE_HEIGHT / 15.0f;
	static const float WIDTH_FUZZINESS      = LevelPiece::PIECE_WIDTH  / 15.0f;
	static const float HALF_PIECE_WIDTH     = LevelPiece::PIECE_WIDTH  / 2.0f;
	static const float HALF_PIECE_HEIGHT    = LevelPiece::PIECE_HEIGHT / 2.0f;

	static const float LEFT_FUZZ_LINE_X   =  -HALF_PIECE_WIDTH  + WIDTH_FUZZINESS;
	static const float RIGHT_FUZZ_LINE_X  =   HALF_PIECE_WIDTH  - WIDTH_FUZZINESS;
	static const float TOP_FUZZ_LINE_Y    =   HALF_PIECE_HEIGHT - HEIGHT_FUZZINESS;
	static const float BOTTOM_FUZZ_LINE_Y =  -HALF_PIECE_HEIGHT + HEIGHT_FUZZINESS;

	// Figure out what face of the ice cube it hit...
	if (delta[0] <= LEFT_FUZZ_LINE_X) {

		if (delta[1] < BOTTOM_FUZZ_LINE_Y) {
			// Collision was in the bottom left corner
			static const Vector2D BOTTOM_LEFT_NORMAL = Vector2D(-1.0f, -1.0f) / SQRT_2;
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-currDir, BOTTOM_LEFT_NORMAL)))));
			if (angleBetweenNormalAndLaser <= 20) {
				// The ray will split...
				rays.push_back(Collision::Ray2D(this->GetCenter(), Vector2D(0.0f, 1.0f)));
				rays.push_back(Collision::Ray2D(this->GetCenter(), Vector2D(1.0f, 0.0f)));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
		else if (delta[1] > TOP_FUZZ_LINE_Y) {
			// Collision was in the top left corner
			static const Vector2D BOTTOM_LEFT_NORMAL = Vector2D(-1.0f, 1.0f) / SQRT_2;
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-currDir, BOTTOM_LEFT_NORMAL)))));
			if (angleBetweenNormalAndLaser <= 20) {
				// The ray will split...
				rays.push_back(Collision::Ray2D(this->GetCenter(), Vector2D(0.0f, -1.0f)));
				rays.push_back(Collision::Ray2D(this->GetCenter(), Vector2D(1.0f, 0.0f)));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
		else {
			// Collision was on the left side
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-currDir, Vector2D(-1.0f, 0.0f))))));
			if (angleBetweenNormalAndLaser <= 60) {
				// The ray will just go striaght right 
				defaultRay.SetUnitDirection(Vector2D(1.0f, 0.0f));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
	}
	else if (delta[0] >= RIGHT_FUZZ_LINE_X) {
		
		if (delta[1] < BOTTOM_FUZZ_LINE_Y) {
			// Collision was in the bottom right corner
			static const Vector2D BOTTOM_LEFT_NORMAL = Vector2D(1.0f, -1.0f) / SQRT_2;
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-currDir, BOTTOM_LEFT_NORMAL)))));
			if (angleBetweenNormalAndLaser <= 20) {
				// The ray will split...
				rays.push_back(Collision::Ray2D(this->GetCenter(), Vector2D(0.0f, 1.0f)));
				rays.push_back(Collision::Ray2D(this->GetCenter(), Vector2D(-1.0f, 0.0f)));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
		else if (delta[1] > TOP_FUZZ_LINE_Y) {
			// Collision was in the top right corner
			static const Vector2D BOTTOM_LEFT_NORMAL = Vector2D(1.0f, 1.0f) / SQRT_2;
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-currDir, BOTTOM_LEFT_NORMAL)))));
			if (angleBetweenNormalAndLaser <= 20) {
				// The ray will split...
				rays.push_back(Collision::Ray2D(this->GetCenter(), Vector2D(0.0f, -1.0f)));
				rays.push_back(Collision::Ray2D(this->GetCenter(), Vector2D(-1.0f, 0.0f)));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
		else {
			// Collision was on the right side
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-currDir, Vector2D(-1.0f, 0.0f))))));
			if (angleBetweenNormalAndLaser <= 60) {
				// The ray will just go striaght right 
				defaultRay.SetUnitDirection(Vector2D(-1.0f, 0.0f));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
	}
	else {
		// Collision was on either the bottom or top sides
		if (delta[1] >= 0) {
			// Top
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-currDir, Vector2D(0.0f, 1.0f))))));
			if (angleBetweenNormalAndLaser <= 60) {
				// The ray will just go striaght down 
				defaultRay.SetUnitDirection(Vector2D(0.0f, -1.0f));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
		else {
			// Bottom
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-currDir, Vector2D(0.0f, -1.0f))))));
			if (angleBetweenNormalAndLaser <= 60) {
				// The ray will just go striaght up 
				defaultRay.SetUnitDirection(Vector2D(0.0f, 1.0f));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
	}

	rays.push_front(defaultRay);
}

bool LevelPiece::DoPossibleFireGlobDrop(GameModel* gameModel, bool alwaysDrop) const {
    if (!alwaysDrop) {
        int fireGlobDropRandomNum = Randomizer::GetInstance()->RandomUnsignedInt() % GameModelConstants::GetInstance()->FIRE_GLOB_CHANCE_MOD;
        if (fireGlobDropRandomNum != 0) {
            return false;
        }
    }

	float globSize  = 0.4f * LevelPiece::HALF_PIECE_WIDTH + 0.9f * Randomizer::GetInstance()->RandomNumZeroToOne() * LevelPiece::HALF_PIECE_WIDTH;
	float edgeDist  = ((LevelPiece::PIECE_WIDTH - globSize) / 2.0f) - 0.01f;
	assert(edgeDist >= 0.0f);

    Vector2D worldGravityDir = gameModel->GetGravityDir().ToVector2D();
    Vector2D worldGravityRightDir = gameModel->GetGravityRightDir().ToVector2D();

	// Calculate a place on the block to drop the glob from...
	Point2D dropPos = this->GetCenter() + (Randomizer::GetInstance()->RandomNumNegOneToOne() * edgeDist) * worldGravityRightDir + 
        (globSize / 2.0f) * worldGravityDir;

	// Drop a glob of fire downwards from the block...
	Projectile* fireGlobProjectile = new FireGlobProjectile(dropPos, globSize, worldGravityDir);
	fireGlobProjectile->SetLastThingCollidedWith(this);
	gameModel->AddProjectile(fireGlobProjectile);
    return true;
}
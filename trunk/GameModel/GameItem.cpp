/**
 * GameItem.cpp
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

#include "GameItem.h"
#include "GameModel.h"
#include "PortalBlock.h"

// Width and Height constants for items
const float GameItem::ITEM_WIDTH	= 2.45f;
const float GameItem::ITEM_HEIGHT	= 1.0f;
const float GameItem::HALF_ITEM_WIDTH		= ITEM_WIDTH / 2.0f;
const float GameItem::HALF_ITEM_HEIGHT	= ITEM_HEIGHT / 2.0f;

// How fast items descend upon the paddle
const float GameItem::SPEED_OF_DESCENT	= 4.4f;

// The alpha of an item when the paddle camera is active
const float GameItem::ALPHA_ON_PADDLE_CAM = 0.9f;
const float GameItem::ALPHA_ON_BALL_CAM		= 0.75f;

GameItem::GameItem(const std::string& name, const Point2D &spawnOrigin, const Vector2D& dropDir,
                   GameModel *gameModel, const GameItem::ItemDisposition disp) : 
	name(name), center(spawnOrigin), gameModel(gameModel), disposition(disp), 
        isActive(false), colour(1, 1, 1, 1), currVelocityDir(dropDir), rotationInDegs(0.0f) {
	assert(gameModel != NULL);

	// If the paddle camera is currently active then we must set the item
	// to be partially transparent
	PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
	if (paddle->GetIsPaddleCameraOn()) {
		this->colour[3] = GameItem::ALPHA_ON_PADDLE_CAM;
	}
	else if (GameBall::GetIsBallCameraOn()) {
		this->colour[3] = GameItem::ALPHA_ON_BALL_CAM;
	}

	this->colourAnimation = AnimationLerp<ColourRGBA>(&this->colour);
	this->colourAnimation.SetRepeat(false);

    this->currVelocityDir.Normalize();
    this->rotationInDegs = Trig::radiansToDegrees(acos(Vector2D::Dot(this->currVelocityDir, Vector2D(0,-1))));
}

GameItem::~GameItem() {
}

/**
 * Adds an animation to the item that sets its alpha based on the
 * given parameter over the given amount of time (linearly animating
 * it from its current alpha).
 */
void GameItem::AnimateItemFade(float endAlpha, double duration) {
	ColourRGBA finalColour = this->colour;
	finalColour[3] = endAlpha;
	this->colourAnimation.SetLerp(duration, finalColour);
}

/**
 * This function will execute the movement / action of the item as it progresses
 * through game time, given in seconds.
 */
void GameItem::Tick(double seconds, const GameModel& model) {

	// Tick any item-related animations
	this->colourAnimation.Tick(seconds);

    // Check to see whether the item collided with a portal block, if it
    // did then we need it to teleport...
    const LevelPiece* piece = model.GetCurrentLevel()->GetLevelPieceAt(this->center);
    if (piece != NULL && piece->GetType() == LevelPiece::Portal && 
        this->portalsEntered.find(piece) == this->portalsEntered.end()) {
        
        const PortalBlock* portal        = static_cast<const PortalBlock*>(piece);
        const PortalBlock* siblingPortal = portal->GetSiblingPortal();

        // EVENT: An item will be teleported between portal blocks
        GameEventManager::Instance()->ActionItemPortalBlockTeleport(*this, *portal);

        // Teleport: set the new position for this item at the sibling portal...
        Vector2D posDiff = this->center - piece->GetCenter();
        this->center = siblingPortal->GetCenter() + posDiff;

        // Keep track of the portals that this item has been through so that it doesn't teleport forever
        this->portalsEntered.insert(portal);
        this->portalsEntered.insert(siblingPortal);
    }

	// With every tick we simply move the item down at its
	// descent speed, all other activity (e.g., player paddle collision) 
	// is taken care of by the game
    model.GetPlayerPaddle()->AugmentDirectionOnPaddleMagnet(seconds, 60.0f, this->center, this->currVelocityDir);
	Vector2D dV = SPEED_OF_DESCENT * seconds * this->currVelocityDir;
	this->center = this->center + dV;
}

/**
 * This function will check for collisions with this item and
 * the given player paddle.
 * Returns: true on collision, false otherwise.
 */
bool GameItem::CollisionCheck(const PlayerPaddle &paddle) {

	// Create basic AABB for both paddle and item and then
	// check for a collision in 2D.	
	Collision::AABB2D paddleAABB(paddle.GetCenterPosition() - Vector2D(paddle.GetHalfWidthTotal(), paddle.GetHalfHeight()), 
		                           paddle.GetCenterPosition() + Vector2D(paddle.GetHalfWidthTotal(), paddle.GetHalfHeight()));
	Collision::AABB2D itemAABB(this->GetCenter() - Vector2D(HALF_ITEM_WIDTH, HALF_ITEM_HEIGHT), 
														 this->GetCenter() + Vector2D(HALF_ITEM_WIDTH, HALF_ITEM_HEIGHT));
	return Collision::IsCollision(paddleAABB, itemAABB);
}

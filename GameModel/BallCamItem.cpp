/**
 * BallCamItem.cpp
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

#include "BallCamItem.h"
#include "GameModel.h"
#include "GameTransformMgr.h"
#include "CannonBlock.h"

#include "../BlammoEngine/BasicIncludes.h"

const char*  BallCamItem::BALL_CAM_ITEM_NAME	    = "BallCam";
const double BallCamItem::BALL_CAM_TIMER_IN_SECS	= 15.0;

BallCamItem::BallCamItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(BallCamItem::BALL_CAM_ITEM_NAME, spawnOrigin, gameModel, GameItem::Bad) {
}

BallCamItem::~BallCamItem() {
}

double BallCamItem::Activate() {
	this->isActive = true;

	// Deactivate any currently active paddle camera items and any other associated viewpoint changing items
	// TODO: DONT FORGET ABOUT THIS (SEE COMMENT ABOVE) !!!
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::BallCamItem || currTimer->GetTimerItemType() == GameItem::PaddleCamItem) {
			iter = activeTimers.erase(iter);
			delete currTimer;
			currTimer = NULL;
		}
		else {
			 ++iter;
		}
	}

	this->gameModel->GetTransformInfo()->SetBallCamera(true);
	GameItem::Activate();

	return BallCamItem::BALL_CAM_TIMER_IN_SECS;
}

void BallCamItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Deactivate the ball camera effect
	this->gameModel->GetTransformInfo()->SetBallCamera(false);

	this->isActive = false;
	GameItem::Deactivate();
}
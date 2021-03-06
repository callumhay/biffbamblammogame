/**
 * PaddleCamItem.cpp
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

#include "PaddleCamItem.h"
#include "GameModel.h"
#include "GameTransformMgr.h"

const char* PaddleCamItem::PADDLE_CAM_ITEM_NAME			  = "PaddleCam";
const double PaddleCamItem::PADDLE_CAM_TIMER_IN_SECS	= 17.0;

PaddleCamItem::PaddleCamItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) : 
GameItem(PaddleCamItem::PADDLE_CAM_ITEM_NAME, spawnOrigin, dropDir, gameModel, GameItem::Bad) {
}

PaddleCamItem::~PaddleCamItem() {
}

double PaddleCamItem::Activate() {
	this->isActive = true;

	// Deactivate any currently active paddle camera items and any other associated viewpoint changing items
	// TODO: DONT FORGET ABOUT THIS (SEE COMMENT ABOVE) !!!
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::PaddleCamItem || currTimer->GetTimerItemType() == GameItem::BallCamItem) {
			iter = activeTimers.erase(iter);
			delete currTimer;
			currTimer = NULL;
		}
		else {
			 ++iter;
		}
	}

	// Activate the paddle camera effect
	this->gameModel->GetTransformInfo()->SetPaddleCamera(true);
	GameItem::Activate();

	return PaddleCamItem::PADDLE_CAM_TIMER_IN_SECS;
}

void PaddleCamItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Deactivate the paddle camera effect
	this->gameModel->GetTransformInfo()->SetPaddleCamera(false);

	this->isActive = false;
	GameItem::Deactivate();
}
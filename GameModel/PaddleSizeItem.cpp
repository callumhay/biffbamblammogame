/**
 * PaddleSizeItem.cpp
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

#include "PaddleSizeItem.h"
#include "GameModel.h"
#include "PlayerPaddle.h"

const double PaddleSizeItem::PADDLE_SIZE_TIMER_IN_SECS	= 0.0;

const char* PaddleSizeItem::PADDLE_GROW_ITEM_NAME		= "PaddleGrow";
const char* PaddleSizeItem::PADDLE_SHRINK_ITEM_NAME = "PaddleShrink";

PaddleSizeItem::PaddleSizeItem(const PaddleSizeChangeType type, const Point2D &spawnOrigin,
                               const Vector2D& dropDir, GameModel *gameModel) : 
GameItem(((type == ShrinkPaddle) ? PADDLE_SHRINK_ITEM_NAME : PADDLE_GROW_ITEM_NAME), spawnOrigin, dropDir, gameModel, 
				 ((type == ShrinkPaddle) ? GameItem::Bad : GameItem::Good)), sizeChangeType(type) {
}

PaddleSizeItem::~PaddleSizeItem() {
}

double PaddleSizeItem::Activate() {
	this->isActive = true;

	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	bool paddleSizeChanged = false;

	switch(this->sizeChangeType) {
		case ShrinkPaddle:
			paddleSizeChanged = paddle->DecreasePaddleSize();
			break;
		case GrowPaddle:
			paddleSizeChanged = paddle->IncreasePaddleSize();
			break;
		default:
			assert(false);
			break;
	}

	// Nothing gets activated if there was no effect...
	if (paddleSizeChanged) {
		GameItem::Activate();
	}
	
	return PaddleSizeItem::PADDLE_SIZE_TIMER_IN_SECS;
}

/** 
 * Does nothing since the paddle size item has no timer or way of being cancelled out.
 */
void PaddleSizeItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	this->isActive = false;
	GameItem::Deactivate();
}
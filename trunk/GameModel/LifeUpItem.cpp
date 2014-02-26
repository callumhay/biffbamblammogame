/**
 * LifeUpItem.cpp
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

#include "LifeUpItem.h"

const char* LifeUpItem::LIFE_UP_ITEM_NAME	    = "LifeUp";
const double LifeUpItem::LIFE_UP_TIMER_IN_SECS	= 0.0;

LifeUpItem::LifeUpItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(LifeUpItem::LIFE_UP_ITEM_NAME, spawnOrigin, gameModel, GameItem::Good) {
}

LifeUpItem::~LifeUpItem() {
}

double LifeUpItem::Activate() {
	this->isActive = true;

	int numLivesLeft = this->gameModel->GetLivesLeft();
	assert(numLivesLeft >= 0);
	numLivesLeft++;
	this->gameModel->SetLivesLeft(numLivesLeft);

	GameItem::Activate();
	return LifeUpItem::LIFE_UP_TIMER_IN_SECS;
}

void LifeUpItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	this->isActive = false;
	GameItem::Deactivate();
}
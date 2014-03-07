/**
 * RandomItem.cpp
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

#include "RandomItem.h"
#include "GameItemFactory.h"

const char*  RandomItem::RANDOM_ITEM_NAME = "Random";
const long RandomItem::TIME_BETWEEN_BLINKS_IN_MS = 100;

RandomItem::RandomItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) :
GameItem(RandomItem::RANDOM_ITEM_NAME, spawnOrigin, dropDir, gameModel, GameItem::Neutral), randomItem(NULL),
currRandomIdx(-1), lastBlinkTime(0) {

	// Build a list of all possible random items, but make sure the random item type isn't included in there...
    this->possibleItemDropTypes = gameModel->GetCurrentLevel()->GetAllowableItemDropTypes();
    for (std::vector<GameItem::ItemType>::iterator iter = this->possibleItemDropTypes.begin();
         iter != this->possibleItemDropTypes.end(); ++iter) {

        if (*iter == GameItem::RandomItem) {
            this->possibleItemDropTypes.erase(iter);
            break;
        }
    }

    this->currRandomIdx = Randomizer::GetInstance()->RandomUnsignedInt() % static_cast<int>(this->possibleItemDropTypes.size());
    this->lastBlinkTime = BlammoTime::GetSystemTimeInMillisecs();
}

RandomItem::~RandomItem() {
    if (this->randomItem != NULL) {
	    delete this->randomItem;
	    this->randomItem = NULL;
    }
}

double RandomItem::Activate() {

    // Choose the random item...
    assert(this->randomItem == NULL);
    GameItem::ItemType randomItemType = this->possibleItemDropTypes[currRandomIdx];
    this->randomItem = GameItemFactory::GetInstance()->CreateItem(randomItemType, this->GetCenter(), this->currVelocityDir, gameModel);
    assert(this->randomItem != NULL);
    assert(this->randomItem->GetItemType() != GameItem::RandomItem);

    // Activate it!
    this->isActive = true;
    this->SetItemDisposition(this->randomItem->GetItemDisposition());
    this->SetItemName(this->randomItem->GetName());
    GameEventManager::Instance()->ActionRandomItemActivated(*this, *this->randomItem);
    return this->randomItem->Activate();
}

GameItem::ItemType RandomItem::GetItemType() const {
    if (this->isActive) {
        assert(this->randomItem != NULL);
        return this->randomItem->GetItemType();
    }
    return GameItem::RandomItem;
}

GameItem::ItemType RandomItem::GetBlinkingRandomItemType() const {
    unsigned long currTime = BlammoTime::GetSystemTimeInMillisecs();
    if (labs(static_cast<long>(currTime) - static_cast<long>(this->lastBlinkTime)) >= TIME_BETWEEN_BLINKS_IN_MS) {
        // Choose a new random item to show...
        this->currRandomIdx = (this->currRandomIdx + 1 + (Randomizer::GetInstance()->RandomUnsignedInt() % 
            static_cast<int>(this->possibleItemDropTypes.size())-1)) % static_cast<int>(this->possibleItemDropTypes.size());
        this->lastBlinkTime = currTime;
    }

    return this->possibleItemDropTypes[this->currRandomIdx];
}
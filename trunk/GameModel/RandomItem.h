/**
 * RandomItem.h
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

#ifndef __RANDOMITEM_H__
#define __RANDOMITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"
#include "GameModel.h"
#include "GameEventManager.h"

// A random item can be any sort of other item in the game - It wraps up the random item in itself so
// that it drops as an entirely separate type of item.
class RandomItem : public GameItem {
public:
	static const char* RANDOM_ITEM_NAME;

	RandomItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel);
	~RandomItem();
	
	std::set<const GameBall*> GetBallsAffected() const ;
	double Activate();
	void Deactivate();
	GameItem::ItemType GetItemType() const;

    GameItem::ItemType GetBlinkingRandomItemType() const;

private:
    static const long TIME_BETWEEN_BLINKS_IN_MS;

	GameItem* randomItem;
    std::vector<GameItem::ItemType> possibleItemDropTypes;
    
    mutable int currRandomIdx;
    mutable unsigned long lastBlinkTime;

    DISALLOW_COPY_AND_ASSIGN(RandomItem);
};

inline std::set<const GameBall*> RandomItem::GetBallsAffected() const {
	return this->randomItem->GetBallsAffected();
}

inline void RandomItem::Deactivate() {
	this->randomItem->Deactivate();
}

#endif
/**
 * MultiBallItem.h
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

#ifndef __MULTIBALLITEM_H__
#define __MULTIBALLITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"

class Point2D;

class MultiBallItem : public GameItem {
public:
	enum NumMultiBalls { ThreeMultiBalls = 3, FiveMultiBalls = 5 };
	
	static const char* MULTI3_BALL_ITEM_NAME;
	static const char* MULTI5_BALL_ITEM_NAME;
	
	static const double MULTI_BALL_TIMER_IN_SECS;
	static const double BALL_COLLISIONS_DISABLED_DURATION;

	static const unsigned int MAX_NUM_SPLITBALLS = 5;		// Maximum number of balls that can split off in a multiball
	static const unsigned int MIN_NUM_SPLITBALLS = 1;		// Minimum number of balls that can split off in a multiball
	static const float MIN_SPLIT_DEGREES;								// Minimum number of degrees between balls when splitting

	MultiBallItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel, NumMultiBalls numBalls);
	~MultiBallItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return (this->numBalls == ThreeMultiBalls) ? GameItem::MultiBall3Item : GameItem::MultiBall5Item;
	}

private:
	NumMultiBalls numBalls;
	unsigned int numNewSpawnedBalls;	// The number of balls created for each preexisting gameball in play

	static std::string NameFromNumBalls(NumMultiBalls n) {
		switch (n) {
			case ThreeMultiBalls:
				return MULTI3_BALL_ITEM_NAME;
			case FiveMultiBalls:
				return MULTI5_BALL_ITEM_NAME;
			default:
				assert(false);
				break;
		}
		return MULTI3_BALL_ITEM_NAME;
	}

    DISALLOW_COPY_AND_ASSIGN(MultiBallItem);
};

#endif
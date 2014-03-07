/**
 * PaddleCamItem.h
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

#ifndef __PADDLECAMITEM_H__
#define __PADDLECAMITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"

/**
 * Item for placing the user's viewpoint inside the paddle looking upwards
 * into the level. Brutal power-down of doom!
 */
class PaddleCamItem : public GameItem {
public:
	static const char* PADDLE_CAM_ITEM_NAME;
	static const double PADDLE_CAM_TIMER_IN_SECS;

	PaddleCamItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel);
	~PaddleCamItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::PaddleCamItem;
	}

private:
    DISALLOW_COPY_AND_ASSIGN(PaddleCamItem);
};
#endif
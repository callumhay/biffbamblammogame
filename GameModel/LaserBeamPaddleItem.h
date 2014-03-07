/**
 * LaserBeamPaddleItem.h
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

#ifndef __LASERBEAMPADDLEITEM_H_
#define __LASERBEAMPADDLEITEM_H_

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"

class LaserBeamPaddleItem : public GameItem {
public:
	static const char* LASER_BEAM_PADDLE_ITEM_NAME;
	static const double LASER_BEAM_PADDLE_TIMER_IN_SECS;

	LaserBeamPaddleItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel);
	~LaserBeamPaddleItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::LaserBeamPaddleItem;
	}

private:
    DISALLOW_COPY_AND_ASSIGN(LaserBeamPaddleItem);
};

#endif // __LASERBEAMPADDLEITEM_H_
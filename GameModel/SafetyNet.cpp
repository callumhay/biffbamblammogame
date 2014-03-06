/**
 * SafetyNet.cpp
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

#include "SafetyNet.h"
#include "GameLevel.h"
#include "PaddleMineProjectile.h"

const float SafetyNet::SAFETY_NET_HEIGHT      = 1.0f;
const float SafetyNet::SAFETY_NET_HALF_HEIGHT = SafetyNet::SAFETY_NET_HEIGHT / 2.0f;

SafetyNet::SafetyNet(const GameLevel& currLevel) {

    const LevelPiece* maxBoundPiece = currLevel.GetMaxXPaddleBoundPiece(0);
    const LevelPiece* minBoundPiece = currLevel.GetMinXPaddleBoundPiece(0);

	// Create the safety net bounding line for this level
	std::vector<Collision::LineSeg2D> lines;
	lines.reserve(2);
    Collision::LineSeg2D safetyNetLine1(
        Point2D(minBoundPiece->GetCenter()[0] - LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
        Point2D(maxBoundPiece->GetCenter()[0] + LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
    Collision::LineSeg2D safetyNetLine2(
        Point2D(minBoundPiece->GetCenter()[0] - LevelPiece::HALF_PIECE_WIDTH, -1.5f*LevelPiece::HALF_PIECE_HEIGHT),
        Point2D(maxBoundPiece->GetCenter()[0] + LevelPiece::HALF_PIECE_WIDTH, -1.5f*LevelPiece::HALF_PIECE_HEIGHT));
	lines.push_back(safetyNetLine1);
    lines.push_back(safetyNetLine2);

	std::vector<Vector2D> normals;
	normals.reserve(2);
	normals.push_back(Vector2D(0, 1));
    normals.push_back(Vector2D(0, -1));
	
	this->bounds = BoundingLines(lines, normals);
}

SafetyNet::~SafetyNet() {
    // Make sure we remove all attached projectiles as well
    // NOTE: DO NOT USE ITERATORS HERE SINCE THE MINE PROJECTILE DETACHES ITSELF IN THE SetAsFalling call
    while (!this->attachedProjectiles.empty()) {
       
        MineProjectile* p = *this->attachedProjectiles.begin();
        p->SetLastThingCollidedWith(NULL);
        p->SetAsFalling();
        this->DetachProjectile(p); // This is redundant and will be ignored, just here for robustness
    }
    this->attachedProjectiles.clear();
}

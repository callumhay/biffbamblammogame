/**
 * FragileCannonBlock.cpp
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

#include "FragileCannonBlock.h"
#include "GameEventManager.h"
#include "GameModel.h"
#include "GameLevel.h"
#include "EmptySpaceBlock.h"

const float FragileCannonBlock::POST_X_TRANSLATION = 1.05f;
const float FragileCannonBlock::POST_Y_TRANSLATION = 0.0f;
const float FragileCannonBlock::POST_Z_TRANSLATION = -0.203f;

FragileCannonBlock::FragileCannonBlock(unsigned int wLoc, unsigned int hLoc, int setRotation) : 
CannonBlock(wLoc, hLoc, setRotation) {
}

FragileCannonBlock::FragileCannonBlock(unsigned int wLoc, unsigned int hLoc, const std::pair<int, int>& rotationInterval) : 
CannonBlock(wLoc, hLoc, rotationInterval) {
}

FragileCannonBlock::~FragileCannonBlock() {
}

LevelPiece* FragileCannonBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {

    // Make sure that the block is destroying itself!
    if (method != LevelPiece::SelfDestruction) {
        return this;
    }
    
    // EVENT: Block is being destroyed
    GameEventManager::Instance()->ActionBlockDestroyed(*this, method);

    // Tell the level that this piece has changed to empty...
    GameLevel* level = gameModel->GetCurrentLevel();
    LevelPiece* emptyPiece = new EmptySpaceBlock(this->wIndex, this->hIndex);
    level->PieceChanged(gameModel, this, emptyPiece, method);

    // Obliterate all that is left of this block...
    LevelPiece* tempThis = this;
    delete tempThis;
    tempThis = NULL;

    return emptyPiece;
}
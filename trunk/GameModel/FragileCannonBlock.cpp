/**
 * FragileCannonBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2014
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
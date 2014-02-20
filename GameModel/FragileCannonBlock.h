/**
 * FragileCannonBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2014
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __FRAGILECANNONBLOCK_H__
#define __FRAGILECANNONBLOCK_H__

#include "CannonBlock.h"

class FragileCannonBlock : public CannonBlock {
public:
    static const float POST_X_TRANSLATION;
    static const float POST_Y_TRANSLATION;
    static const float POST_Z_TRANSLATION;

    FragileCannonBlock(unsigned int wLoc, unsigned int hLoc, int setRotation);
    FragileCannonBlock(unsigned int wLoc, unsigned int hLoc, const std::pair<int, int>& rotationInterval);
    ~FragileCannonBlock();

    LevelPieceType GetType() const { return LevelPiece::FragileCannon; }

    LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);
    
private:
    LevelPiece* CannonWasFired(GameModel* gameModel) {
        return this->Destroy(gameModel, LevelPiece::SelfDestruction);
    }

    DISALLOW_COPY_AND_ASSIGN(FragileCannonBlock);
};

#endif // __FRAGILECANNONBLOCK_H__
/**
 * DecoBoss.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __DECOBOSS_H__
#define __DECOBOSS_H__

class BossBodyPart;
class BossWeakpoint;
class BossCompositeBodyPart;

namespace decobossai {
class DecoBossAIState;
class Stage1AI;
};

#include "Boss.h"
#include "GameLevel.h"
#include "TeslaBlock.h"

class DecoBoss : public Boss {
    
    friend Boss* Boss::BuildStyleBoss(const GameWorld::WorldStyle& style);
    friend class decobossai::DecoBossAIState;
    friend class decobossai::Stage1AI;

public:
    static const float CORE_HEIGHT;
    static const float CORE_WIDTH;
    static const float HALF_CORE_HEIGHT;
    static const float CORE_HALF_DEPTH;

    static const float SIDE_BODY_PART_WIDTH;
    static const float SIDE_BODY_PART_HEIGHT;

    static const float FULL_BODY_TOTAL_HEIGHT;
    static const float FULL_BODY_TOTAL_WIDTH;

    static const float CORE_AND_ARMS_WIDTH;

    static const float ARM_POS_X_OFFSET;
    static const float ARM_Y_OFFSET;
    static const float ARM_Z_OFFSET;

    static const float SIDE_POS_X_OFFSET;
    static const float SIDE_Y_OFFSET;
    static const float SIDE_Z_OFFSET;

    static const float LIGHTNING_FIRE_POS_X;
    static const float LIGHTNING_FIRE_POS_Y;
    static const float LIGHTNING_FIRE_POS_Z;

    static const float ITEM_LOAD_OFFSET_Y;

    ~DecoBoss();

    // Middle body parts
    const BossCompositeBodyPart* GetMiddleBody() const { return static_cast<const BossCompositeBodyPart*>(this->bodyParts[this->middleBodyIdx]); }
    const BossBodyPart* GetCore() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->coreIdx]); }
    const BossBodyPart* GetLightningRelay() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->lightningRelayIdx]); }
    
    // Arms (part of the middle body)
    const BossCompositeBodyPart* GetLeftArm() const { return static_cast<const BossCompositeBodyPart*>(this->bodyParts[this->leftArmIdx]); }
    const BossBodyPart* GetLeftArmGear() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftArmGearIdx]); }
    const BossBodyPart* GetLeftArmScopingSeg1() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftArmScopingSeg1Idx]); }
    const BossBodyPart* GetLeftArmScopingSeg2() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftArmScopingSeg2Idx]); }
    const BossBodyPart* GetLeftArmHand() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftArmHandIdx]); }

    const BossCompositeBodyPart* GetRightArm() const { return static_cast<const BossCompositeBodyPart*>(this->bodyParts[this->rightArmIdx]); }
    const BossBodyPart* GetRightArmGear() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightArmGearIdx]); }
    const BossBodyPart* GetRightArmScopingSeg1() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightArmScopingSeg1Idx]); }
    const BossBodyPart* GetRightArmScopingSeg2() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightArmScopingSeg2Idx]); }
    const BossBodyPart* GetRightArmHand() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightArmHandIdx]); }

    // Left and right body parts
    const BossBodyPart* GetLeftBody() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftBodyIdx]); } 
    const BossBodyPart* GetRightBody() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightBodyIdx]); } 

    float GetCurrentHeight() const;
    float GetCurrentWidth() const;

    Point2D GetItemDropPosition() const;
    static float GetItemDropAnimDisplacement();

    static Vector3D GetLightningFireVec3D() { return Vector3D(LIGHTNING_FIRE_POS_X, LIGHTNING_FIRE_POS_Y, LIGHTNING_FIRE_POS_Z); }
    static Vector2D GetLightningFireVec2D() { return Vector2D(LIGHTNING_FIRE_POS_X, LIGHTNING_FIRE_POS_Y); }

    bool IsRightBodyStillAlive() const;
    bool IsLeftBodyStillAlive() const;

    // Inherited from Boss
    bool ProjectilePassesThrough(const Projectile* projectile) const;
    //bool ProjectileIsDestroyedOnCollision(const Projectile* projectile, BossBodyPart* collisionPart) const;

private:
    static const float SCALE_AMT;

    static const int LEVEL_WIDTH_NUM_PIECES  = 25;
    static const int LEVEL_HEIGHT_NUM_PIECES = 32;
    static const int CONFINE_X_PADDING_NUM_PIECES = 5;
    static const int CONFINE_Y_TOP_PADDING_NUM_PIECES = 2;
    static const float MOVEMENT_PADDING_X;
    static const float MOVEMENT_PADDING_Y;
    static const float MOVEMENT_MIN_Y_BOUNDARY;

    static const int LEFT_TESLA_BLOCK_X_IDX  = CONFINE_X_PADDING_NUM_PIECES - 1;
    static const int LEFT_TESLA_BLOCK_Y_IDX  = 17;
    static const int RIGHT_TESLA_BLOCK_X_IDX = LEVEL_WIDTH_NUM_PIECES - CONFINE_X_PADDING_NUM_PIECES;
    static const int RIGHT_TESLA_BLOCK_Y_IDX = LEFT_TESLA_BLOCK_Y_IDX;

    size_t middleBodyIdx, leftBodyIdx, rightBodyIdx;
    size_t coreIdx, lightningRelayIdx;
    size_t leftArmIdx, leftArmGearIdx, leftArmScopingSeg1Idx, leftArmScopingSeg2Idx, leftArmHandIdx;
    size_t rightArmIdx, rightArmGearIdx, rightArmScopingSeg1Idx, rightArmScopingSeg2Idx, rightArmHandIdx;

    DecoBoss();

    // Inherited from Boss
    void Init(float startingX, float startingY);

    BossCompositeBodyPart* BuildArm(BossCompositeBodyPart* middleBody, size_t& armIdx, 
        size_t& gearIdx, size_t& scoping1Idx, size_t& scoping2Idx, size_t& handIdx);

    BossBodyPart* GetLeftBodyEditable() { return static_cast<BossBodyPart*>(this->bodyParts[this->leftBodyIdx]); } 
    BossBodyPart* GetRightBodyEditable() { return static_cast<BossBodyPart*>(this->bodyParts[this->rightBodyIdx]); } 

    static float GetMinXOfConfines() { return DecoBoss::CONFINE_X_PADDING_NUM_PIECES * LevelPiece::PIECE_WIDTH; }
    static float GetMaxXOfConfines() { return (DecoBoss::LEVEL_WIDTH_NUM_PIECES - DecoBoss::CONFINE_X_PADDING_NUM_PIECES) * LevelPiece::PIECE_WIDTH; }
    static float GetMaxYOfConfines() { return (DecoBoss::LEVEL_HEIGHT_NUM_PIECES - DecoBoss::CONFINE_Y_TOP_PADDING_NUM_PIECES) * LevelPiece::PIECE_HEIGHT; }

    static float GetMovementMinXBound() { return DecoBoss::GetMinXOfConfines() + DecoBoss::MOVEMENT_PADDING_X; }
    static float GetMovementMaxXBound() { return DecoBoss::GetMaxXOfConfines() - DecoBoss::MOVEMENT_PADDING_X; }
    static float GetMovementMinYBound() { return DecoBoss::MOVEMENT_MIN_Y_BOUNDARY; }
    static float GetMovementMaxYBound() { return DecoBoss::GetMaxYOfConfines() - DecoBoss::MOVEMENT_PADDING_Y; }

    static TeslaBlock* GetLeftTeslaBlock(const GameLevel& level);
    static TeslaBlock* GetRightTeslaBlock(const GameLevel& level);

    DISALLOW_COPY_AND_ASSIGN(DecoBoss);
};

inline float DecoBoss::GetCurrentHeight() const {
    return ((this->IsRightBodyStillAlive() || this->IsLeftBodyStillAlive()) ? FULL_BODY_TOTAL_HEIGHT : CORE_HEIGHT);
}

inline float DecoBoss::GetCurrentWidth() const {
    return ((this->IsRightBodyStillAlive() || this->IsLeftBodyStillAlive()) ? FULL_BODY_TOTAL_WIDTH : CORE_AND_ARMS_WIDTH);
}

inline Point2D DecoBoss::GetItemDropPosition() const {
    return this->GetCore()->GetTranslationPt2D() + Vector2D(0.0f, DecoBoss::ITEM_LOAD_OFFSET_Y - DecoBoss::GetItemDropAnimDisplacement());
}

inline float DecoBoss::GetItemDropAnimDisplacement() {
    return 1.5f * GameItem::ITEM_HEIGHT;
}

inline bool DecoBoss::IsRightBodyStillAlive() const {
    return !this->GetRightBody()->GetIsDestroyed();
}
inline bool DecoBoss::IsLeftBodyStillAlive() const {
    return !this->GetLeftBody()->GetIsDestroyed();
}

inline TeslaBlock* DecoBoss::GetLeftTeslaBlock(const GameLevel& level) {
    LevelPiece* block = level.GetLevelPieceFromCurrentLayout(LEFT_TESLA_BLOCK_Y_IDX, LEFT_TESLA_BLOCK_X_IDX);
    assert(block->GetType() == LevelPiece::Tesla);
    return static_cast<TeslaBlock*>(block);
}

inline TeslaBlock* DecoBoss::GetRightTeslaBlock(const GameLevel& level) {
    LevelPiece* block = level.GetLevelPieceFromCurrentLayout(RIGHT_TESLA_BLOCK_Y_IDX, RIGHT_TESLA_BLOCK_X_IDX);
    assert(block->GetType() == LevelPiece::Tesla);
    return static_cast<TeslaBlock*>(block);
}

#endif // __DECOBOSS_H__
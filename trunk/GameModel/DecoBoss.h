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
class Stage2AI;
class Stage3AI;
};

#include "Boss.h"
#include "GameLevel.h"
#include "TeslaBlock.h"

class DecoBoss : public Boss {
    
    friend Boss* Boss::BuildStyleBoss(const GameWorld::WorldStyle& style);
    friend class decobossai::DecoBossAIState;
    friend class decobossai::Stage1AI;
    friend class decobossai::Stage2AI;
    friend class decobossai::Stage3AI;

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

    static const float ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION;
    static const float ARM_SCOPING_SEG2_Y_RETRACT_TRANSLATION;
    static const float ARM_SCOPING_SEG3_Y_RETRACT_TRANSLATION;
    static const float ARM_SCOPING_SEG4_Y_RETRACT_TRANSLATION;

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
    const BossBodyPart* GetLeftArmScopingSeg3() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftArmScopingSeg3Idx]); }
    const BossBodyPart* GetLeftArmScopingSeg4() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftArmScopingSeg4Idx]); }
    const BossBodyPart* GetLeftArmHand() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftArmHandIdx]); }

    const BossCompositeBodyPart* GetRightArm() const { return static_cast<const BossCompositeBodyPart*>(this->bodyParts[this->rightArmIdx]); }
    const BossBodyPart* GetRightArmGear() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightArmGearIdx]); }
    const BossBodyPart* GetRightArmScopingSeg1() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightArmScopingSeg1Idx]); }
    const BossBodyPart* GetRightArmScopingSeg2() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightArmScopingSeg2Idx]); }
    const BossBodyPart* GetRightArmScopingSeg3() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightArmScopingSeg3Idx]); }
    const BossBodyPart* GetRightArmScopingSeg4() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightArmScopingSeg4Idx]); }
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
    static const float ROTATION_Y_POSITION;

    static const int LEFT_TESLA_BLOCK_X_IDX  = CONFINE_X_PADDING_NUM_PIECES - 1;
    static const int LEFT_TESLA_BLOCK_Y_IDX  = 17;
    static const int RIGHT_TESLA_BLOCK_X_IDX = LEVEL_WIDTH_NUM_PIECES - CONFINE_X_PADDING_NUM_PIECES;
    static const int RIGHT_TESLA_BLOCK_Y_IDX = LEFT_TESLA_BLOCK_Y_IDX;

    static const float LEFT_ARM_HORIZ_ORIENT_ROT_ANGLE_IN_DEGS;
    static const float RIGHT_ARM_HORIZ_ORIENT_ROT_ANGLE_IN_DEGS;

    static const float ATTACK_PADDLE_WITH_ARMS_MIN_Y_POS;
    static const float ATTACK_PADDLE_WITH_ARMS_MAX_Y_POS;

    size_t middleBodyIdx, leftBodyIdx, rightBodyIdx;
    size_t coreIdx, lightningRelayIdx;
    size_t leftArmIdx, leftArmGearIdx, leftArmScopingSeg1Idx, leftArmScopingSeg2Idx, leftArmScopingSeg3Idx, leftArmScopingSeg4Idx, leftArmHandIdx;
    size_t rightArmIdx, rightArmGearIdx, rightArmScopingSeg1Idx, rightArmScopingSeg2Idx, rightArmScopingSeg3Idx, rightArmScopingSeg4Idx, rightArmHandIdx;

    DecoBoss();

    // Inherited from Boss
    void Init(float startingX, float startingY);

    BossCompositeBodyPart* BuildArm(BossCompositeBodyPart* middleBody, size_t& armIdx, size_t& gearIdx, 
        size_t& scoping1Idx, size_t& scoping2Idx, size_t& scoping3Idx, size_t& scoping4Idx, size_t& handIdx);

    void GenerateArmAndRemainingBoundingLines();

    BossBodyPart* GetCoreEditable() { return static_cast<BossBodyPart*>(this->bodyParts[this->coreIdx]); }
    BossBodyPart* GetLeftBodyEditable() { return static_cast<BossBodyPart*>(this->bodyParts[this->leftBodyIdx]); } 
    BossBodyPart* GetRightBodyEditable() { return static_cast<BossBodyPart*>(this->bodyParts[this->rightBodyIdx]); } 
    
    BossCompositeBodyPart* GetLeftArmEditable() { return static_cast<BossCompositeBodyPart*>(this->bodyParts[this->leftArmIdx]); }
    BossBodyPart* GetLeftArmScopingSeg1Editable() { return static_cast<BossBodyPart*>(this->bodyParts[this->leftArmScopingSeg1Idx]); }
    BossBodyPart* GetLeftArmScopingSeg2Editable() { return static_cast<BossBodyPart*>(this->bodyParts[this->leftArmScopingSeg2Idx]); }
    BossBodyPart* GetLeftArmScopingSeg3Editable() { return static_cast<BossBodyPart*>(this->bodyParts[this->leftArmScopingSeg3Idx]); }
    BossBodyPart* GetLeftArmScopingSeg4Editable() { return static_cast<BossBodyPart*>(this->bodyParts[this->leftArmScopingSeg4Idx]); }
    BossBodyPart* GetLeftArmHandEditable() { return static_cast<BossBodyPart*>(this->bodyParts[this->leftArmHandIdx]); }
    
    BossCompositeBodyPart* GetRightArmEditable() { return static_cast<BossCompositeBodyPart*>(this->bodyParts[this->rightArmIdx]); }
    BossBodyPart* GetRightArmScopingSeg1Editable() { return static_cast<BossBodyPart*>(this->bodyParts[this->rightArmScopingSeg1Idx]); }
    BossBodyPart* GetRightArmScopingSeg2Editable() { return static_cast<BossBodyPart*>(this->bodyParts[this->rightArmScopingSeg2Idx]); }
    BossBodyPart* GetRightArmScopingSeg3Editable() { return static_cast<BossBodyPart*>(this->bodyParts[this->rightArmScopingSeg3Idx]); }
    BossBodyPart* GetRightArmScopingSeg4Editable() { return static_cast<BossBodyPart*>(this->bodyParts[this->rightArmScopingSeg4Idx]); }
    BossBodyPart* GetRightArmHandEditable() { return static_cast<BossBodyPart*>(this->bodyParts[this->rightArmHandIdx]); }

    float GetSideToSideDropStateDistance() const;
    Point2D GetFarLeftDropStatePosition() const;
    Point2D GetFarRightDropStatePosition() const;

    static float GetMinXOfConfines() { return DecoBoss::CONFINE_X_PADDING_NUM_PIECES * LevelPiece::PIECE_WIDTH; }
    static float GetMaxXOfConfines() { return (DecoBoss::LEVEL_WIDTH_NUM_PIECES - DecoBoss::CONFINE_X_PADDING_NUM_PIECES) * LevelPiece::PIECE_WIDTH; }
    static float GetMaxYOfConfines() { return (DecoBoss::LEVEL_HEIGHT_NUM_PIECES - DecoBoss::CONFINE_Y_TOP_PADDING_NUM_PIECES) * LevelPiece::PIECE_HEIGHT; }

    static float GetMovementMinXBound() { return DecoBoss::GetMinXOfConfines() + DecoBoss::MOVEMENT_PADDING_X; }
    static float GetMovementMaxXBound() { return DecoBoss::GetMaxXOfConfines() - DecoBoss::MOVEMENT_PADDING_X; }
    static float GetMovementMinYBound() { return DecoBoss::MOVEMENT_MIN_Y_BOUNDARY; }
    static float GetMovementMaxYBound() { return DecoBoss::GetMaxYOfConfines() - DecoBoss::MOVEMENT_PADDING_Y; }

    static float GetRandomAttackPaddleYPos() { 
        return ATTACK_PADDLE_WITH_ARMS_MIN_Y_POS + Randomizer::GetInstance()->RandomNumZeroToOne() *
            (ATTACK_PADDLE_WITH_ARMS_MAX_Y_POS - ATTACK_PADDLE_WITH_ARMS_MIN_Y_POS);
    }

    static Point2D GetBossPositionForLevelRotation();

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
    return GameItem::ITEM_HEIGHT;
}

inline bool DecoBoss::IsRightBodyStillAlive() const {
    return !this->GetRightBody()->GetIsDestroyed();
}
inline bool DecoBoss::IsLeftBodyStillAlive() const {
    return !this->GetLeftBody()->GetIsDestroyed();
}

inline float DecoBoss::GetSideToSideDropStateDistance() const {
    return DecoBoss::GetMaxXOfConfines() - DecoBoss::GetMinXOfConfines() - this->GetCurrentWidth();
}

inline Point2D DecoBoss::GetFarLeftDropStatePosition() const {
    return Point2D(DecoBoss::GetMinXOfConfines() + this->GetCurrentWidth() / 2.0f, DecoBoss::GetMovementMinYBound() + 0.75f * this->GetCurrentHeight());
}
inline Point2D DecoBoss::GetFarRightDropStatePosition() const {
    return Point2D(DecoBoss::GetMaxXOfConfines() - this->GetCurrentWidth() / 2.0f, DecoBoss::GetMovementMinYBound() + 0.75f * this->GetCurrentHeight());
}

inline Point2D DecoBoss::GetBossPositionForLevelRotation() {
    return Point2D(
        DecoBoss::GetMinXOfConfines() + ((DecoBoss::GetMaxXOfConfines() - DecoBoss::GetMinXOfConfines()) / 2.0f),
        DecoBoss::ROTATION_Y_POSITION);
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
/**
 * DecoBoss.h
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
    
    friend Boss* Boss::BuildStyleBoss(GameModel* gameModel, const GameWorld::WorldStyle& style);
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

    static const float LIGHTNING_RELAY_GLOW_SIZE;

    static const float ARM_WIDTH;
    static const float ARM_NOT_EXTENDED_HEIGHT;

    static const float ARM_POS_X_OFFSET;
    static const float ARM_Y_OFFSET;
    static const float ARM_Z_OFFSET;

    static const float ARM_SCOPING_SEG1_Y_RETRACT_TRANSLATION;
    static const float ARM_SCOPING_SEG2_Y_RETRACT_TRANSLATION;
    static const float ARM_SCOPING_SEG3_Y_RETRACT_TRANSLATION;
    static const float ARM_SCOPING_SEG4_Y_RETRACT_TRANSLATION;

    static const float ARM_ORIGIN_TO_HAND_END;
    static const float HAND_WIDTH;

    static const float SIDE_POS_X_OFFSET;
    static const float SIDE_Y_OFFSET;
    static const float SIDE_Z_OFFSET;

    static const float LIGHTNING_FIRE_POS_X;
    static const float LIGHTNING_FIRE_POS_Y;
    static const float LIGHTNING_FIRE_POS_Z;

    static const float ITEM_LOAD_OFFSET_Y;

    static const int NUM_EYES = 6;
    static const float EYE1_OFFSET_X;
    static const float EYE2_OFFSET_X;
    static const float EYE1_2_OFFSET_Y;
    static const float EYE3_OFFSET_Y;
    static const float EYE4_OFFSET_Y;
    static const float EYE5_OFFSET_Y;
    static const float EYE6_OFFSET_Y;

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

    Point3D GetLeftLightningRelayOffset() const;
    Point3D GetRightLightningRelayOffset() const;

    static Vector3D GetLightningFireVec3D() { return Vector3D(LIGHTNING_FIRE_POS_X, LIGHTNING_FIRE_POS_Y, LIGHTNING_FIRE_POS_Z); }
    static Vector2D GetLightningFireVec2D() { return Vector2D(LIGHTNING_FIRE_POS_X, LIGHTNING_FIRE_POS_Y); }

    bool IsRightBodyStillAlive() const;
    bool IsLeftBodyStillAlive() const;

    Point3D GetEyeClusterCenterPosition() const;
    const Vector2D& GetEyeOffset(int idx) const;

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

    static const float ATTACK_PADDLE_WITH_BOTH_ARMS_MIN_Y_POS;
    static const float ATTACK_PADDLE_WITH_BOTH_ARMS_MAX_Y_POS;
    static const float ATTACK_PADDLE_WITH_ONE_ARM_MIN_Y_POS;
    static const float ATTACK_PADDLE_WITH_ONE_ARM_MAX_Y_POS;

    size_t middleBodyIdx, leftBodyIdx, rightBodyIdx;
    size_t coreIdx, lightningRelayIdx;
    size_t leftArmIdx, leftArmGearIdx, leftArmScopingSeg1Idx, leftArmScopingSeg2Idx, leftArmScopingSeg3Idx, leftArmScopingSeg4Idx, leftArmHandIdx;
    size_t rightArmIdx, rightArmGearIdx, rightArmScopingSeg1Idx, rightArmScopingSeg2Idx, rightArmScopingSeg3Idx, rightArmScopingSeg4Idx, rightArmHandIdx;

    std::vector<Vector2D> eyeOffsets;

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
    float GetYBallMaxForLevelRotate() const;
    Point2D GetFarLeftDropStatePosition() const;
    Point2D GetFarRightDropStatePosition() const;

    static float GetMinXOfConfines() { return DecoBoss::CONFINE_X_PADDING_NUM_PIECES * LevelPiece::PIECE_WIDTH; }
    static float GetMaxXOfConfines() { return (DecoBoss::LEVEL_WIDTH_NUM_PIECES - DecoBoss::CONFINE_X_PADDING_NUM_PIECES) * LevelPiece::PIECE_WIDTH; }
    static float GetMaxYOfConfines() { return (DecoBoss::LEVEL_HEIGHT_NUM_PIECES - DecoBoss::CONFINE_Y_TOP_PADDING_NUM_PIECES) * LevelPiece::PIECE_HEIGHT; }

    static float GetMovementMinXBound() { return DecoBoss::GetMinXOfConfines() + DecoBoss::MOVEMENT_PADDING_X; }
    static float GetMovementMaxXBound() { return DecoBoss::GetMaxXOfConfines() - DecoBoss::MOVEMENT_PADDING_X; }
    static float GetMovementMinYBound() { return DecoBoss::MOVEMENT_MIN_Y_BOUNDARY; }
    static float GetMovementMaxYBound() { return DecoBoss::GetMaxYOfConfines() - DecoBoss::MOVEMENT_PADDING_Y; }

    float GetRandomAttackPaddleYPos() const;
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

inline Point3D DecoBoss::GetLeftLightningRelayOffset() const {
    return Point3D(-1.049f, -6.427f, 0.975f);
}

inline Point3D DecoBoss::GetRightLightningRelayOffset() const {
    return Point3D(1.049f, -6.427f, 0.975f);
}

inline bool DecoBoss::IsRightBodyStillAlive() const {
    return !this->GetRightBody()->GetIsDestroyed();
}
inline bool DecoBoss::IsLeftBodyStillAlive() const {
    return !this->GetLeftBody()->GetIsDestroyed();
}

inline Point3D DecoBoss::GetEyeClusterCenterPosition() const {
    return this->GetCore()->GetTranslationPt3D() + Vector3D(0.0f, 2.844f, 1.2f);
}

inline const Vector2D& DecoBoss::GetEyeOffset(int idx) const {
    return this->eyeOffsets[idx];
}

inline float DecoBoss::GetSideToSideDropStateDistance() const {
    return DecoBoss::GetMaxXOfConfines() - DecoBoss::GetMinXOfConfines() - this->GetCurrentWidth();
}

inline float DecoBoss::GetYBallMaxForLevelRotate() const {
    return (DecoBoss::ROTATION_Y_POSITION - 1.0f);
}

inline Point2D DecoBoss::GetFarLeftDropStatePosition() const {
    return Point2D(DecoBoss::GetMovementMinXBound() + this->GetCurrentWidth() / 2.0f, DecoBoss::GetMovementMinYBound() + 0.75f * this->GetCurrentHeight());
}
inline Point2D DecoBoss::GetFarRightDropStatePosition() const {
    return Point2D(DecoBoss::GetMovementMaxXBound() - this->GetCurrentWidth() / 2.0f, DecoBoss::GetMovementMinYBound() + 0.75f * this->GetCurrentHeight());
}

inline Point2D DecoBoss::GetBossPositionForLevelRotation() {
    return Point2D(
        DecoBoss::GetMinXOfConfines() + ((DecoBoss::GetMaxXOfConfines() - DecoBoss::GetMinXOfConfines()) / 2.0f), DecoBoss::ROTATION_Y_POSITION);
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
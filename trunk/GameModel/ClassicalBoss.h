/**
 * ClassicalBoss.h
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

#ifndef __CLASSICALBOSS_H__
#define __CLASSICALBOSS_H__

#include "Boss.h"

namespace classicalbossai {
class ClassicalBossAI;
class ArmsBodyHeadAI;
class BodyHeadAI;
class HeadAI;
}

class BossBodyPart;
class BossWeakpoint;
class BossCompositeBodyPart;

/**
 * Child class of Boss that represents the Classical movement's boss ("Pediment Head").
 * This class contains all of the setup and constants and particulars for the
 * Classical movement boss.
 */
class ClassicalBoss : public Boss {

    friend class classicalbossai::ClassicalBossAI;
    friend class classicalbossai::ArmsBodyHeadAI;
    friend class classicalbossai::BodyHeadAI;
    friend class classicalbossai::HeadAI;
    friend Boss* Boss::BuildStyleBoss(GameModel* gameModel, const GameWorld::WorldStyle& style);

public:
    static const float ARM_WIDTH;
    static const float HALF_ARM_WIDTH;
    static const float ARM_HEIGHT;
    static const float HALF_ARM_HEIGHT;
    static const float COLUMN_WIDTH;
    static const float COLUMN_HEIGHT;
    static const float BASE_WIDTH;
    static const float BASE_HEIGHT;
    static const float TABLATURE_WIDTH;
    static const float TABLATURE_HEIGHT;
    static const float PEDIMENT_WIDTH;
    static const float PEDIMENT_HEIGHT;
    static const float EYE_WIDTH;
    static const float EYE_HEIGHT;
    static const float EYE_DEPTH;

    ~ClassicalBoss();

    bool ProjectilePassesThrough(const Projectile*) const { return true; } // This boss is not affected by projectiles

    const BossBodyPart* GetEye() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->eyeIdx]); }
    const BossBodyPart* GetPediment() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->pedimentIdx]); }
    const BossBodyPart* GetBase() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->baseIdx]); }

    const BossBodyPart* GetTopLeftTablature() const     { return static_cast<const BossBodyPart*>(this->bodyParts[this->topLeftTablatureIdx]); }
    const BossBodyPart* GetTopRightTablature() const    { return static_cast<const BossBodyPart*>(this->bodyParts[this->topRightTablatureIdx]); }
    const BossBodyPart* GetBottomLeftTablature() const  { return static_cast<const BossBodyPart*>(this->bodyParts[this->bottomLeftTablatureIdx]); }
    const BossBodyPart* GetBottomRightTablature() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->bottomRightTablatureIdx]); }

    const BossBodyPart* GetLeftRestOfArm() const  { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftRestOfArmIdx]); }
    const BossBodyPart* GetRightRestOfArm() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightRestOfArmIdx]); }
    const BossBodyPart* GetLeftArmSquare() const  { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftArmSquareIdx]); }
    const BossBodyPart* GetRightArmSquare() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightArmSquareIdx]); }

    // Returns columns from left to right
    std::vector<const BossBodyPart*> GetBodyColumns() const;

private:
    size_t eyeIdx;
    size_t pedimentIdx;
    size_t baseIdx;
    size_t leftArmIdx, leftRestOfArmIdx, leftArmSquareIdx;
    size_t rightArmIdx, rightRestOfArmIdx, rightArmSquareIdx;
    size_t topLeftTablatureIdx, topRightTablatureIdx, bottomLeftTablatureIdx, bottomRightTablatureIdx;
    size_t leftCol1Idx, leftCol2Idx, leftCol3Idx, rightCol1Idx, rightCol2Idx, rightCol3Idx;

    static const float ARM_X_TRANSLATION_FROM_CENTER;
    static const float ARM_X_TRANSLATION_TO_INNER_EDGE_FROM_CENTER;

    static const float ARMS_BODY_HEAD_MAX_SPEED;
    static const float ARMS_BODY_HEAD_ACCELERATION;

    static const float BODY_HEAD_MAX_SPEED;
    static const float BODY_HEAD_ACCELERATION;

    static const float PEDIMENT_MAX_SPEED;
    static const float PEDIMENT_ACCELERATION;
    
    static const float EYE_MAX_SPEED;
    static const float EYE_ACCELERATION;

    ClassicalBoss();

    void BuildArm(bool isLeftArm, const Vector3D& armTranslation, size_t& armIdx, size_t& restOfArmIdx, size_t& squareIdx);
    
    void GenerateFullColumnPedimentBounds();
    void GenerateEyeBounds();

    void BuildEyeBounds(BoundingLines& eyeBounds);
    void BuildColumnBounds(BoundingLines& columnBounds);
    
    // Inherited from Boss
    void Init(float startingX, float startingY);

    DISALLOW_COPY_AND_ASSIGN(ClassicalBoss);
};

#endif // __CLASSICALBOSS_H__
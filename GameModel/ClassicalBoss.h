/**
 * ClassicalBoss.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CLASSICALBOSS_H__
#define __CLASSICALBOSS_H__

#include "Boss.h"

namespace classicalbossai {
class ArmsBodyHeadAI;
}

class BossBodyPart;
class BossWeakpoint;
class BossCompositeBodyPart;

class ClassicalBoss : public Boss {

    friend class classicalbossai::ArmsBodyHeadAI;
    friend Boss* Boss::BuildStyleBoss(const GameWorld::WorldStyle& style);

public:
    ~ClassicalBoss();

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

    // Inherited from Boss
    bool GetIsDead() const;

private:
    size_t eyeIdx;
    size_t pedimentIdx;
    size_t baseIdx;
    size_t leftArmIdx, leftRestOfArmIdx, leftArmSquareIdx;
    size_t rightArmIdx, rightRestOfArmIdx, rightArmSquareIdx;
    size_t topLeftTablatureIdx, topRightTablatureIdx, bottomLeftTablatureIdx, bottomRightTablatureIdx;
    size_t leftCol1Idx, leftCol2Idx, leftCol3Idx, rightCol1Idx, rightCol2Idx, rightCol3Idx;

    static const float ARM_X_TRANSLATION_FROM_CENTER;
    static const float ARM_WIDTH;
    static const float HALF_ARM_WIDTH;
    static const float ARM_HEIGHT;

    static const float ARMS_BODY_HEAD_MAX_SPEED;
    static const float ARMS_BODY_HEAD_ACCELERATION;

    static const float BODY_HEAD_MAX_SPEED;
    static const float BODY_HEAD_ACCELERATION;

    static const float PEDIMENT_MAX_SPEED;
    static const float PEDIMENT_ACCELERATION;
    
    static const float EYE_MAX_SPEED;
    static const float EYE_ACCELERATION;

    ClassicalBoss();
    void BuildArm(const Vector3D& armTranslation, size_t& armIdx, size_t& restOfArmIdx, size_t& squareIdx);
    
    void ConvertAliveBodyPartToWeakpoint(size_t index, float lifePoints, float ballDmgOnHit);
    void ConvertAliveBodyPartToDeadBodyPart(size_t index);

    // Inherited from Boss
    void Init();

    DISALLOW_COPY_AND_ASSIGN(ClassicalBoss);
};

#endif // __CLASSICALBOSS_H__
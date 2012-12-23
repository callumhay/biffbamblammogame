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

class BossBodyPart;
class BossWeakpoint;
class BossCompositeBodyPart;

class ClassicalBoss : public Boss {
    friend Boss* Boss::BuildStyleBoss(const GameWorld::WorldStyle& style);

public:
    ~ClassicalBoss();

    // Inherited from Boss
    bool GetIsDead() const;
	void CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart);
	void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);
    void CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart);

private:
    BossCompositeBodyPart* deadPartsRoot;
    BossCompositeBodyPart* alivePartsRoot;

    size_t leftArmIdx, leftArmSquareIdx;
    size_t rightArmIdx, rightArmSquareIdx;

    /*
    // Important body parts of the boss based on state...
    
    // ArmsBodyHead State
    BossWeakpoint* leftArmSquareWeakpoint;
    BossWeakpoint* rightArmSquareWeakpoint;
    BossCompositeBodyPart* leftArm;
    BossCompositeBodyPart* rightArm;

    // BodyHead State
    std::vector<BossWeakpoint*> columnWeakpoints;
    // Head State
    BossWeakpoint* eyeWeakpoint;
    */

    ClassicalBoss();
    void BuildArm(const Vector3D& armTranslation, size_t& armIdx, size_t& squareIdx);

    // Inherited from Boss
    void Init();

    DISALLOW_COPY_AND_ASSIGN(ClassicalBoss);
};

#endif // __CLASSICALBOSS_H__
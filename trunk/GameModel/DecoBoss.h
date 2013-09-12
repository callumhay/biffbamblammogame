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

#include "Boss.h"

class DecoBoss : public Boss {
    
    friend Boss* Boss::BuildStyleBoss(const GameWorld::WorldStyle& style);

public:
    static const float CORE_HEIGHT;
    static const float HALF_CORE_HEIGHT;

    static const float ARM_POS_X_OFFSET;
    static const float ARM_Y_OFFSET;
    static const float ARM_Z_OFFSET;

    ~DecoBoss();

    const BossBodyPart* GetCore() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->coreIdx]); }


    // Inherited from Boss
    bool ProjectilePassesThrough(const Projectile* projectile) const;
    //bool ProjectileIsDestroyedOnCollision(const Projectile* projectile, BossBodyPart* collisionPart) const;

private:
    size_t middleBodyIdx, leftBodyIdx, rightBodyIdx;
    size_t coreIdx, lightningRelayIdx;
    size_t leftArmIdx, leftArmGearIdx, leftArmScopingSeg1Idx, leftArmScopingSeg2Idx, leftArmHandIdx;
    size_t rightArmIdx, rightArmGearIdx, rightArmScopingSeg1Idx, rightArmScopingSeg2Idx, rightArmHandIdx;

    DecoBoss();

    // Inherited from Boss
    void Init(float startingX, float startingY);


    BossCompositeBodyPart* BuildArm(BossCompositeBodyPart* middleBody, size_t& armIdx, 
        size_t& gearIdx, size_t& scoping1Idx, size_t& scoping2Idx, size_t& handIdx);

    DISALLOW_COPY_AND_ASSIGN(DecoBoss);
};

#endif // __DECOBOSS_H__
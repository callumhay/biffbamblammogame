/**
 * NouveauBoss.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __NOUVEAUBOSS_H__
#define __NOUVEAUBOSS_H__

class BossBodyPart;
class BossWeakpoint;
class BossCompositeBodyPart;

namespace nouveaubossai {
class NouveauBossAI;
class SideSphereAI;
};

#include "Boss.h"
#include "../BlammoEngine/Collision.h"

class NouveauBoss : public Boss {

    friend Boss* Boss::BuildStyleBoss(const GameWorld::WorldStyle& style);
    friend class nouveaubossai::NouveauBossAI;
    friend class nouveaubossai::SideSphereAI;

public:
    static const int NUM_CURLS = 8;

    static const float BODY_CORE_HEIGHT;
    static const float BODY_CORE_TOP_WIDTH;
    static const float BODY_CORE_BOTTOM_WIDTH;

    static const float TOP_ENCLOSURE_GAZEBO_HEIGHT;
    static const float TOP_ENCLOSURE_GAZEBO_WIDTH;
    static const float TOP_ENCLOSURE_DOME_HEIGHT;
    static const float TOP_ENCLOSURE_DOME_TOP_WIDTH;

    static const float ARM_SPHERE_HOLDER_CURL_WIDTH;

    static const float DEFAULT_ACCELERATION;

    static const float MIN_X_BOUNDS_WITH_PADDING;
    static const float MAX_X_BOUNDS_WITH_PADDING;
    static const float MIN_Y_BOUNDS_WITH_PADDING;
    static const float MAX_Y_BOUNDS_WITH_PADDING;

    ~NouveauBoss();


    const BossBodyPart* GetBody() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->bodyIdx]); }

    Collision::AABB2D GetMovementAABB() const;

    // Inherited from Boss
    bool ProjectilePassesThrough(const Projectile* projectile) const;

private:
    size_t bodyIdx, gazeboIdx, domeIdx, topSphereIdx, leftSideCurlsIdx, rightSideCurlsIdx;
    size_t leftSideSphereHolderIdx, leftSideHolderCurlIdx, leftSideSphereIdx, leftSideSphereFrillsIdx;
    size_t rightSideSphereHolderIdx, rightSideHolderCurlIdx, rightSideSphereIdx, rightSideSphereFrillsIdx;
    size_t bottomHexSphereIdx;
    size_t bottomCurlIdxs[NUM_CURLS];

    NouveauBoss();

    // Inherited from Boss
    void Init();
    

    void BuildBottomCurl(const Vector3D& translation, float yRotation, size_t& idx);

    static BoundingLines BuildSemiCircleBoundingLines(float height, float lowerMidHeight, float upperMidHeight,
        float halfBottomWidth, float halfLowerMidWidth, float halfUpperMidWidth, float halfTopWidth);

    DISALLOW_COPY_AND_ASSIGN(NouveauBoss);
};

inline Collision::AABB2D NouveauBoss::GetMovementAABB() const {
    Collision::AABB2D bossAABB = this->alivePartsRoot->GenerateWorldAABB();
    float halfWidth  = bossAABB.GetWidth()/2.0f;
    float halfHeight = bossAABB.GetHeight()/2.0f;
    return Collision::AABB2D(
        Point2D(MIN_X_BOUNDS_WITH_PADDING + halfWidth, MIN_Y_BOUNDS_WITH_PADDING + halfHeight),
        Point2D(MAX_X_BOUNDS_WITH_PADDING - halfWidth, MAX_Y_BOUNDS_WITH_PADDING - halfHeight));
}

#endif // __NOUVEAUBOSS_H__
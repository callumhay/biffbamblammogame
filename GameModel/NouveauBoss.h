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
#include "TriangleBlocks.h"
#include "GameLevel.h"

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
    static const float BODY_CORE_MID_HALF_DEPTH;

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
    const BossBodyPart* GetLeftSideCurls() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftSideCurlsIdx]); }
    const BossBodyPart* GetRightSideCurls() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightSideCurlsIdx]); }

    const BossBodyPart* GetLeftSideHolderCurl() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftSideHolderCurlIdx]); }
    const BossBodyPart* GetLeftSideSphere() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftSideSphereIdx]); }
    const BossBodyPart* GetLeftSideSphereFrills() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftSideSphereFrillsIdx]); }

    const BossBodyPart* GetRightSideHolderCurl() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightSideHolderCurlIdx]); }
    const BossBodyPart* GetRightSideSphere() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightSideSphereIdx]); }
    const BossBodyPart* GetRightSideSphereFrills() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightSideSphereFrillsIdx]); }

    const BossBodyPart* GetBottomHexSphere() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->bottomHexSphereIdx]); }
    const BossBodyPart* GetBottomCurl(int idx) const { return static_cast<const BossBodyPart*>(this->bodyParts[this->bottomCurlIdxs[idx]]); }

    const BossBodyPart* GetTopSphere() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->topSphereIdx]); }
    const BossBodyPart* GetTopGazebo() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->gazeboIdx]); }
    const BossBodyPart* GetTopDome() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->domeIdx]); }

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

    // Level attributes made use of by the boss AI
    static const int LEVEL_NUM_PIECES_WIDTH    = 23;
    static const int LEVEL_NUM_PIECES_HEIGHT   = 32;
    static const int MIN_X_CONFINES_NUM_PIECES = 5;
    static const int MAX_X_CONFINES_NUM_PIECES = 19;
    static float GetMinXOfConfines() { return MIN_X_CONFINES_NUM_PIECES * LevelPiece::PIECE_WIDTH; }
    static float GetMaxXOfConfines() { return (MAX_X_CONFINES_NUM_PIECES - 1) * LevelPiece::PIECE_WIDTH; }

    static const int NUM_SIDE_PRISMS = 17;
    static const std::pair<int,int> SIDE_PRISM_IDX_PAIRS[NUM_SIDE_PRISMS]; // Stores the row,col of each of the side prisms in the level
    static PrismTriangleBlock* GetSidePrism(const GameLevel& level, int idx) {
        assert(idx >= 0 && idx < NUM_SIDE_PRISMS);
        assert(LEVEL_NUM_PIECES_HEIGHT == level.GetCurrentLevelLayout().size());
        assert(LEVEL_NUM_PIECES_WIDTH  == level.GetCurrentLevelLayout()[0].size());

        const std::pair<int, int>& prismIdxPair = SIDE_PRISM_IDX_PAIRS[idx];
        LevelPiece* piece = level.GetCurrentLevelLayout()[prismIdxPair.first][prismIdxPair.second];
        
        assert(piece != NULL);
        assert(piece->GetType() == LevelPiece::PrismTriangle);
        
        return static_cast<PrismTriangleBlock*>(piece);
    }


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
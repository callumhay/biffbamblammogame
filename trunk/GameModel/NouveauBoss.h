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
class GlassDomeAI;
class TopSphereAI;
};

#include "Boss.h"
#include "TriangleBlocks.h"
#include "GameLevel.h"

#include "../BlammoEngine/Collision.h"

class NouveauBoss : public Boss {

    friend Boss* Boss::BuildStyleBoss(const GameWorld::WorldStyle& style);
    friend class nouveaubossai::NouveauBossAI;
    friend class nouveaubossai::SideSphereAI;
    friend class nouveaubossai::GlassDomeAI;
    friend class nouveaubossai::TopSphereAI;

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
    static const float TOP_ENCLOSURE_DOME_BOTTOM_WIDTH;
    static const float TOP_SPHERE_WIDTH;
    static const float TOP_SPHERE_HEIGHT;

    static const float ARM_SPHERE_HOLDER_CURL_WIDTH;
    static const float ARM_SPHERE_HOLDER_CURL_HEIGHT;

    static const float BOTTOM_SPHERE_LASER_BEAM_RADIUS;
    static const float SIDE_CURL_TOP_LASER_BEAM_RADIUS;
    static const float SIDE_CURL_BOTTOM_LASER_BEAM_RADIUS;

    static const float MIN_X_BOUNDS_WITH_PADDING;
    static const float MAX_X_BOUNDS_WITH_PADDING;
    static const float MIN_Y_BOUNDS_WITH_PADDING;
    static const float MAX_Y_BOUNDS_WITH_PADDING;

    static const float LEFT_SIDE_CURL_SHOOT1_OFFSET_X;
    static const float LEFT_SIDE_CURL_SHOOT1_OFFSET_Y;
    static const float LEFT_SIDE_CURL_SHOOT2_OFFSET_X;
    static const float LEFT_SIDE_CURL_SHOOT2_OFFSET_Y;

    static const float RIGHT_SIDE_CURL_SHOOT1_OFFSET_X;
    static const float RIGHT_SIDE_CURL_SHOOT1_OFFSET_Y;
    static const float RIGHT_SIDE_CURL_SHOOT2_OFFSET_X;
    static const float RIGHT_SIDE_CURL_SHOOT2_OFFSET_Y;

    static const float SIDE_CURL_SHOOT_OFFSET_Z;
    static const float BOTTOM_SPHERE_SHOOT_OFFSET_Y;

    static const float ARM_CURL_CENTER_X_OFFSET;
    static const float ARM_CURL_CENTER_Y_OFFSET;

    static const float TOP_SPHERE_CENTER_Y_OFFSET;

    ~NouveauBoss();

    Vector3D GetLeftSideCurlShootTopPtOffset() const;
    Vector3D GetLeftSideCurlShootBottomPtOffset() const;
    Vector3D GetRightSideCurlShootTopPtOffset() const;
    Vector3D GetRightSideCurlShootBottomPtOffset() const;
    Vector3D GetBottomSphereBeamShootPtOffset() const;

    Point3D GetLeftSideCurlShootTopPt() const;
    Point3D GetLeftSideCurlShootBottomPt() const;
    Point3D GetRightSideCurlShootTopPt() const;
    Point3D GetRightSideCurlShootBottomPt() const;
    Point3D GetBottomSphereShootPt() const;
    Point3D GetBottomSphereBeamShootPt() const;

    Point2D GetLeftSideCurlHolderExplosionCenter() const;
    Point2D GetRightSideCurlHolderExplosionCenter() const;
    Point2D GetTopDomeExplosionCenter() const;

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
    bool ProjectileIsDestroyedOnCollision(const Projectile* projectile, BossBodyPart* collisionPart) const;

private:
    size_t bodyIdx, gazeboIdx, domeIdx, topSphereIdx, leftSideCurlsIdx, rightSideCurlsIdx;
    size_t leftSideSphereHolderIdx, leftSideHolderCurlIdx, leftSideSphereIdx, leftSideSphereFrillsIdx;
    size_t rightSideSphereHolderIdx, rightSideHolderCurlIdx, rightSideSphereIdx, rightSideSphereFrillsIdx;
    size_t bottomHexSphereIdx;
    size_t bottomCurlIdxs[NUM_CURLS];

    NouveauBoss();

    // Level attributes made use of by the boss AI
    static const int LEVEL_NUM_PIECES_WIDTH    = 25;
    static const int LEVEL_NUM_PIECES_HEIGHT   = 35;
    static const int MIN_X_CONFINES_NUM_PIECES = 6;
    static const int MAX_X_CONFINES_NUM_PIECES = 20;
    static float GetMinXOfConfines() { return MIN_X_CONFINES_NUM_PIECES * LevelPiece::PIECE_WIDTH; }
    static float GetMaxXOfConfines() { return (MAX_X_CONFINES_NUM_PIECES - 1) * LevelPiece::PIECE_WIDTH; }
    static float GetReasonableYForBigAttack() { return MIN_Y_BOUNDS_WITH_PADDING + (MAX_Y_BOUNDS_WITH_PADDING - MIN_Y_BOUNDS_WITH_PADDING) / 2.0f; }
    static float GetLevelMidX() { return LEVEL_NUM_PIECES_WIDTH * LevelPiece::PIECE_WIDTH / 2.0f; }

    static const int NUM_LEFT_SIDE_PRISMS  = 4;
    static const int NUM_RIGHT_SIDE_PRISMS = 4;
    static const int NUM_SIDE_PRISMS = NUM_LEFT_SIDE_PRISMS + NUM_RIGHT_SIDE_PRISMS;
    static const std::pair<int,int> SIDE_PRISM_IDX_PAIRS[NUM_SIDE_PRISMS]; // Stores the row,col of each of the side prisms in the level
    static const std::pair<int,int> LEFT_SIDE_PRISM_IDX_PAIRS[NUM_LEFT_SIDE_PRISMS];
    static const std::pair<int,int> RIGHT_SIDE_PRISM_IDX_PAIRS[NUM_RIGHT_SIDE_PRISMS];
    static PrismTriangleBlock* GetSidePrism(const GameLevel& level, int idx);
    static PrismTriangleBlock* GetLeftSidePrism(const GameLevel& level, int idx);
    static PrismTriangleBlock* GetRightSidePrism(const GameLevel& level, int idx);
    static std::vector<PrismTriangleBlock*> GetBestSidePrismCandidates(const GameLevel& level, 
        const PlayerPaddle& paddle, const Point2D& shotOrigin, bool leftSide, bool rightSide);

    static PrismBlock* GetLeftSplitterPrism(const GameLevel& level);
    static PrismBlock* GetRightSplitterPrism(const GameLevel& level);
    
    static const float Y_POS_FOR_LASER_BEAMS1;
    static const float Y_POS_FOR_LASER_BEAMS2;

    // Inherited from Boss
    void Init(float startingX, float startingY);
   
    void BuildBottomCurl(const Vector3D& translation, float yRotation, size_t& idx);

    static BoundingLines BuildSemiCircleBoundingLines(float height, float lowerMidHeight, float upperMidHeight,
        float halfBottomWidth, float halfLowerMidWidth, float halfUpperMidWidth, float halfTopWidth, bool doInsideOnSides);

    DISALLOW_COPY_AND_ASSIGN(NouveauBoss);
};

inline Vector3D NouveauBoss::GetLeftSideCurlShootTopPtOffset() const {
    return Vector3D(LEFT_SIDE_CURL_SHOOT1_OFFSET_X, LEFT_SIDE_CURL_SHOOT1_OFFSET_Y, SIDE_CURL_SHOOT_OFFSET_Z);
}
inline Vector3D NouveauBoss::GetLeftSideCurlShootBottomPtOffset() const {
    return Vector3D(LEFT_SIDE_CURL_SHOOT2_OFFSET_X, LEFT_SIDE_CURL_SHOOT2_OFFSET_Y, SIDE_CURL_SHOOT_OFFSET_Z);
}
inline Vector3D NouveauBoss::GetRightSideCurlShootTopPtOffset() const {
    return Vector3D(RIGHT_SIDE_CURL_SHOOT1_OFFSET_X, RIGHT_SIDE_CURL_SHOOT1_OFFSET_Y, SIDE_CURL_SHOOT_OFFSET_Z);
}
inline Vector3D NouveauBoss::GetRightSideCurlShootBottomPtOffset() const {
    return Vector3D(RIGHT_SIDE_CURL_SHOOT2_OFFSET_X, RIGHT_SIDE_CURL_SHOOT2_OFFSET_Y, SIDE_CURL_SHOOT_OFFSET_Z);
}
inline Vector3D NouveauBoss::GetBottomSphereBeamShootPtOffset() const {
    return Vector3D(0, BOTTOM_SPHERE_SHOOT_OFFSET_Y + 1.0f, 0.0f);
}

inline Point3D NouveauBoss::GetLeftSideCurlShootTopPt() const {
    return this->GetLeftSideCurls()->GetTranslationPt3D() + this->GetLeftSideCurlShootTopPtOffset();
}

inline Point3D NouveauBoss::GetLeftSideCurlShootBottomPt() const {
    return this->GetLeftSideCurls()->GetTranslationPt3D() + this->GetLeftSideCurlShootBottomPtOffset();
}

inline Point3D NouveauBoss::GetRightSideCurlShootTopPt() const {
    return this->GetRightSideCurls()->GetTranslationPt3D() + this->GetRightSideCurlShootTopPtOffset();
}

inline Point3D NouveauBoss::GetRightSideCurlShootBottomPt() const {
    return this->GetRightSideCurls()->GetTranslationPt3D() + this->GetRightSideCurlShootBottomPtOffset();
}

inline Point3D NouveauBoss::GetBottomSphereShootPt() const {
    Point3D pt = this->GetBottomHexSphere()->GetTranslationPt3D();
    pt[1] += BOTTOM_SPHERE_SHOOT_OFFSET_Y;
    return pt;
}

inline Point3D NouveauBoss::GetBottomSphereBeamShootPt() const {
    Point3D pt = this->GetBottomHexSphere()->GetTranslationPt3D();
    pt[1] += BOTTOM_SPHERE_SHOOT_OFFSET_Y + 1.0f;
    return pt;
}

inline Point2D NouveauBoss::GetLeftSideCurlHolderExplosionCenter() const {
    Point2D pt(-ARM_CURL_CENTER_X_OFFSET, ARM_CURL_CENTER_Y_OFFSET);
    return this->GetLeftSideHolderCurl()->GetWorldTransform() * pt;
}

inline Point2D NouveauBoss::GetRightSideCurlHolderExplosionCenter() const {
    Point2D pt(-ARM_CURL_CENTER_X_OFFSET, ARM_CURL_CENTER_Y_OFFSET);
    return this->GetRightSideHolderCurl()->GetWorldTransform() * pt;
}

inline Point2D NouveauBoss::GetTopDomeExplosionCenter() const {
    Point2D pt(0, TOP_SPHERE_CENTER_Y_OFFSET);
    return this->GetTopDome()->GetWorldTransform() * pt;
}

inline Collision::AABB2D NouveauBoss::GetMovementAABB() const {
    Collision::AABB2D bossAABB = this->alivePartsRoot->GenerateWorldAABB();
    float halfWidth  = bossAABB.GetWidth()/2.0f;
    float halfHeight = bossAABB.GetHeight()/2.0f;
    return Collision::AABB2D(
        Point2D(MIN_X_BOUNDS_WITH_PADDING + halfWidth, MIN_Y_BOUNDS_WITH_PADDING + halfHeight),
        Point2D(MAX_X_BOUNDS_WITH_PADDING - halfWidth, MAX_Y_BOUNDS_WITH_PADDING - halfHeight));
}

#endif // __NOUVEAUBOSS_H__
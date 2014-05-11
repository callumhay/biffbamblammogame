/**
 * FuturismBoss.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#ifndef __FUTURISMBOSS_H__
#define __FUTURISMBOSS_H__

class BossBodyPart;
class BossWeakpoint;
class BossCompositeBodyPart;


class FuturismBossAIState;
class FuturismBossStage1AIState;
class FuturismBossStage2AIState;
class FuturismBossStage3AIState;

#include "Boss.h"
#include "GameLevel.h"

class FuturismBoss : public Boss {

    friend Boss* Boss::BuildStyleBoss(GameModel* gameModel, const GameWorld::WorldStyle& style);
    friend class FuturismBossAIState;
    friend class FuturismBossStage1AIState;
    friend class FuturismBossStage2AIState;
    friend class FuturismBossStage3AIState;

public:
    static const float FULLY_SHIELDED_BOSS_HEIGHT;
    static const float FULLY_SHIELDED_BOSS_HALF_HEIGHT;
    static const float FULLY_SHIELDED_BOSS_WIDTH;
    static const float FULLY_SHIELDED_BOSS_HALF_WIDTH;

    static const float CORE_BOSS_SIZE;
    static const float CORE_BOSS_HALF_SIZE;
    static const float CORE_EYE_SIZE;
    static const float CORE_EYE_HALF_SIZE;
    static const float CORE_SHIELD_SIZE;
    static const float CORE_BULB_SIZE;
    static const float CORE_BULB_HALF_SIZE;
    static const float CORE_BULB_Z_DIST;
    static const float CORE_BULB_OFFSET_FROM_ORIGIN;
    static const float CORE_ARMS_MOVE_FORWARD_AMT;

    static const float CORE_Z_DIST_FROM_ORIGIN;
    static const float CORE_Z_SHOOT_DIST_FROM_ORIGIN_WITHOUT_SHIELD;
    static const float CORE_Z_SHOOT_DIST_FROM_ORIGIN_WITH_SHIELD;

    static const float TOP_BOTTOM_SHIELD_EXPLODE_WIDTH;
    static const float TOP_BOTTOM_SHIELD_EXPLODE_HEIGHT;
    static const float LEFT_RIGHT_SHIELD_EXPLODE_WIDTH;
    static const float LEFT_RIGHT_SHIELD_EXPLODE_HEIGHT;

    static const float TOP_BOTTOM_SHIELD_X_OFFSET;
    static const float TOP_BOTTOM_SHIELD_Y_OFFSET;
    static const float LEFT_RIGHT_SHIELD_X_OFFSET;
    static const float LEFT_RIGHT_SHIELD_Y_OFFSET;

    ~FuturismBoss();

    float GetCurrentZShootDistFromOrigin() const;
    float GetCurrentBeamZShootDistFromOrigin() const;

    // Get the various shields of the Boss
    const BossBodyPart* GetCoreShield() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->coreShieldIdx]); }
    const BossBodyPart* GetTopShield() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->topShieldIdx]); }
    const BossBodyPart* GetBottomShield() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->bottomShieldIdx]); }
    const BossBodyPart* GetLeftShield() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftShieldIdx]); }
    const BossBodyPart* GetRightShield() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightShieldIdx]); }

    // Get the core parts of the boss
    const BossCompositeBodyPart* GetCoreAssembly() const { return static_cast<const BossCompositeBodyPart*>(this->bodyParts[this->coreAssemblyIdx]); }
    const BossBodyPart* GetCoreBody() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->coreBodyIdx]); }
    const BossBodyPart* GetCoreTopBulb() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->topBulbIdx]); }
    const BossBodyPart* GetCoreBottomBulb() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->bottomBulbIdx]); }
    const BossBodyPart* GetCoreLeftBulb() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftBulbIdx]); }
    const BossBodyPart* GetCoreRightBulb() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightBulbIdx]); }


    Point2D GetCoreTopBulbWorldPos() const { 
        Vector2D vec = GetCoreTopBulbLocalVec();
        return this->GetCoreAssembly()->GetWorldTransform() * Point2D(vec[0], vec[1]); 
    }
    Point2D GetCoreBottomBulbWorldPos() const { 
        Vector2D vec = GetCoreBottomBulbLocalVec();
        return this->GetCoreAssembly()->GetWorldTransform() * Point2D(vec[0], vec[1]); 
    }
    Point2D GetCoreLeftBulbWorldPos() const { 
        Vector2D vec = GetCoreLeftBulbLocalVec();
        return this->GetCoreAssembly()->GetWorldTransform() * Point2D(vec[0], vec[1]); 
    }
    Point2D GetCoreRightBulbWorldPos() const { 
        Vector2D vec = GetCoreRightBulbLocalVec();
        return this->GetCoreAssembly()->GetWorldTransform() * Point2D(vec[0], vec[1]); 
    }

    Vector2D GetCoreTopBulbWorldVec() const { 
        return this->GetCoreAssembly()->GetWorldTransform() * GetCoreTopBulbLocalVec(); 
    }
    Vector2D GetCoreBottomBulbWorldVec() const { 
        return this->GetCoreAssembly()->GetWorldTransform() * GetCoreBottomBulbLocalVec(); 
    }
    Vector2D GetCoreLeftBulbWorldVec() const { 
        return this->GetCoreAssembly()->GetWorldTransform() * GetCoreLeftBulbLocalVec(); 
    }
    Vector2D GetCoreRightBulbWorldVec() const { 
        return this->GetCoreAssembly()->GetWorldTransform() * GetCoreRightBulbLocalVec(); 
    }

    bool IsCoreShieldVulnerable() const;
    bool AreBulbsVulnerable() const;
    bool IsFrozen() const;
    
    bool AreAllBulbsDestroyed() const {
        return this->GetCoreTopBulb()->GetIsDestroyed() && this->GetCoreBottomBulb()->GetIsDestroyed() &&
            this->GetCoreLeftBulb()->GetIsDestroyed() && this->GetCoreRightBulb()->GetIsDestroyed();
    }

    bool IsCoreShieldWeakened() const   { return this->GetCoreShield()->GetType() == AbstractBossBodyPart::WeakpointBodyPart;   }
    bool IsTopShieldWeakened() const    { return this->GetTopShield()->GetType() == AbstractBossBodyPart::WeakpointBodyPart;    }
    bool IsBottomShieldWeakened() const { return this->GetBottomShield()->GetType() == AbstractBossBodyPart::WeakpointBodyPart; }
    bool IsLeftShieldWeakened() const   { return this->GetLeftShield()->GetType() == AbstractBossBodyPart::WeakpointBodyPart;   }
    bool IsRightShieldWeakened() const  { return this->GetRightShield()->GetType() == AbstractBossBodyPart::WeakpointBodyPart;  }
    
    float GetIceRotationInDegs() const { return this->currIceRotInDegs; }
    float GetCurrArmMoveForwardOffset() const { return this->currArmMoveForwardOffset; }
    float GetCurrArmZOffset() const { return CORE_BULB_Z_DIST + this->currArmMoveForwardOffset; }

    // Inherited from Boss
    bool ProjectilePassesThrough(const Projectile* projectile) const;
    //bool ProjectileIsDestroyedOnCollision(const Projectile* projectile, BossBodyPart* collisionPart) const;

private:
    static const float CORE_SHIELD_CORNER_HEIGHT;

    // The number of pieces from the side of the level to the first solid block making up the walls of the arena
    static const int LEVEL_X_OUTER_PADDING_NUM_PIECES = 4;
    // The number of pieces along the x-axis on the interior of a sub-arena
    static const int LEVEL_SUBARENA_WIDTH_NUM_PIECES  = 9;

    // The number of pieces from the top/bottom of the level to the first 
    // solid/no-entry block making up the enclosed sub arena
    static const int LEVEL_Y_OUTER_PADDING_NUM_PIECES = 4;
    // The number of pieces along the y-axis on the interior of the sub-arena
    static const int LEVEL_SUBARENA_HEIGHT_NUM_PIECES = 28;

    // The number of units along x and y from the inside blocks of the sub arena to where the boss' body can be positioned
    static const float BLOCK_TO_BOSS_X_PADDING;
    static const float BLOCK_TO_BOSS_Y_PADDING;

    // Dimensions for portals shot by the boss
    static const float PORTAL_PROJECTILE_WIDTH;
    static const float PORTAL_PROJECTILE_HEIGHT;
    static const float PORTAL_PROJECTILE_HALF_WIDTH;
    static const float PORTAL_PROJECTILE_HALF_HEIGHT;

    enum ShieldLimbType { TopShield, BottomShield, LeftShield, RightShield, CoreShield };

    // Core Assembly Indices
    size_t coreAssemblyIdx, coreBodyIdx, topBulbIdx, bottomBulbIdx, leftBulbIdx, rightBulbIdx;
    // Shield Indices
    size_t coreShieldIdx, topShieldIdx, bottomShieldIdx, leftShieldIdx, rightShieldIdx;

    float currIceRotInDegs;
    float currArmMoveForwardOffset;
    

    // Valid movement positions for the boss in each half of the level and for when the level is eventually opened up
    // NOTE: Treat these as constant!!
    std::vector<Point2D> leftSideMovePositions;
    std::vector<Point2D> rightSideMovePositions;
    std::vector<Point2D> fullLevelMovePositions;

    std::set<LevelPiece*> leftSubArenaPieces;
    std::set<LevelPiece*> rightSubArenaPieces;
    std::set<LevelPiece*> fullArenaPieces;

    std::vector<Colour> portalProjectileColours;
    int currPortalProjectileColourIdx;

    FuturismBoss();

    // Inherited from Boss
    void Init(float startingX, float startingY, const std::vector<std::vector<LevelPiece*> >& levelPieces);

    // FuturismBoss Helper Methods
    void AddSpecialCoreBounds(ShieldLimbType sectorToAdd);
    void UpdateBoundsForDestroyedShieldLimb(ShieldLimbType type);
    
    void RegenerateBoundsForCoreWithShield();
    void RegenerateBoundsForFinalCore();

    size_t GetShieldIndex(ShieldLimbType type) const;
    BossBodyPart* GetShieldBodyPart(ShieldLimbType type) { return static_cast<BossBodyPart*>(this->bodyParts[this->GetShieldIndex(type)]); }
    
    void TopShieldUpdate();
    void BottomShieldUpdate();
    void LeftShieldUpdate();
    void RightShieldUpdate();
    
    bool AllLimbShieldsDestroyed() const {
        return this->GetTopShield()->GetIsDestroyed() && this->GetBottomShield()->GetIsDestroyed() && 
            this->GetLeftShield()->GetIsDestroyed() && this->GetRightShield()->GetIsDestroyed();
    }

    const Colour& GetAndIncrementPortalColour() {
        this->currPortalProjectileColourIdx = (this->currPortalProjectileColourIdx + 1) % 
            static_cast<int>(this->portalProjectileColours.size());
        return this->portalProjectileColours[this->currPortalProjectileColourIdx];
    }

    // Info Accessors for AI States
    static float GetSubArenaWidth()   { return FuturismBoss::LEVEL_SUBARENA_WIDTH_NUM_PIECES * LevelPiece::PIECE_WIDTH; }
    static float GetTotalArenaWidth() { return (2*FuturismBoss::LEVEL_SUBARENA_WIDTH_NUM_PIECES+1)*LevelPiece::PIECE_WIDTH; }
    static float GetSubArenaHeight()  { return FuturismBoss::LEVEL_SUBARENA_HEIGHT_NUM_PIECES * LevelPiece::PIECE_HEIGHT; }

    // Confines
    // NOTE: The confines do NOT take into account the interior level padding or the size of the boss itself!!
    static float GetLeftSubArenaMinXConfines() { return FuturismBoss::LEVEL_X_OUTER_PADDING_NUM_PIECES * LevelPiece::PIECE_WIDTH; }
    static float GetLeftSubArenaMaxXConfines() { return FuturismBoss::GetLeftSubArenaMinXConfines() + FuturismBoss::GetSubArenaWidth(); }
    static float GetLeftSubArenaMinYConfines() { return FuturismBoss::LEVEL_Y_OUTER_PADDING_NUM_PIECES * LevelPiece::PIECE_HEIGHT; }
    static float GetLeftSubArenaMaxYConfines() { return FuturismBoss::GetSubArenaHeight(); }

    static float GetRightSubArenaMinXConfines() { return FuturismBoss::GetLeftSubArenaMaxXConfines() + LevelPiece::PIECE_WIDTH; }
    static float GetRightSubArenaMaxXConfines() { return FuturismBoss::GetRightSubArenaMinXConfines() + FuturismBoss::GetSubArenaWidth(); }
    static float GetRightSubArenaMinYConfines() { return FuturismBoss::LEVEL_Y_OUTER_PADDING_NUM_PIECES * LevelPiece::PIECE_HEIGHT; }
    static float GetRightSubArenaMaxYConfines() { return FuturismBoss::GetSubArenaHeight(); }
    
    // Padded Confines
    static float GetLeftSubArenaMinXPaddedConfines() { return FuturismBoss::GetLeftSubArenaMinXConfines() + FuturismBoss::BLOCK_TO_BOSS_X_PADDING; }
    static float GetLeftSubArenaMaxXPaddedConfines() { return FuturismBoss::GetLeftSubArenaMaxXConfines() - FuturismBoss::BLOCK_TO_BOSS_X_PADDING; }
    static float GetLeftSubArenaMinYPaddedConfines() { return FuturismBoss::GetLeftSubArenaMinYConfines() + FuturismBoss::BLOCK_TO_BOSS_Y_PADDING; }
    static float GetLeftSubArenaMaxYPaddedConfines() { return FuturismBoss::GetLeftSubArenaMaxYConfines() - FuturismBoss::BLOCK_TO_BOSS_Y_PADDING; }

    static float GetRightSubArenaMinXPaddedConfines() { return FuturismBoss::GetRightSubArenaMinXConfines() + FuturismBoss::BLOCK_TO_BOSS_X_PADDING; }
    static float GetRightSubArenaMaxXPaddedConfines() { return FuturismBoss::GetRightSubArenaMaxXConfines() - FuturismBoss::BLOCK_TO_BOSS_X_PADDING; }
    static float GetRightSubArenaMinYPaddedConfines() { return FuturismBoss::GetRightSubArenaMinYConfines() + FuturismBoss::BLOCK_TO_BOSS_Y_PADDING; }
    static float GetRightSubArenaMaxYPaddedConfines() { return FuturismBoss::GetRightSubArenaMaxYConfines() - FuturismBoss::BLOCK_TO_BOSS_Y_PADDING; }

    // Min and Max Boss Movement Confines (Padding and Boss Size are Accommodated)
    static float GetLeftSubArenaMinXBossPos(float halfWidth)  { return FuturismBoss::GetLeftSubArenaMinXPaddedConfines() + halfWidth;  }
    static float GetLeftSubArenaMaxXBossPos(float halfWidth)  { return FuturismBoss::GetLeftSubArenaMaxXPaddedConfines() - halfWidth;  }
    static float GetLeftSubArenaMinYBossPos(float halfHeight) { return FuturismBoss::GetLeftSubArenaMinYPaddedConfines() + halfHeight; }
    static float GetLeftSubArenaMaxYBossPos(float halfHeight) { return FuturismBoss::GetLeftSubArenaMaxYPaddedConfines() - halfHeight; }

    static float GetRightSubArenaMinXBossPos(float halfWidth)  { return FuturismBoss::GetRightSubArenaMinXPaddedConfines() + halfWidth;  }
    static float GetRightSubArenaMaxXBossPos(float halfWidth)  { return FuturismBoss::GetRightSubArenaMaxXPaddedConfines() - halfWidth;  }
    static float GetRightSubArenaMinYBossPos(float halfHeight) { return FuturismBoss::GetRightSubArenaMinYPaddedConfines() + halfHeight; }
    static float GetRightSubArenaMaxYBossPos(float halfHeight) { return FuturismBoss::GetRightSubArenaMaxYPaddedConfines() - halfHeight; }

    static Point2D GetLeftSubArenaCenterPos() {
        float minX = GetLeftSubArenaMinXConfines();
        float minY = GetLeftSubArenaMinYConfines();
        return Point2D(minX + (GetLeftSubArenaMaxXConfines() - minX) / 2.0f, 
            minY + (GetLeftSubArenaMaxYConfines() - minY) / 2.0f);
    }
    static Point2D GetRightSubArenaCenterPos() {
        float minX = GetRightSubArenaMinXConfines();
        float minY = GetRightSubArenaMinYConfines();
        return Point2D(minX + (GetRightSubArenaMaxXConfines() - minX) / 2.0f, 
            minY + (GetRightSubArenaMaxYConfines() - minY) / 2.0f);
    }
    static Point2D GetFullArenaCenterPos() {
        float minX = GetLeftSubArenaMinXConfines();
        float minY = GetRightSubArenaMinYConfines();
        return Point2D(minX + (GetRightSubArenaMaxXConfines() - minX) / 2.0f, 
            minY + (GetRightSubArenaMaxYConfines() - minY) / 2.0f);
    }

    static bool IsInLeftSubArena(const Point2D& pt) {
        return pt[0] <= GetLeftSubArenaMaxXConfines() && pt[0] >= GetLeftSubArenaMinXConfines() &&
            pt[1] <= GetLeftSubArenaMaxYConfines() && pt[1] >= GetLeftSubArenaMinYConfines();
    }
    static bool IsInRightSubArena(const Point2D& pt) {
        return pt[0] <= GetRightSubArenaMaxXConfines() && pt[0] >= GetRightSubArenaMinXConfines() &&
            pt[1] <= GetRightSubArenaMaxYConfines() && pt[1] >= GetRightSubArenaMinYConfines();
    }

    static void GetRocketStrategyPortalSearchPieces(const GameLevel& level, std::set<LevelPiece*>& pieces);
    static Collision::AABB2D GetRocketStrategyPortalSearchAABB(const GameLevel& level, float halfPortalWidth, float halfPortalHeight) {
        return Collision::AABB2D(
            Point2D(level.GetLevelUnitWidth() - halfPortalWidth - (3*LevelPiece::PIECE_WIDTH - 2*halfPortalWidth), -halfPortalHeight + 15*LevelPiece::PIECE_HEIGHT), 
            Point2D(level.GetLevelUnitWidth() - halfPortalWidth, -halfPortalHeight + 15*LevelPiece::PIECE_HEIGHT + 6*LevelPiece::PIECE_HEIGHT));
    }

    static void GetIceStrategyPortalSearchPieces(const GameLevel& level, std::set<LevelPiece*>& pieces);
    static Collision::AABB2D GetIceStrategyPortalSearchAABB(float halfPortalWidth, float halfPortalHeight) {
        return Collision::AABB2D(
            Point2D(halfPortalWidth, halfPortalHeight + 11*LevelPiece::PIECE_HEIGHT),
            Point2D(halfPortalWidth + (3*LevelPiece::PIECE_WIDTH - 2*halfPortalWidth), halfPortalHeight + 11*LevelPiece::PIECE_HEIGHT + 6*LevelPiece::PIECE_HEIGHT));
    }

    static Vector2D GetCoreTopBulbLocalDir()    { return Vector2D(0,  1); }
    static Vector2D GetCoreBottomBulbLocalDir() { return Vector2D(0, -1); }
    static Vector2D GetCoreLeftBulbLocalDir()   { return Vector2D(-1, 0); }
    static Vector2D GetCoreRightBulbLocalDir()  { return Vector2D(1,  0); }

    static Vector2D GetCoreTopBulbLocalVec()    { return FuturismBoss::CORE_BULB_OFFSET_FROM_ORIGIN * GetCoreTopBulbLocalDir(); }
    static Vector2D GetCoreBottomBulbLocalVec() { return FuturismBoss::CORE_BULB_OFFSET_FROM_ORIGIN * GetCoreBottomBulbLocalDir(); }
    static Vector2D GetCoreLeftBulbLocalVec()   { return FuturismBoss::CORE_BULB_OFFSET_FROM_ORIGIN * GetCoreLeftBulbLocalDir(); }
    static Vector2D GetCoreRightBulbLocalVec()  { return FuturismBoss::CORE_BULB_OFFSET_FROM_ORIGIN * GetCoreRightBulbLocalDir(); }

    static void GetBarrierPiecesTopToBottom(const GameLevel& level, std::vector<LevelPiece*>& pieces);

    // DEBUGGING...
#ifdef _DEBUG
    void DebugDraw() const;
#endif

    DISALLOW_COPY_AND_ASSIGN(FuturismBoss);
};

inline float FuturismBoss::GetCurrentZShootDistFromOrigin() const {
    return this->GetCoreShield()->GetIsDestroyed() ? (CORE_Z_SHOOT_DIST_FROM_ORIGIN_WITHOUT_SHIELD + 0.1f) : CORE_Z_SHOOT_DIST_FROM_ORIGIN_WITH_SHIELD;
}
inline float FuturismBoss::GetCurrentBeamZShootDistFromOrigin() const {
    return this->GetCoreShield()->GetIsDestroyed() ? CORE_Z_SHOOT_DIST_FROM_ORIGIN_WITHOUT_SHIELD : CORE_Z_SHOOT_DIST_FROM_ORIGIN_WITH_SHIELD;
}
#endif // __FUTURISMBOSS_H__
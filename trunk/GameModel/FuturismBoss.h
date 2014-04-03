/**
 * FuturismBoss.h
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

#ifndef __FUTURISMBOSS_H__
#define __FUTURISMBOSS_H__

class BossBodyPart;
class BossWeakpoint;
class BossCompositeBodyPart;


class FuturismBossAIState;
class FuturismBossStage1AIState;

#include "Boss.h"
#include "GameLevel.h"

class FuturismBoss : public Boss {

    friend Boss* Boss::BuildStyleBoss(GameModel* gameModel, const GameWorld::WorldStyle& style);
    friend class FuturismBossAIState;
    friend class FuturismBossStage1AIState;

public:
    static const float FULLY_SHIELDED_BOSS_HEIGHT;
    static const float FULLY_SHIELDED_BOSS_HALF_HEIGHT;
    static const float FULLY_SHIELDED_BOSS_WIDTH;
    static const float FULLY_SHIELDED_BOSS_HALF_WIDTH;

    static const float CORE_BOSS_SIZE;
    static const float CORE_BOSS_HALF_SIZE;

    static const float CORE_EYE_SIZE;

    static const float CORE_Z_DIST_FROM_ORIGIN;
    static const float CORE_Z_SHOOT_DIST_FROM_ORIGIN;

    ~FuturismBoss();

    // Get the various shields of the Boss
    const BossBodyPart* GetCoreShield() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->coreShieldIdx]); }
    const BossBodyPart* GetTopShield() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->topShieldIdx]); }
    const BossBodyPart* GetBottomShield() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->bottomShieldIdx]); }
    const BossBodyPart* GetLeftShield() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftShieldIdx]); }
    const BossBodyPart* GetRightShield() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightShieldIdx]); }

    // Get the core parts of the boss
    const BossBodyPart* GetCoreBody() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->coreBodyIdx]); }
    const BossBodyPart* GetCoreTopBulb() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->topBulbIdx]); }
    const BossBodyPart* GetCoreBottomBulb() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->bottomBulbIdx]); }
    const BossBodyPart* GetCoreLeftBulb() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->leftBulbIdx]); }
    const BossBodyPart* GetCoreRightBulb() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->rightBulbIdx]); }

    bool IsCoreShieldVulnerable() const;

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

    enum ShieldLimbType { TopShield, BottomShield, LeftShield, RightShield };

    // Core Assembly Indices
    size_t coreAssemblyIdx, coreBodyIdx, topBulbIdx, bottomBulbIdx, leftBulbIdx, rightBulbIdx;
    // Shield Indices
    size_t coreShieldIdx, topShieldIdx, bottomShieldIdx, leftShieldIdx, rightShieldIdx;

    float currCoreRotInDegs;

    // Valid movement positions for the boss in each half of the level and for when the level is eventually opened up
    // NOTE: Treat these as constant!!
    std::vector<Point2D> leftSideMovePositions;
    std::vector<Point2D> rightSideMovePositions;
    std::vector<Point2D> fullLevelMovePositions;

    std::set<LevelPiece*> leftSubArenaPieces;
    std::set<LevelPiece*> rightSubArenaPieces;

    std::vector<Colour> portalProjectileColours;
    int currPortalProjectileColourIdx;

    FuturismBoss();

    // Inherited from Boss
    void Init(float startingX, float startingY, const std::vector<std::vector<LevelPiece*> >& levelPieces);

    // FuturismBoss Helper Methods
    void AddCoreShieldBounds(ShieldLimbType sectorToAdd);
    void UpdateBoundsForDestroyedShieldLimb(ShieldLimbType type);
    void RegenerateBoundsForFinalCore();

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

    static void GetRocketStrategyPortalSearchPieces(const GameLevel& level, std::set<LevelPiece*>& pieces);
    static Collision::AABB2D GetRocketStrategyPortalSearchAABB(const GameLevel& level, float halfPortalWidth, float halfPortalHeight) {
        return Collision::AABB2D(
            Point2D(level.GetLevelUnitWidth() - halfPortalWidth - (3*LevelPiece::PIECE_WIDTH - 2*halfPortalWidth), halfPortalHeight + 11*LevelPiece::PIECE_HEIGHT), 
            Point2D(level.GetLevelUnitWidth() - halfPortalWidth, halfPortalHeight + 11*LevelPiece::PIECE_HEIGHT + 6*LevelPiece::PIECE_HEIGHT));
    }

    static void GetIceStrategyPortalSearchAABB(const GameLevel& level, std::set<LevelPiece*>& pieces);
    static Collision::AABB2D GetIceStrategyPortalSearchAABB(float halfPortalWidth, float halfPortalHeight) {
        return Collision::AABB2D(
            Point2D(halfPortalWidth, -halfPortalHeight + 15*LevelPiece::PIECE_HEIGHT),
            Point2D(halfPortalWidth + (3*LevelPiece::PIECE_WIDTH - 2*halfPortalWidth), -halfPortalHeight + 15*LevelPiece::PIECE_HEIGHT + 6*LevelPiece::PIECE_HEIGHT));
    }

    // DEBUGGING...
#ifdef _DEBUG
    void DebugDraw() const;
#endif

    DISALLOW_COPY_AND_ASSIGN(FuturismBoss);
};
#endif // __FUTURISMBOSS_H__
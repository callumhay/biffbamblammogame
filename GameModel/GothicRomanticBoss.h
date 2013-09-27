/**
 * GothicRomanticBoss.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __GOTHICROMANTICBOSS_H__
#define __GOTHICROMANTICBOSS_H__

class BossBodyPart;
class BossWeakpoint;
class BossCompositeBodyPart;

namespace gothicromanticbossai {
class GothicRomanticBossAI;
class ConfinedAI;
class FireBallAI;
class IceBallAI;
class FreeMovingAttackAI;
};

#include "Boss.h"

/**
 * Child class of Boss that represents the Gothic and Romantic movement's boss ("Itemmancer").
 * This class contains all of the setup and constants and particulars for the
 * Gothic and Romantic movement boss.
 */
class GothicRomanticBoss : public Boss {

    friend Boss* Boss::BuildStyleBoss(const GameWorld::WorldStyle& style);
    friend class gothicromanticbossai::GothicRomanticBossAI;
    friend class gothicromanticbossai::ConfinedAI;
    friend class gothicromanticbossai::FireBallAI;
    friend class gothicromanticbossai::IceBallAI;
    friend class gothicromanticbossai::FreeMovingAttackAI;

public:
    static const int NUM_LEGS = 8;

    static const float TOTAL_WIDTH_WITH_LEGS;
    static const float TOTAL_HEIGHT;

    static const float BODY_WIDTH;
    static const float BODY_HEIGHT;
    static const float HALF_BODY_WIDTH;
    static const float HALF_BODY_HEIGHT;
    
    static const float TOP_POINT_WIDTH;
    static const float HALF_TOP_POINT_WIDTH;
    static const float TOP_POINT_HEIGHT;
    static const float TOP_POINT_TIP_Y;

    static const float BOTTOM_POINT_WIDTH;
    static const float HALF_BOTTOM_POINT_WIDTH;
    static const float BOTTOM_POINT_HEIGHT;
    static const float BOTTOM_POINT_TIP_Y;

    static const float LEG_WIDTH;
    static const float LEG_HEIGHT;

    static const float DEFAULT_ACCELERATION;

    static const double DELAY_BEFORE_SUMMONING_ITEMS_IN_SECS;

    ~GothicRomanticBoss();
    
    bool ProjectilePassesThrough(const Projectile* projectile) const;

    const BossBodyPart* GetBody() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->bodyIdx]); }
    const BossBodyPart* GetTopPoint() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->topPointIdx]); }
    const BossBodyPart* GetBottomPoint() const { return static_cast<const BossBodyPart*>(this->bodyParts[this->bottomPointIdx]); }
    const BossBodyPart* GetLeg(int idx) const { assert(idx >= 0 && idx < 8); return static_cast<const BossBodyPart*>(this->bodyParts[this->legIdxs[idx]]); }

    Point3D GetLegPointPos(int idx) const;
    Point3D GetBottomPointTipPos() const;
    Point3D GetTopPointTipPos() const;

private:
    static const float LEVEL_WIDTH;
    static const float LEVEL_HEIGHT;

    size_t bodyIdx;
    size_t topPointIdx, bottomPointIdx;
    size_t legIdxs[NUM_LEGS];

    GothicRomanticBoss();

    static const int CONFINES_NUM_PIECES_IN_X = 5;
    float GetMinXOfConfines() const { return CONFINES_NUM_PIECES_IN_X*LevelPiece::PIECE_WIDTH; }
    float GetMaxXOfConfines() const { return LEVEL_WIDTH - CONFINES_NUM_PIECES_IN_X*LevelPiece::PIECE_WIDTH; }
    float GetMinYOfConfines() const { return 12*LevelPiece::PIECE_HEIGHT; }
    float GetMaxYOfConfines() const { return LEVEL_HEIGHT - 6*LevelPiece::PIECE_HEIGHT; }

    void BuildLeg(const Vector3D& legTranslation, float legYRotation, size_t& legIdx);

    // Inherited from Boss
    void Init(float startingX, float startingY);

    DISALLOW_COPY_AND_ASSIGN(GothicRomanticBoss);
};

#endif // __GOTHICROMANTICBOSS_H__
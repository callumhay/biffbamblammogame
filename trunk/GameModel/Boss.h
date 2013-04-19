/**
 * Boss.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BOSS_H__
#define __BOSS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Vector.h"

#include "BoundingLines.h"
#include "GameBall.h"
#include "PlayerPaddle.h"
#include "GameWorld.h"
#include "BossBodyPart.h"
#include "BossCompositeBodyPart.h"

class BossAIState;

class Boss {
    friend class BossAIState;
public:
    static const double WAIT_BEFORE_FADE_TO_BLACK_FINAL_DEAD_BODY_PART_TIME;
    static const double FADE_TO_BLACK_FINAL_DEAD_BODY_PART_TIME;
    static const double TOTAL_DEATH_ANIM_TIME;

    virtual ~Boss();

    static Boss* BuildStyleBoss(const GameWorld::WorldStyle& style);

    Collision::AABB2D GenerateDyingAABB() const;

    float GetAliveHeight() const;
    float GetAliveWidth() const;
    
	BossBodyPart* CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
        Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const;
    BossBodyPart* CollisionCheck(const PlayerPaddle& paddle) const;
	BossBodyPart* CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	BossBodyPart* CollisionCheck(const BoundingLines& boundingLines, const Vector2D& velDir) const;
	BossBodyPart* CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) const;

    void Translate(const Vector3D& t);

    void Tick(double dT, GameModel* gameModel);

    bool IsBodyPartDead(const AbstractBossBodyPart* bodyPart) const;

    bool GetIsStateMachineFinished() const;
    void SetIsLevelCompleteDead(bool isComplete);
    bool GetIsLevelCompleteDead() const;

	void CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart);
	void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);
    void CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart);

    bool CanHurtPaddleWithBody() const;

    virtual bool ProjectilePassesThrough(const Projectile* projectile) const = 0;

    // Builders for various boss animations
    static AnimationMultiLerp<ColourRGBA> BuildBossHurtAndInvulnerableColourAnim();
    static AnimationMultiLerp<ColourRGBA> BuildBossHurtFlashAndFadeAnim(double totalAnimTime);
    static AnimationMultiLerp<ColourRGBA> BuildBossFinalDeathFlashAnim();
    static AnimationMultiLerp<ColourRGBA> BuildBossAngryFlashAnim();
    static AnimationMultiLerp<Vector3D>   BuildBossAngryShakeAnim(float shakeMagnitude);
    static AnimationMultiLerp<Vector3D>   BuildBossFinalDeathShakeAnim(float shakeMagnitude);
    static AnimationMultiLerp<Vector3D>   BuildBossHurtMoveAnim(const Vector2D& hurtDir, float shakeMagnitude);

    static AnimationMultiLerp<Vector3D> BuildLimbFallOffTranslationAnim(double totalAnimTime, float xDist, float yDist);
    static AnimationMultiLerp<float> BuildLimbFallOffZRotationAnim(double totalAnimTime, float rotAmtInDegs);

    // DEBUGGING...
#ifdef _DEBUG
    void DebugDraw() const;
#endif

protected:
    BossAIState* currAIState;
    BossAIState* nextAIState;

    BossCompositeBodyPart* root;
    BossCompositeBodyPart* deadPartsRoot;
    BossCompositeBodyPart* alivePartsRoot;

    std::vector<AbstractBossBodyPart*> bodyParts;

    Boss();

    void ConvertAliveBodyPartToWeakpoint(size_t index, float lifePoints, float ballDmgOnHit);
    void ConvertAliveBodyPartToDeadBodyPart(size_t index);
    void ConvertAliveBodyPartToDeadBodyPart(AbstractBossBodyPart* bodyPart);

    void SetNextAIState(BossAIState* nextState);

    virtual void Init() = 0;

private:
    bool isBossDeadAndLevelCompleted;

    void UpdateAIState();
    void SetCurrentAIStateImmediately(BossAIState* newState);

    DISALLOW_COPY_AND_ASSIGN(Boss);
};

inline bool Boss::IsBodyPartDead(const AbstractBossBodyPart* bodyPart) const {
    return this->deadPartsRoot->IsOrContainsPart(bodyPart, true);
}

inline float Boss::GetAliveHeight() const {
    return this->alivePartsRoot->GenerateWorldAABB().GetHeight();
}

inline float Boss::GetAliveWidth() const {
    return this->alivePartsRoot->GenerateWorldAABB().GetWidth();
}

inline void Boss::Translate(const Vector3D& t) {
    this->root->Translate(t);
}

inline void Boss::SetIsLevelCompleteDead(bool isComplete) {
    this->isBossDeadAndLevelCompleted = isComplete;
}

inline bool Boss::GetIsLevelCompleteDead() const {
    return this->isBossDeadAndLevelCompleted;
}

inline BossBodyPart* Boss::CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
                                          Collision::LineSeg2D& collisionLine,
                                          double& timeSinceCollision) const {
    return this->alivePartsRoot->CollisionCheck(ball, dT, n, collisionLine, timeSinceCollision);
}

inline BossBodyPart* Boss::CollisionCheck(const PlayerPaddle& paddle) const {
    return this->alivePartsRoot->CollisionCheck(paddle);
}

inline BossBodyPart* Boss::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
    return this->alivePartsRoot->CollisionCheck(ray, rayT);
}

inline BossBodyPart* Boss::CollisionCheck(const BoundingLines& boundingLines, const Vector2D& velDir) const {
    return this->alivePartsRoot->CollisionCheck(boundingLines, velDir);
}

inline BossBodyPart* Boss::CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) const {
    return this->alivePartsRoot->CollisionCheck(c, velDir);
}

#endif // __BOSS_H__
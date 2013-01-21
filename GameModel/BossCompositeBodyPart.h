/**
 * BossCompositeBodyPart.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __COMPOSITEBOSSBODYPART_H__
#define __COMPOSITEBOSSBODYPART_H__

#include "AbstractBossBodyPart.h"

class BossBodyPart;

/**
 * The composite/decorator class of the boss body part hierarchy. Holds composites of
 * body parts (parts containing other parts) for the game's bosses.
 */
class BossCompositeBodyPart : public AbstractBossBodyPart {
public:
    BossCompositeBodyPart();
    ~BossCompositeBodyPart();

    void SetBodyParts(const std::vector<AbstractBossBodyPart*>& parts);
    void AddBodyPart(AbstractBossBodyPart* part);
    void RemoveBodyPart(AbstractBossBodyPart* part); // Not recursive

    // Inherited from AbstractBossBodyPart
    AbstractBossBodyPart::Type GetType() const { return AbstractBossBodyPart::CompositeBodyPart; }
    void Tick(double dT);
    BossBodyPart* CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
        Collision::LineSeg2D& collisionLine, double& timeSinceCollision);
    BossBodyPart* CollisionCheck(const PlayerPaddle& paddle);
	BossBodyPart* CollisionCheck(const Collision::Ray2D& ray, float& rayT);
	BossBodyPart* CollisionCheck(const BoundingLines& boundingLines, const Vector2D& velDir);
	BossBodyPart* CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir);

    void Translate(const Vector3D& t);
    void RotateZ(float rotZDegs);
    void Transform(const Matrix4x4& m);

    void SetLocalTranslation(const Vector3D& t);
    void SetLocalZRotation(float zRotInDegs);
    void SetLocalTransform(const Vector3D& translation, float zRotInDegs);

    void GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const;
	void TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
    
    //void TickStatus(double dT, GameModel* gameModel, int32_t& removedStatuses);

    bool IsOrContainsPart(AbstractBossBodyPart* part, bool recursiveSearch) const;
    AbstractBossBodyPart* SearchForParent(AbstractBossBodyPart* part);

    bool GetIsDestroyed() const;
    void SetAsDestroyed();

    Collision::AABB2D GenerateWorldAABB() const;

    void AnimateColourRGBA(const AnimationMultiLerp<ColourRGBA>& rgbaAnim);
    void ResetColourRGBAAnimation();
        
#ifdef _DEBUG
    void DebugDraw() const;
#endif

private:
    std::vector<AbstractBossBodyPart*> childParts;
    bool isDestroyed;

    DISALLOW_COPY_AND_ASSIGN(BossCompositeBodyPart);
};


inline void BossCompositeBodyPart::SetBodyParts(const std::vector<AbstractBossBodyPart*>& parts) {
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        delete this->childParts[i];
    }
    this->childParts = parts;
}

inline void BossCompositeBodyPart::AddBodyPart(AbstractBossBodyPart* part) {
    this->childParts.push_back(part);
}

#endif // __COMPOSITEBOSSBODYPART_H__
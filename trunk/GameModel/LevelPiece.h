/**
 * LevelPiece.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __LEVELPIECE_H__
#define __LEVELPIECE_H__

#include "../BlammoEngine/IPositionObject.h"
#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Matrix.h"

#include "BoundingLines.h"
#include "GameBall.h"
#include "PlayerPaddle.h"

class Circle2D;
class LineSeg2D;
class Vector2D;
class GameLevel;
class GameModel;
class Projectile;
class BeamSegment;
class SwitchBlock;

class LevelPiece : public IPositionObject {
public:
    // Typedef and constants for trigger IDs - used to indicate a unique id for a level piece
    // so it can be triggered by switches
    typedef int TriggerID;
    static const TriggerID NO_TRIGGER_ID;

	// All level pieces must conform to these measurements...
	static const float PIECE_WIDTH;
	static const float PIECE_HEIGHT;
	static const float PIECE_DEPTH;
	static const float HALF_PIECE_WIDTH;
	static const float HALF_PIECE_HEIGHT;
	static const float HALF_PIECE_DEPTH;

    enum DestructionMethod { 
        NotApplicableDestruction, RegularDestruction, LaserProjectileDestruction, RocketDestruction, LaserBeamDestruction,
        CollateralDestruction, PaddleShieldDestruction, BombDestruction, FireDestruction,
        TeslaDestruction, MineDestruction, IceShatterDestruction, DisintegrationDestruction, BasicProjectileDestruction
    }; 

	enum LevelPieceType {
        Breakable, Solid, Empty, Bomb, SolidTriangle, BreakableTriangle, Ink, Prism, Portal, PrismTriangle, Cannon,
        Collateral, Tesla, ItemDrop, Switch, OneWay, NoEntry, LaserTurret, RocketTurret, MineTurret, AlwaysDrop, Regen
    };

	virtual LevelPieceType GetType() const = 0;
    static bool IsValidLevelPieceType(int pieceType);
    virtual bool IsExplosionStoppedByPiece(const Point2D& explosionCenter);

	LevelPiece(unsigned int wLoc, unsigned int hLoc);
	virtual ~LevelPiece();

	void SetWidthAndHeightIndex(unsigned int wLoc, unsigned int hLoc);

    // Inherited from IPositionObject interface
    Point3D GetPosition3D() const {
        return Point3D(this->GetCenter(), 0.0f);
    }

	const Point2D& GetCenter() const {	return this->center; }
	const BoundingLines& GetBounds() const { return this->bounds; }
	size_t GetWidthIndex() const { return this->wIndex; }
	size_t GetHeightIndex() const { return this->hIndex; }
	const ColourRGBA& GetColour() const { return this->colour; }
	void SetColour(const ColourRGBA& c) { this->colour = c; }

	virtual Matrix4x4 GetPieceToLevelTransform() const {
		return Matrix4x4::translationMatrix(Vector3D(this->center[0], this->center[1], 0.0f));
	}
	virtual Matrix4x4 GetPieceToLevelInvTransform() const {
		return Matrix4x4::translationMatrix(Vector3D(-this->center[0], -this->center[1], 0.0f));
	}

	virtual Collision::AABB2D GetAABB() const;
    virtual bool SecondaryCollisionCheck(double dT, const GameBall& ball) const;
	virtual bool CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, 
        double& timeUntilCollision, Point2D& pointOfCollision) const;
	virtual bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	virtual bool CollisionCheck(const BoundingLines& boundingLines, double dT, const Vector2D& velocity) const;
	virtual bool CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) const;

	// Collision related stuffs
    virtual LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) = 0;

	virtual void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
	    const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
	    const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
	    const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	virtual LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball) = 0;
	virtual LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile) = 0;
    virtual LevelPiece* CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);

	virtual void GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const;
	virtual LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
	virtual LevelPiece* TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel);

	virtual bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses);

    virtual bool ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const = 0;

	virtual bool IsNoBoundsPieceType() const = 0;
	virtual bool BallBouncesOffWhenHit() const = 0;
	virtual bool MustBeDestoryedToEndLevel() const = 0;
	virtual bool CanBeDestroyedByBall() const = 0;
    virtual bool CanChangeSelfOrOtherPiecesWhenHit() const = 0;
	virtual bool BallBlastsThrough(const GameBall& b) const = 0;
	virtual bool GhostballPassesThrough() const = 0;
	virtual bool ProjectilePassesThrough(const Projectile* projectile) const = 0;
	virtual bool IsLightReflectorRefractor() const = 0;

    virtual int GetPointsOnChange(const LevelPiece& changeToPiece) const = 0;

    virtual bool ProjectileIsDestroyedOnCollision(const Projectile* projectile) const;

    virtual bool IsAIPiece() const { return false; }
    virtual void AITick(double dT, GameModel* gameModel) { UNUSED_PARAMETER(dT); UNUSED_PARAMETER(gameModel); }
    
    virtual void Triggered(GameModel* gameModel);
    void SetTriggerID(const LevelPiece::TriggerID& id) { this->triggerID = id; }
    const LevelPiece::TriggerID& GetTriggerID() const { return this->triggerID; }
    bool GetHasTriggerID() const { return (this->GetTriggerID() != LevelPiece::NO_TRIGGER_ID); };

	// Track the status of the piece, effects properties of the piece and how it works/acts in a level
	// NOTE: IF YOU ADD TO THIS DON'T FORGET TO UPDATE LevelPiece::RemoveStatuses !!!!!
	enum PieceStatus { NormalStatus = 0x00000000, OnFireStatus = 0x00000001, IceCubeStatus = 0x00000002 };
	bool HasStatus(int32_t statusMask) const;
	void AddStatus(GameLevel* level, const PieceStatus& status);
	virtual void RemoveStatus(GameLevel* level, const PieceStatus& status);
	void RemoveStatuses(GameLevel* level, int32_t statusMask);

    // Projectile attachment functionality
    void AttachProjectile(MineProjectile* p);
    void DetachProjectile(MineProjectile* p);
    void DetachAllProjectiles();

    virtual void DrawWireframe() const;

	// Debug Stuffs
	void DebugDraw() const;

protected:
	ColourRGBA colour;        // The colour of this level piece
	Point2D center;           // The exact center of this piece in the game model
	size_t wIndex, hIndex;    // The width and height index to where this block is in its level
	BoundingLines bounds;     // The bounding box, rep. as lines forming the boundary of this, kept in world space
    
    TriggerID triggerID;

    std::set<MineProjectile*> attachedProjectiles; // Any projectiles that have latched on to this piece

	// Pointers to any neighboring level pieces to this one - if a neighbor does not
	// exist it will be NULL - these get initialized/set with any call to UpdateBounds.
	const LevelPiece* leftNeighbor;
	const LevelPiece* bottomNeighbor;
	const LevelPiece* rightNeighbor;
	const LevelPiece* topNeighbor;
	const LevelPiece* topRightNeighbor;
	const LevelPiece* topLeftNeighbor;
	const LevelPiece* bottomRightNeighbor;
	const LevelPiece* bottomLeftNeighbor;

	void SetBounds(const BoundingLines& bounds, const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                   const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                   const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                   const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

    static void UpdateBreakableBlockBounds(LevelPiece* thePiece,
        const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
        const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
        const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
        const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);
    static void UpdateSolidRectBlockBounds(LevelPiece* thePiece,
        const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
        const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
        const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
        const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	int32_t pieceStatus;

	void RemoveAllStatus(GameLevel* level);

	void LightPieceOnFire(GameModel* gameModel, bool canCatchOnFire = true);
	void FreezePieceInIce(GameModel* gameModel, bool canBeFrozen = true);

	void DoIceCubeReflectRefractLaserBullets(Projectile* projectile, GameModel* gameModel) const;
	void GetIceCubeReflectionRefractionRays(const Point2D& currCenter, const Vector2D& currDir, 
	    std::list<Collision::Ray2D>& rays) const;
    bool DoPossibleFireGlobDrop(GameModel* gameModel, bool alwaysDrop) const;

private:
	DISALLOW_COPY_AND_ASSIGN(LevelPiece);
};

inline bool LevelPiece::IsExplosionStoppedByPiece(const Point2D&) {
    // By default most level pieces will allow an explosion to continue through them
    return false;
}

inline Collision::AABB2D LevelPiece::GetAABB() const {
	return Collision::AABB2D(this->center - Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
                             this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
}

inline bool LevelPiece::SecondaryCollisionCheck(double dT, const GameBall& ball) const {
    UNUSED_PARAMETER(dT);

	if (this->IsNoBoundsPieceType()) {
		return false;
	}
	if (ball.IsLastPieceCollidedWith(this)) {
		return false;
	}
    return true;
}

/**
 * Check for a collision of a given circle with this block.
 * Returns: true on collision as well as the normal of the line being collided with
 * and the distance from that line of the given circle; false otherwise.
 */
inline bool LevelPiece::CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
                                       Collision::LineSeg2D& collisionLine,
                                       double& timeUntilCollision, Point2D& pointOfCollision) const {

	// If there are no bounds to collide with or this level piece was the
	// last one collided with then we can't collide with this piece
    if (!this->SecondaryCollisionCheck(dT, ball)) {
        return false;
    }
	return this->bounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), n, 
        collisionLine, timeUntilCollision, pointOfCollision);
}

/**
 * Check for a collision of a given ray with this block. Also, on collision, will
 * set the value rayT to the value on the ray where the collision occurred.
 * Returns: true on collision, false otherwise.
 */
inline bool LevelPiece::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	if (this->IsNoBoundsPieceType()) {
		return false;
	}
	return Collision::IsCollision(ray, this->GetAABB(), rayT);
}

/**
 * Check for a collision of a given set of bounding lines with this block.
 * Returns: true on collision, false otherwise.
 */
inline bool LevelPiece::CollisionCheck(const BoundingLines& boundingLines, double dT, const Vector2D& velocity) const {
	if (this->IsNoBoundsPieceType()) {
		return false;
	}

	return this->bounds.CollisionCheck(boundingLines, dT, velocity);
}

inline bool LevelPiece::CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) const {
    UNUSED_PARAMETER(velDir);
	if (this->IsNoBoundsPieceType()) {
		return false;
	}
	return Collision::IsCollision(this->GetAABB(), c);
}

inline LevelPiece* LevelPiece::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle) {
    UNUSED_PARAMETER(gameModel);
    UNUSED_PARAMETER(paddle);
    return this;
}

/**
 * Hit this block with the given beam segment - if this block reflects or refracts
 * the beam segment then this will return any new beam segments created by that reflection/
 * refraction.
 */
inline void LevelPiece::GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const {
	// If this piece is frozen then there are reflection/refraction rays...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		this->GetIceCubeReflectionRefractionRays(hitPoint, impactDir, rays);
	}
	else {
		// The default behaviour is to just not do any reflection/refraction and return an empty list
		rays.clear();
	}
}

inline LevelPiece* LevelPiece::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(gameModel);
    UNUSED_PARAMETER(beamSegment);

	assert(beamSegment != NULL);
	assert(gameModel != NULL);
	return this;
}

inline LevelPiece* LevelPiece::TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel) {
	// By default this does nothing
    UNUSED_PARAMETER(gameModel);
	UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(paddle);

	assert(gameModel != NULL);
	return this;
}

// Used to tick the status effects on a particular level piece.
// removedStatuses [out]: Any status effects that have been removed during this tick, 0x00000000 (i.e., NormalStatus)
// if no effects were removed.
// Returns: true to say that this piece no longer needs to tick, false if it does.
inline bool LevelPiece::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
	// By default this does nothing
	UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(gameModel);
	assert(gameModel != NULL);
	assert(false);
	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return true;
}

// When a switch (or some other triggering mechanism) triggers a block it calls this method
inline void LevelPiece::Triggered(GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);
    // Must be overridden for functionality.
    //return false;
}

// Draws the boundary lines and normals for this level piece.
inline void LevelPiece::DebugDraw() const {
	this->bounds.DebugDraw();
}

inline void LevelPiece::DrawWireframe() const {
    this->bounds.DrawSimpleBounds();
}

inline bool LevelPiece::HasStatus(int32_t statusMask) const {
	return (this->pieceStatus & statusMask) != 0x0;
}

inline void LevelPiece::SetBounds(const BoundingLines& bounds, const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                  const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                                  const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                                  const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {
	this->bounds = bounds;
	this->leftNeighbor        = leftNeighbor;
	this->bottomNeighbor      = bottomNeighbor;
	this->rightNeighbor       = rightNeighbor;
	this->topNeighbor         = topNeighbor;
	this->topRightNeighbor    = topRightNeighbor;
	this->topLeftNeighbor     = topLeftNeighbor;
	this->bottomRightNeighbor = bottomRightNeighbor;
	this->bottomLeftNeighbor  = bottomLeftNeighbor;
}

inline void LevelPiece::AttachProjectile(MineProjectile* p) {
    assert(p != NULL);
    this->attachedProjectiles.insert(p);
}

inline void LevelPiece::DetachProjectile(MineProjectile* p) {
    assert(p != NULL);
    this->attachedProjectiles.erase(p);
}

#endif
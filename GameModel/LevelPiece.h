#ifndef __LEVELPIECE_H__
#define __LEVELPIECE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Matrix.h"

#include "BoundingLines.h"

class Circle2D;
class LineSeg2D;
class Vector2D;
class GameBall;
class GameLevel;
class GameModel;
class Projectile;
class Beam;

class LevelPiece {

public:

	// All level pieces must conform to these measurements...
	static const float PIECE_WIDTH;
	static const float PIECE_HEIGHT;
	static const float HALF_PIECE_WIDTH;
	static const float HALF_PIECE_HEIGHT;

	enum LevelPieceType { Breakable, Solid, Empty, Bomb, SolidTriangle, BreakableTriangle, Ink, Prism, PrismTriangle };
	virtual LevelPieceType GetType() const = 0;

protected:
	Colour colour;								// The colour of this level piece
	Point2D center;								// The exact center of this piece in the game model
	unsigned int wIndex, hIndex;	// The width and height index to where this block is in its level
	BoundingLines bounds;			 		// The bounding box, rep. as lines forming the boundry of this, kept in world space

	void AddPossibleItemDrop(GameModel *gameModel);

public:
	LevelPiece(unsigned int wLoc, unsigned int hLoc);
	virtual ~LevelPiece();

	Point2D GetCenter() const {	return this->center; }
	unsigned int GetWidthIndex() const { return this->wIndex; }
	unsigned int GetHeightIndex() const { return this->hIndex; }
	Colour GetColour() const { return this->colour; }

	virtual Matrix4x4 GetPieceToLevelTransform() const {
		return Matrix4x4::translationMatrix(Vector3D(this->center[0], this->center[1], 0.0f));
	}
	virtual Matrix4x4 GetPieceToLevelInvTransform() const {
		return Matrix4x4::translationMatrix(Vector3D(-this->center[0], -this->center[1], 0.0f));
	}

	bool CollisionCheck(const Collision::Circle2D& c, const Vector2D& velocity, Vector2D& n, float& d) const;
	bool CollisionCheck(const Collision::AABB2D& aabb) const;
	bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	bool CollisionCheck(const BoundingLines& boundingLines) const;

	virtual int GetPointValueForCollision() = 0;

	// Collision related stuffs
	virtual LevelPiece* Destroy(GameModel* gameModel) = 0;
	virtual void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
													  const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
														const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
														const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	virtual LevelPiece* CollisionOccurred(GameModel* gameModel, const GameBall& ball) = 0;
	virtual LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile) = 0;

	virtual std::list<Collision::Ray2D> GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir) const;
	virtual LevelPiece* TickBeamCollision(double dT, const Beam* beam);

	// Debug Stuffs
	void DebugDraw() const;

	virtual bool IsNoBoundsPieceType() const = 0;
	virtual bool MustBeDestoryedToEndLevel() const = 0;
	virtual bool CanBeDestroyed() const = 0;
	virtual bool UberballBlastsThrough() const = 0;
	virtual bool GhostballPassesThrough() const = 0;
	virtual bool ProjectilePassesThrough(Projectile* projectile) = 0;
	virtual bool IsLightReflectorRefractor() const = 0;
};
#endif
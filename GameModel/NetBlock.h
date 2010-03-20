/**
 * NetBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __NETBLOCK_H__
#define __NETBLOCK_H__

#include "LevelPiece.h"

class NetBlock : public LevelPiece {

public:
	NetBlock(unsigned int wLoc, unsigned int hLoc);
	~NetBlock();

	LevelPieceType GetType() const { 
		return LevelPiece::Net;
	}

	// Net blocks have no bounds per-se... the ball doesn't bounce off of them,
	// instead, much like the portal block, they manipulate ball properties when it hits
	bool IsNoBoundsPieceType() const {
		return true;
	}

	// Net blocks don't need to be destroyed to end a level
	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyed() const {
		return true;
	}
	
	// Even the uber ball gets captured by the nefarious net block!
	bool UberballBlastsThrough() const {
		return false;
	}

	// Whether or not the ghost ball can just pass through this block.
	bool GhostballPassesThrough() const {
		return true;
	}

	// Obtain the point value for a collision with this block.
	int GetPointValueForCollision() {
		return 0;
	}

	// Particles pass through net blocks.
	bool ProjectilePassesThrough(Projectile* projectile) {
		return true;
	}

	// Light beams will hit this and pass through... in a slightly smaller form
	bool IsLightReflectorRefractor() const {
		return true;
	}

	LevelPiece* Destroy(GameModel* gameModel);
	
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	std::list<Collision::Ray2D> GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir) const;
};


#endif // __NETBLOCK_H__
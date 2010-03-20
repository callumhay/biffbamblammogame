#include "NetBlock.h"
#include "GameBall.h"
#include "Projectile.h"
#include "GameEventManager.h"
#include "GameLevel.h"
#include "GameModel.h"
#include "EmptySpaceBlock.h"
#include "GameTransformMgr.h"

NetBlock::NetBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc) {
}

NetBlock::~NetBlock() {
}

/**
 * The net block is destroyed and replaced by an empty space.
 * Returns: A new empty space block.
 */
LevelPiece* NetBlock::Destroy(GameModel* gameModel) {
	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this);

	// Tell the level that this piece has changed to empty...
	GameLevel* level = gameModel->GetCurrentLevel();
	LevelPiece* emptyPiece = new EmptySpaceBlock(this->wIndex, this->hIndex);
	level->PieceChanged(this, emptyPiece);

	// Obliterate all that is left of this block...
	LevelPiece* tempThis = this;
	delete tempThis;
	tempThis = NULL;

	return emptyPiece;
}

// The net block will capture the colliding ball and send it barrelling downwards (in the direction of objective gravity)
LevelPiece* NetBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	// Make the ball fall directly downwards
	Vector3D worldGravityDir = gameModel->GetTransformInfo()->GetGameTransform() * Vector3D(0, -1, 0);
	worldGravityDir.Normalize();
	ball.WrappedUpInNetBlock(worldGravityDir);

	LevelPiece* resultingPiece = this->Destroy(gameModel);
	ball.SetLastPieceCollidedWith(resultingPiece);
	return resultingPiece;
}

// Currently just lets the projectile pass through, maybe we can shrink it a bit in the future?
LevelPiece* NetBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	// Ignore the collision if this was the last piece collided with
	if (projectile->IsLastLevelPieceCollidedWith(this)) {
		return this;
	}
	
	// Just let the projectile pass through for now...

	projectile->SetLastLevelPieceCollidedWith(this);
	return this;
}

std::list<Collision::Ray2D> NetBlock::GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir) const {
	// The net allows projectiles and rays to pass through itself in the same direction they were travelling...
	std::list<Collision::Ray2D> resultingRay;
	resultingRay.push_back(Collision::Ray2D(hitPoint, impactDir));
	return resultingRay;
}
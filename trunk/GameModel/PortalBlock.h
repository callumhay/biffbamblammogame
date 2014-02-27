/**
 * PortalBlock.h
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

#ifndef __PORTALBLOCK_H__
#define __PORTALBLOCK_H__

#include "LevelPiece.h"

/**
 * Represents the model for a portal block - the piece that can teleport the ball
 * and various other things between itself and its sibling portal block.
 */
class PortalBlock : public LevelPiece {
public:
	PortalBlock(unsigned int wLoc, unsigned int hLoc, PortalBlock* sibling);
	~PortalBlock();

	const PortalBlock* GetSiblingPortal() const {
		return this->sibling;
	}
	PortalBlock* GetSiblingPortal() {
		return this->sibling;
	}
	void SetSiblingPortal(PortalBlock* sibling) {
		assert(sibling != NULL);
		this->sibling = sibling;
	}

	LevelPieceType GetType() const { 
		return LevelPiece::Portal;
	}

    bool ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const {
        UNUSED_PARAMETER(b);
        return false;
    }

	// Is this piece one without any boundries (i.e., no collision surface/line)?
	// Return: true if non-collider, false otherwise.
	bool IsNoBoundsPieceType() const {
		return true;
	}
	bool BallBouncesOffWhenHit() const {
		return false;
	}	

	// Doesn't need to be destroyed to end the level - it's more just an
	// obstruction to make it harder if it's hit
	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyedByBall() const {
		return false;
	}
    bool CanChangeSelfOrOtherPiecesWhenHit() const {
        return false;
    }

	// Whether or not the ball can just blast right through this block.
	bool BallBlastsThrough(const GameBall& b) const {
        UNUSED_PARAMETER(b);
		// All balls pass through the portal
        return true;
	}

	// Whether or not the ghost ball can just pass through this block.
	bool GhostballPassesThrough() const {
		return false;
	}

	// Particles pass through portal blocks.
	bool ProjectilePassesThrough(const Projectile* projectile) const {
		UNUSED_PARAMETER(projectile);
		return true;
	}

    int GetPointsOnChange(const LevelPiece& changeToPiece) const {
        UNUSED_PARAMETER(changeToPiece);
        return 0;
    }

	// Portal blocks change light...
	bool IsLightReflectorRefractor() const {
		return true;
	}

	// Portal blocks are never destroyed.
	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
		UNUSED_PARAMETER(gameModel);
        UNUSED_PARAMETER(method);
		return this;
	}

    bool SecondaryCollisionCheck(double dT, const GameBall& ball) const;
	bool CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, 
        double& timeUntilCollision, Point2D& pointOfCollision) const;
	bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	bool CollisionCheck(const BoundingLines& boundingLines, double dT, const Vector2D& velocity) const;

	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                      const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                      const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                      const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
    LevelPiece* CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	void GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const;

    void DrawWireframe() const;

	static void ResetPortalColourGenerator();
	static Colour GeneratePortalColour();

protected:
    static const unsigned long TIME_BETWEEN_BALL_USES_IN_MILLISECONDS;
	PortalBlock* sibling;

	static bool portalGeneratorReset;
    unsigned long timeOfLastBallCollision;

    DISALLOW_COPY_AND_ASSIGN(PortalBlock);
};

inline bool PortalBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	return this->bounds.CollisionCheck(ray, rayT);	
}

inline bool PortalBlock::CollisionCheck(const BoundingLines& boundingLines, double dT, const Vector2D& velocity) const {
	return this->bounds.CollisionCheck(boundingLines, dT, velocity);
}

inline void PortalBlock::DrawWireframe() const {
    // Draw the portal as an ellipse...
    glPushMatrix();
    glTranslatef(this->GetCenter()[0], this->GetCenter()[1], 0.0f);
    glScalef(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT, 1.0f);
    GeometryMaker::GetInstance()->DrawLineCircle();
    glPopMatrix();
}

#endif // __PORTALBLOCK_H__
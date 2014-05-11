/**
 * PrismBlock.h
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

#ifndef __PRISMBLOCK_H__
#define __PRISMBLOCK_H__

#include "LevelPiece.h"
#include "Projectile.h"

/**
* Represents a prism block in the game. Prism blocks are not destroyable,
* they are instead used to refract and reflect other effects. For example projectiles
* and beams.
*/
class PrismBlock : public LevelPiece {

public:
    static const float REFLECTION_REFRACTION_SPLIT_ANGLE;

    PrismBlock(unsigned int wLoc, unsigned int hLoc);
    virtual ~PrismBlock();

    virtual LevelPieceType GetType() const { 
        return LevelPiece::Prism;
    }

    bool IsExplosionStoppedByPiece(const Point2D&) {
        return true;
    }

    bool ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const {
        UNUSED_PARAMETER(b);    
        return true;
    }

    // Is this piece one without any boundries (i.e., no collision surface/line)?
    // Return: true if non-collider, false otherwise.
    bool IsNoBoundsPieceType() const {
        return false;
    }
    bool BallBouncesOffWhenHit() const {
        return true;
    }

    // Prism blocks can NEVER be destroyed...
    bool MustBeDestoryedToEndLevel() const {
        return false;
    }
    bool CanBeDestroyedByBall() const {
        return false;
    }
    bool CanChangeSelfOrOtherPiecesWhenHit() const {
        return false;
    }

    // Whether or not the uber ball can just blast right through this block.
    // Returns: true if it can, false otherwise.
    bool BallBlastsThrough(const GameBall& b) const {
        UNUSED_PARAMETER(b);
        return false;	// Cannot pass through prism blocks...
    }

    // Whether or not the ghost ball can just pass through this block.
    // Returns: true if it can, false otherwise.
    bool GhostballPassesThrough() const {
        return true; // Ghost ball can float through them, why not?
    }

    bool ProjectilePassesThrough(const Projectile* projectile) const {
        return projectile->IsRefractableOrReflectable();
    }

    int GetPointsOnChange(const LevelPiece& changeToPiece) const {
        UNUSED_PARAMETER(changeToPiece);
        return 0;
    };

    // Prism blocks reflect and refract light.
    // Returns: true
    bool IsLightReflectorRefractor() const {
        return true;
    }

    LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);
    virtual void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
        const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
        const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
        const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

    // Doesn't matter if a ball collides with a prism block, it does nothing to the block.
    LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball) {
        UNUSED_PARAMETER(gameModel);

        // Tell the ball what the last piece it collided with was...
        ball.SetLastPieceCollidedWith(NULL);
        return this;
    }
    LevelPiece* CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle) {
        UNUSED_PARAMETER(gameModel);
        UNUSED_PARAMETER(paddle);
        return this;
    }

    bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;

    virtual LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
    virtual void GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const;

    //protected:
    //	BoundingLines reflectRefractBounds;	// Bounds used for reflection/refraction of beams
};

inline bool PrismBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	return this->bounds.CollisionCheck(ray, rayT);
}

#endif // __PRISMBLOCK_H__
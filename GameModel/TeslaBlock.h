/**
 * TeslaBlock.h
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

#ifndef __TESLABLOCK_H__
#define __TESLABLOCK_H__

#include "LevelPiece.h"

class GameLevel;

class TeslaBlock : public LevelPiece {
    friend class GameLevel;
public:
	static const float LIGHTNING_ARC_RADIUS;

	TeslaBlock(bool isActive, bool isChangable, unsigned int wLoc, unsigned int hLoc);
	~TeslaBlock();

	LevelPieceType GetType() const { 
		return LevelPiece::Tesla;
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

	// Tesla blocks can NEVER be destroyed...
	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyedByBall() const {
		return false;
	}
    bool CanChangeSelfOrOtherPiecesWhenHit() const {
        return true;
    }

	// Whether or not the uber ball can just blast right through this block.
	// Returns: true if it can, false otherwise.
	bool BallBlastsThrough(const GameBall& b) const {
        UNUSED_PARAMETER(b);
		return false;	// Cannot pass through tesla blocks...
	}

	// Whether or not the ghost ball can just pass through this block.
	// Returns: true if it can, false otherwise.
	bool GhostballPassesThrough() const {
        return this->isChangable;
	}

	// Tesla blocks do not reflect or refract light.
	// Returns: false
	bool IsLightReflectorRefractor() const {
		return false;
	}

	// Determine whether the given projectile will pass through this block...
	bool ProjectilePassesThrough(const Projectile* projectile) const {
		UNUSED_PARAMETER(projectile);
		return false;
	}

    int GetPointsOnChange(const LevelPiece& changeToPiece) const {
        UNUSED_PARAMETER(changeToPiece);
        return 0;
    }

    // Nothing ever destroys a tesla block, but it can be triggered...
	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);

    void Triggered(GameModel* gameModel);

	bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;

	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
										const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
										const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
										const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
    LevelPiece* CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);

	LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);

	// Tesla Block Specific Functionality
	void SetConnectedTeslaBlockList(const std::list<TeslaBlock*>& teslaBlocks);
	const std::list<TeslaBlock*>& GetConnectedTeslaBlockList() const;

	bool GetIsElectricityActive() const;
	bool GetIsChangable() const;
	std::list<TeslaBlock*> GetLightningArcTeslaBlocks() const;
	std::list<TeslaBlock*> GetActiveConnectedTeslaBlocks() const;

	const Point2D& GetLightningOrigin() const;
    
    float GetRotationAmount() const;
    void SetRotationAmount(float rotAmt);

    const Vector3D& GetRotationAxis() const;

    void ToggleElectricity(GameModel& gameModel, GameLevel& level, bool ignoreIsChangable = false);

private:
    
    static const int MIN_TIME_BETWEEN_TOGGLINGS_IN_MS = 100;

	bool electricityIsActive;                     // Whether the lightning/electric current is active on this tesla block
	bool isChangable;                             // Whether the state (on/off) of the tesla block can be changed via game play
	std::list<TeslaBlock*> connectedTeslaBlocks;  // All tesla blocks that this one can connect to when active (and when they're active)

	static const int TOGGLE_ON_OFF_LIFE_POINTS;	
	float lifePointsUntilNextToggle;              // Current life points left of this block before it toggles between on/off

    unsigned long timeOfLastToggling;

    Vector3D rotationAxis;
    float rotationAmt;

    void SetRandomRotationAxis();
    void SetRandomRotationAmount();

    void SetElectricityIsActive(bool isActive);
    void SetIsChangable(bool isChangable);
};

inline bool TeslaBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	return Collision::IsCollision(ray, this->GetAABB(), rayT);
}

// Sets the list of Tesla blocks (not including this one) that this tesla block will connect
// to when it's active and they're active.
inline void TeslaBlock::SetConnectedTeslaBlockList(const std::list<TeslaBlock*>& teslaBlocks) {
	this->connectedTeslaBlocks = teslaBlocks;
}
inline const std::list<TeslaBlock*>& TeslaBlock::GetConnectedTeslaBlockList() const {
	return this->connectedTeslaBlocks;
}

inline void TeslaBlock::SetElectricityIsActive(bool isActive) {
	this->electricityIsActive = isActive;
}

// Query whether this tesla block is active - i.e., will connect to any connected tesla blocks
// that are also active with an arc of lightning
inline bool TeslaBlock::GetIsElectricityActive() const {
	return this->electricityIsActive;
}

inline void TeslaBlock::SetIsChangable(bool isChangable) {
	this->isChangable = isChangable;
}

// Query whether this tesla block can be turned on/off by the player
inline bool TeslaBlock::GetIsChangable() const {
	return this->isChangable;
}

// Query the set of tesla blocks connected to this block that are active along with this one
// which are currently forming an arc of lightning/electricity between themselves and this block.
inline std::list<TeslaBlock*> TeslaBlock::GetLightningArcTeslaBlocks() const {
	std::list<TeslaBlock*> activeConnections;
	// First check to see if any electricity is active for this block, 
	// if not then there are no active connections at all
	if (!this->GetIsElectricityActive()) {
		return activeConnections;
	}

	for (std::list<TeslaBlock*>::const_iterator iter = this->connectedTeslaBlocks.begin(); iter != this->connectedTeslaBlocks.end(); ++iter) {
		TeslaBlock* currBlock = *iter;
		if (currBlock->GetIsElectricityActive()) {
			activeConnections.push_back(currBlock);
		}
	}

	return activeConnections;
}

// Query the set of tesla blocks that are connected to this block and that are active (whether this
// block is active or not).
inline std::list<TeslaBlock*> TeslaBlock::GetActiveConnectedTeslaBlocks() const {
	std::list<TeslaBlock*> activeConnections;

	for (std::list<TeslaBlock*>::const_iterator iter = this->connectedTeslaBlocks.begin(); iter != this->connectedTeslaBlocks.end(); ++iter) {
		TeslaBlock* currBlock = *iter;
		if (currBlock->GetIsElectricityActive()) {
			activeConnections.push_back(currBlock);
		}
	}

	return activeConnections;
}

// Get the point where the lighting shoots into/out of for this tesla block
inline const Point2D& TeslaBlock::GetLightningOrigin() const {
	return this->GetCenter();
}

inline float TeslaBlock::GetRotationAmount() const {
    return this->rotationAmt;
}
inline void TeslaBlock::SetRotationAmount(float rotAmt) {
    this->rotationAmt = fmod(rotAmt, 360.0f);
}

inline const Vector3D& TeslaBlock::GetRotationAxis() const {
    return this->rotationAxis;
}

inline void TeslaBlock::SetRandomRotationAxis() {
    this->rotationAxis = Vector3D(
        Randomizer::GetInstance()->RandomNumNegOneToOne(),
        Randomizer::GetInstance()->RandomNumNegOneToOne(), 0.0f);

    if (this->rotationAxis.IsZero()) {
        this->rotationAxis[0] = 1.0f;
        this->rotationAxis[1] = 1.0f;
    }
    this->rotationAxis.Normalize();
}

inline void TeslaBlock::SetRandomRotationAmount() {
    this->rotationAmt = static_cast<float>(Randomizer::GetInstance()->RandomNumZeroToOne() * 360.0f);
}

#endif // __TESLABLOCK_H__
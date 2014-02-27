/**
 * SwitchBlock.h
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

#ifndef __SWITCHBLOCK_H__
#define __SWITCHBLOCK_H__

#include "LevelPiece.h"

class SwitchBlock : public LevelPiece {
public:
    static const float RESET_TIME_IN_MS;
    static const float SWITCH_WIDTH;
    static const float SWITCH_HEIGHT;

    SwitchBlock(const LevelPiece::TriggerID& idToTriggerOnSwitch, unsigned int wLoc, unsigned int hLoc);
	~SwitchBlock();

	LevelPieceType GetType() const;

    bool IsExplosionStoppedByPiece(const Point2D&) {
        return false;
    }

    bool ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const;
	bool IsNoBoundsPieceType() const;
	bool BallBouncesOffWhenHit() const;
	bool MustBeDestoryedToEndLevel() const;
	bool CanBeDestroyedByBall() const;
    bool CanChangeSelfOrOtherPiecesWhenHit() const;
	bool BallBlastsThrough(const GameBall& b) const;
	bool GhostballPassesThrough() const;
	bool IsLightReflectorRefractor() const;
	bool ProjectilePassesThrough(const Projectile* projectile) const;
    
    int GetPointsOnChange(const LevelPiece& changeToPiece) const {
        UNUSED_PARAMETER(changeToPiece);
        return 0;
    }

    void Triggered(GameModel* gameModel);

	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);

    bool CollisionCheck(const BoundingLines& boundingLines, double dT, const Vector2D& velocity) const;
	bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                      const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                      const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                      const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
    LevelPiece* CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
    
	LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);

    bool GetIsSwitchOn() const;
    const LevelPiece::TriggerID& GetIDTriggeredBySwitch() const { return this->idToTriggerOnSwitch; }

    bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses);

private:
    static const int TOGGLE_ON_OFF_LIFE_POINTS;
    
    float lifePointsUntilNextToggle;
    unsigned long timeOfLastSwitchPress;
    LevelPiece::TriggerID idToTriggerOnSwitch;
    
    void SwitchPressed(GameModel* gameModel);

    DISALLOW_COPY_AND_ASSIGN(SwitchBlock);
};

inline LevelPiece::LevelPieceType SwitchBlock::GetType() const { 
    return LevelPiece::Switch;
}
inline bool SwitchBlock::ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const {
    UNUSED_PARAMETER(b);
    return true;
}
inline bool SwitchBlock::IsNoBoundsPieceType() const {
	return false;
}
inline bool SwitchBlock::BallBouncesOffWhenHit() const {
	return true;
}
inline bool SwitchBlock::MustBeDestoryedToEndLevel() const {
	return false;
}
inline bool SwitchBlock::CanBeDestroyedByBall() const {
	return false;
}
inline bool SwitchBlock::CanChangeSelfOrOtherPiecesWhenHit() const {
    return false;
}
inline bool SwitchBlock::BallBlastsThrough(const GameBall& b) const {
    UNUSED_PARAMETER(b);
	return false;
}
inline bool SwitchBlock::GhostballPassesThrough() const {
	return true;
}
inline bool SwitchBlock::IsLightReflectorRefractor() const {
	return false;
}

inline void SwitchBlock::Triggered(GameModel* gameModel) {
    // When a switch block is triggered it gets its switch activated
    this->SwitchPressed(gameModel);
}

// Nothing can destroy a switch block.
inline LevelPiece* SwitchBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
    UNUSED_PARAMETER(method);
    this->Triggered(gameModel);
	return this;
}

inline bool SwitchBlock::CollisionCheck(const BoundingLines& boundingLines, double dT, const Vector2D& velocity) const {
    return this->bounds.CollisionCheck(boundingLines, dT, velocity);
}

inline bool SwitchBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
    return Collision::IsCollision(ray, this->GetAABB(), rayT);
}

inline bool SwitchBlock::GetIsSwitchOn() const {
    unsigned long currSysTime = BlammoTime::GetSystemTimeInMillisecs();
    if (currSysTime - this->timeOfLastSwitchPress < SwitchBlock::RESET_TIME_IN_MS) {
        // Do nothing, need to wait for the switch to reset
        return true;
    }
    return false;
}

inline bool SwitchBlock::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
	UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(gameModel);
	assert(gameModel != NULL);

	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return false;
}

#endif // __SWITCHBLOCK_H__
/**
 * SwitchBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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
	bool IsNoBoundsPieceType() const;
	bool BallBouncesOffWhenHit() const;
	bool MustBeDestoryedToEndLevel() const;
	bool CanBeDestroyedByBall() const;
    bool CanChangeSelfOrOtherPiecesWhenHitByBall() const;
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

	bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                      const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                      const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                      const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
    LevelPiece* CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);

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
inline bool SwitchBlock::CanChangeSelfOrOtherPiecesWhenHitByBall() const {
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
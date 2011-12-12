/**
 * RocketTurretBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __ROCKETTURRETBLOCK_H__
#define __ROCKETTURRETBLOCK_H__

#include "LevelPiece.h"

class RocketTurretBlock : public LevelPiece {
public:
    static const float BALL_DAMAGE_AMOUNT;
    static const float ONE_MORE_BALL_HIT_LIFE_PERCENT;
    static const float ROCKET_HOLE_RADIUS;

    static const float BARREL_OFFSET_EXTENT_ALONG_X;
    static const float BARREL_OFFSET_EXTENT_ALONG_Y;
    static const float BARREL_OFFSET_EXTENT_ALONG_Z;

    enum TurretAIState { IdleTurretState = 0, SeekingTurretState, TargetFoundTurretState, TargetLostTurretState };

	RocketTurretBlock(unsigned int wLoc, unsigned int hLoc);
	~RocketTurretBlock();

	LevelPieceType GetType() const { return LevelPiece::RocketTurret; }

	bool IsNoBoundsPieceType() const { return false; }
	bool BallBouncesOffWhenHit() const { return true; }
	bool MustBeDestoryedToEndLevel() const { return false; }
	bool CanBeDestroyedByBall() const { return true; }
    bool CanChangeSelfOrOtherPiecesWhenHitByBall() const { return true; }
    bool BallBlastsThrough(const GameBall& b) const { UNUSED_PARAMETER(b); return false; }
    bool GhostballPassesThrough() const { return false; }

    bool IsLightReflectorRefractor() const {
		// When frozen in ice a block can reflect/refract lasers and the like
		return this->HasStatus(LevelPiece::IceCubeStatus);
	}

	bool ProjectilePassesThrough(Projectile* projectile) const;
    int GetPointsOnChange(const LevelPiece& changeToPiece) const;

    bool IsAIPiece() const { return true; }

	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                      const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                      const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                      const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);


	// Collision related stuffs
	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);	
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
	LevelPiece* TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel);

	bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses); 

    void AITick(double dT, GameModel* gameModel);

    void GetBarrelInfo(Point3D& endOfBarrelPt, Vector2D& barrelDir) const;
    void GetFiringDirection(Vector2D& unitDir) const;
    float GetRotationDegreesFromX() const;
    float GetBarrelRecoilAmount() const;
    float GetHealthPercent() const;

    bool HasRocketLoaded() const {
        return (this->barrelState == BarrelForwardRocketLoading || this->barrelState == BarrelForwardRocketLoaded);
    }
    float GetRocketTranslationFromCenter() const {
        return this->rocketMovementAnim.GetInterpolantValue();
    }

    RocketTurretBlock::TurretAIState GetTurretAIState() const;

private:
    static const int POINTS_ON_BLOCK_DESTROYED  = 800;
    static const int PIECE_STARTING_LIFE_POINTS = 600;

    static const float MAX_ROTATION_SPEED_IN_DEGS_PER_SEC;
    static const float ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD;
    static const float BARREL_RECOIL_TRANSLATION_AMT;
    static const float ROCKET_DISPLACEMENT_PRE_LOAD;
    static const float ROCKET_DISPLACEMENT_ON_LOAD;

    static const float FIRE_RATE_IN_ROCKETS_PER_SEC;
    static const float BARREL_RELOAD_TIME;
    static const float BARREL_RECOIL_TIME;
    static const float ROCKET_LOAD_TIME;

    static const double LOST_AND_FOUND_MIN_SEEK_TIME;
    static const double LOST_AND_FOUND_MAX_SEEK_TIME;

    static const int LOST_AND_FOUND_MIN_NUM_LOOK_TIMES;
    static const int LOST_AND_FOUND_MAX_NUM_LOOK_TIMES;

    enum BarrelAnimationState { BarrelForwardRocketLoading = 0, BarrelForwardRocketLoaded,
                                BarrelRecoiling, BarrelBack, BarrelReloading };

    float currLifePoints;
    TurretAIState currTurretState;
    BarrelAnimationState barrelState;

    // Turret-and-appearence-related variables
    float currRotationFromXInDegs;
    float currRotationSpd;
    float currRotationAccel;

    double lostAndFoundTimeCounter;
    double nextLostAndFoundSeekTime;
    int numSearchTimesCounter;
    int numTimesToSearch;

    AnimationLerp<float> barrelMovementAnim;
    AnimationLerp<float> rocketMovementAnim;

    // AI update private members
    bool ExecutePaddleSeekingAI(double dT, const GameModel* model);
    bool ExecuteContinueTrackingAI(double dT, const GameModel* model);
    bool ContinueTrackingStateUpdateFromView(const GameModel* model);
    bool ExecuteLostAndFoundAI(double dT, const GameModel* model);

    void SetTurretState(const TurretAIState& state);
    void SetBarrelState(const BarrelAnimationState& state, GameModel* model);
    void UpdateBarrelState(double dT, bool isAllowedToFire, GameModel* model);

    bool IsDead() const { return this->currLifePoints <= 0; }
    
    void CanSeeAndFireAtPaddle(const GameModel* model, bool& canSeePaddle, bool& canFireAtPaddle) const;
    LevelPiece* DiminishPiece(float dmgAmount, GameModel* model, const LevelPiece::DestructionMethod& method);
    void UpdateSpeed();

    DISALLOW_COPY_AND_ASSIGN(RocketTurretBlock);
};

inline bool RocketTurretBlock::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
	UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(gameModel);
	assert(gameModel != NULL);

	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return false;
}

inline float RocketTurretBlock::GetRotationDegreesFromX() const {
    return this->currRotationFromXInDegs;
}
inline float RocketTurretBlock::GetBarrelRecoilAmount() const {
    return this->barrelMovementAnim.GetInterpolantValue();
}

inline float RocketTurretBlock::GetHealthPercent() const {
    return this->currLifePoints / static_cast<float>(PIECE_STARTING_LIFE_POINTS);
}

inline RocketTurretBlock::TurretAIState RocketTurretBlock::GetTurretAIState() const {
    return this->currTurretState;
}
#endif // __ROCKETTURRETBLOCK_H__
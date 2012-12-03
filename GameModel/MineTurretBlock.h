/**
 * MineTurretBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __MINETURRETBLOCK_H__
#define __MINETURRETBLOCK_H__

#include "TurretBlock.h"

class MineTurretBlock : public TurretBlock {
public:
    static const float BALL_DAMAGE_AMOUNT;
    static const float ONE_MORE_BALL_HIT_LIFE_PERCENT;

    static const float BARREL_OFFSET_EXTENT_ALONG_X;
    static const float BARREL_OFFSET_EXTENT_ALONG_Y;
    static const float BARREL_OFFSET_EXTENT_ALONG_Z;

    enum TurretAIState { IdleTurretState = 0, SeekingTurretState, TargetFoundTurretState,
                         /*ProxFireTurretState, */ TargetLostTurretState };
    
    MineTurretBlock(unsigned int wLoc, unsigned int hLoc);
	~MineTurretBlock();

	LevelPieceType GetType() const { return LevelPiece::MineTurret; }

	bool ProjectilePassesThrough(const Projectile* projectile) const;
    int GetPointsOnChange(const LevelPiece& changeToPiece) const;

	// Collision related stuffs
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);

    void AITick(double dT, GameModel* gameModel);

    void GetBarrelInfo(Point3D& endOfBarrelPt, Vector2D& barrelDir) const;
    void GetFiringDirection(Vector2D& unitDir) const;
    float GetRotationDegreesFromX() const;
    float GetBarrelRecoilAmount() const;

    bool HasMineLoaded() const {
        return (this->barrelState == BarrelForwardMineLoading || this->barrelState == BarrelForwardMineLoaded);
    }
    float GetMineTranslationFromCenter() const {
        return this->mineMovementAnim.GetInterpolantValue();
    }

    MineTurretBlock::TurretAIState GetTurretAIState() const;

private:
    static const int POINTS_ON_BLOCK_DESTROYED  = 800;
    static const int PIECE_STARTING_LIFE_POINTS = 500;

    static const float MAX_ROTATION_SPEED_IN_DEGS_PER_SEC;
    static const float ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD;
    static const float BARREL_RECOIL_TRANSLATION_AMT;
    static const float MINE_DISPLACEMENT_PRE_LOAD;
    static const float MINE_DISPLACEMENT_ON_LOAD;

    static const float FIRE_RATE_IN_MINES_PER_SEC;
    static const float BARREL_RELOAD_TIME;
    static const float BARREL_RECOIL_TIME;
    static const float MINE_LOAD_TIME;

    static const double LOST_AND_FOUND_MIN_SEEK_TIME;
    static const double LOST_AND_FOUND_MAX_SEEK_TIME;

    static const int LOST_AND_FOUND_MIN_NUM_LOOK_TIMES;
    static const int LOST_AND_FOUND_MAX_NUM_LOOK_TIMES;

    enum BarrelAnimationState { BarrelForwardMineLoading = 0, BarrelForwardMineLoaded,
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
    AnimationLerp<float> mineMovementAnim;

    // AI update private members
    bool ExecutePaddleSeekingAI(double dT, const GameModel* model);
    bool ExecuteContinueTrackingAI(double dT, const GameModel* model);
    bool ContinueTrackingStateUpdateFromView(const GameModel* model);
    bool ExecuteLostAndFoundAI(double dT, const GameModel* model);

    void SetTurretState(const TurretAIState& state);
    void SetBarrelState(const BarrelAnimationState& state, GameModel* model);
    void UpdateBarrelState(double dT, bool isAllowedToFire, GameModel* model);

    void CanSeeAndFireAtPaddle(const GameModel* model, bool& canSeePaddle, bool& canFireAtPaddle) const;
    void UpdateSpeed();

    DISALLOW_COPY_AND_ASSIGN(MineTurretBlock);
};

inline float MineTurretBlock::GetRotationDegreesFromX() const {
    return this->currRotationFromXInDegs;
}

inline float MineTurretBlock::GetBarrelRecoilAmount() const {
    return this->barrelMovementAnim.GetInterpolantValue();
}

inline MineTurretBlock::TurretAIState MineTurretBlock::GetTurretAIState() const {
    return this->currTurretState;
}

#endif // __MINETURRETBLOCK_H__